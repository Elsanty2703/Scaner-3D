// processScanDistance_mfs.c
#include "processScanDistance_mfs.h"
    #include "surf2stl.h"
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static float degrees_to_radians(float degrees) {
    return degrees * M_PI / 180.0f;
}

void processScanDistance_step(ScanData *data, Surf2STLContext *ctx) {
    switch (data->state) {
        case OPEN_FILE:
            data->count = data->rows = data->cols = 0;
            data->i = data->j = data->start = data->row = 0;
            data->file = fopen(data->inputFilename, "r");
            if (!data->file) {
                perror("Error abriendo archivo");
                data->state = OPEN_FILE;
            } else {
                data->state = FILE_LOAD;
            }
            break;

        case FILE_LOAD:
            if (fscanf(data->file, "%f", &data->raw[data->count]) == 1) {
                data->count++;
                data->state = NEGATIVE;
            } else {
                fclose(data->file);
                data->state = CREATE_R;
            }
            break;

        case NEGATIVE:
            if (data->count < MAX_RAW) {
                if (data->raw[data->count] < 0) {
                    data->raw[data->count] = 0;
                }
                data->state = FILE_LOAD;
            } else {
                fclose(data->file);
                data->state = CREATE_R;
            }
            break;

        case CREATE_R:
            if (data->i < MAX_ROWS) {
                data->r[data->i] = malloc(MAX_COLS * sizeof(float));
                data->i++;
                data->state = CREATE_R;
            } else {
                data->i = 0;
                data->state = LOAD_R;
            }
            break;

        case LOAD_R:
            if (data->i < data->count) {
                data->state = R_I;
            } else {
                data->rows = data->row;
                data->cols = (data->start > 0 && data->row > 0) ? (data->start / data->row) - 1 : 0;
                data->i = data->j = 0;
                data->x = calloc(data->rows, sizeof(float *));
                data->y = calloc(data->rows, sizeof(float *));
                data->z = calloc(data->rows, sizeof(float *));
                data->state = CREATE_XYZ;
            }
            break;

        case R_I:
            if (data->row >= MAX_ROWS) {
                data->rows = data->row;
                data->cols = (data->start > 0 && data->row > 0) ? (data->start / data->row) - 1 : 0;
                data->i = data->j = 0;
                data->x = calloc(data->rows, sizeof(float *));
                data->y = calloc(data->rows, sizeof(float *));
                data->z = calloc(data->rows, sizeof(float *));
                data->state = CREATE_XYZ;
                break;
            }

            if (data->raw[data->i] == 9999) {
                data->len = data->i - data->start;
                data->j = 0;
                data->i++;
                data->state = R_J;
            } else {
                data->i++;
                data->state = LOAD_R;
            }
            break;

        case R_J:
            if (data->len > MAX_COLS)
                data->len = MAX_COLS;

            if (data->j < data->len) {
                data->r[data->row][data->j] = data->centerDistance - data->raw[data->start + data->j];
                data->j++;
                data->state = R_J;
            } else {
                data->start = data->i;
                data->row++;
                data->state = R_I;
            }
            break;

        case CREATE_XYZ:
            if (data->i < data->rows) {
                data->x[data->i] = calloc(data->cols, sizeof(float));
                data->y[data->i] = calloc(data->cols, sizeof(float));
                data->z[data->i] = calloc(data->cols, sizeof(float));
                data->j = 0;
                data->state = LOAD_XYZ;
            } else {
                ctx->x = data->x;
                ctx->y = data->y;
                ctx->z = data->z;
                do {
                    surf2stl(ctx);
                }while(ctx->state != F_OPEN);
                data->state = LIBERATE_MEMORY;
            }
            break;

        case LOAD_XYZ:
            if (data->j < data->cols) {
                float radius = data->r[data->i][data->j];
               // printf("radio.%f\n",radius);
                if (radius < data->minDistance || radius > data->maxDistance) {
                    data->x[data->i][data->j] = data->x[(data->i)-1][data->j];
                    data->y[data->i][data->j] = data->y[(data->i)-1][data->j];
                    data->z[data->i][data->j] = data->z[(data->i)-1][data->j];
                    printf("radio fuera de rango.%f \n",radius);

                } else {
                    float theta = degrees_to_radians(360.0f - (360.0f * data->j / data->cols));
                    data->x[data->i][data->j] = radius * cosf(theta);
                    data->y[data->i][data->j] = radius * sinf(theta);
                    data->z[data->i][data->j] = data->i * data->zDelta;

                }
                data->j++;
                data->state = LOAD_XYZ;
            } else {
                data->i++;
                data->state = CREATE_XYZ;
            }
            break;

        case LIBERATE_MEMORY:
            for (int i = 0; i < data->rows; i++) {
                free(data->x[i]);
                free(data->y[i]);
                free(data->z[i]);
                free(data->r[i]);
            }
            free(data->x);
            free(data->y);
            free(data->z);
            printf("STL generado y memoria liberada correctamente.\n");
            data->state = OPEN_FILE;
            break;
    }
}
