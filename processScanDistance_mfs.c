// processScanDistance_mfs.c
// Author Thomas Santiago Ramirez Moreno
#include "processScanDistance_mfs.h"
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static float degrees_to_radians(float degrees) {
    return degrees * M_PI / 180.0f;
}

void write_facet(FILE *f, float *p1, float *p2, float *p3) {
    float v1[3], v2[3], normal[3];

    v1[0] = p2[0] - p1[0];
    v2[0] = p3[0] - p1[0];
    v1[1] = p2[1] - p1[1];
    v2[1] = p3[1] - p1[1];
    v1[2] = p2[2] - p1[2];
    v2[2] = p3[2] - p1[2];

    normal[0] = v1[1]*v2[2] - v1[2]*v2[1];
    normal[1] = v1[2]*v2[0] - v1[0]*v2[2];
    normal[2] = v1[0]*v2[1] - v1[1]*v2[0];

    float norm = sqrtf(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
    if (norm != 0) {
        normal[0] /= norm;
        normal[1] /= norm;
        normal[2] /= norm; //normalizar
    }


    fwrite(normal, sizeof(float), 3, f);
    fwrite(p1, sizeof(float), 3, f);
    fwrite(p2, sizeof(float), 3, f);
    fwrite(p3, sizeof(float), 3, f);
    unsigned short attr = 0;
    fwrite(&attr, sizeof(unsigned short), 1, f);

}

void processScanDistance_step(ScanData *data, Surf2STLContext *ctx,FileData *fileData,MatrixData *matrixData) {
    char new_filename[256];
    char new_filenametxt[256];
    switch (data->state) {
        case FOPEN:

            fileData->file = fopen(fileData->filename, "w");
            sprintf(new_filenametxt, "%d_%s", ctx->file_count, fileData->filename);
            fileData->file2 = fopen(new_filenametxt, "w");
            fileData->rows = matrixData->rows;
            fileData->cols = matrixData->cols_ult - 1;
          fileData->matriz = matrixData->matriz;
            ctx->rows = matrixData->rows;
            ctx->cols = matrixData->cols_ult - 1;
            fileData->cols_por_fila = malloc(matrixData->rows * sizeof(int));
            if(!fileData->cols_por_fila) {
                fprintf(stderr, "Error al asignar memoria para cols_por_fila\n");
                exit(EXIT_FAILURE);
            }
            memcpy(fileData->cols_por_fila, matrixData->cols_por_fila, matrixData->rows * sizeof(int));
        if (!fileData->file) {
            perror("Esperando txt");
        }
        else {
            printf("Cargando txt\n");
            data->state = ROWS;
        }
        break;

        case ROWS:
            if (fileData->i < fileData->rows) {
                fileData->j = 0;

                data->state = COLS;
            } else {
               fclose(fileData->file);
                fileData->file = NULL;
                fclose(fileData->file2);
                fileData->file2 = NULL;
                fileData->i = 0;

                data->state = OPEN_FILE; // Fin
            }
        break;

        case COLS:
            if (fileData->j < fileData->cols_por_fila[fileData->i]) {
                fprintf(fileData->file, "%f ", fileData->matriz[fileData->i][fileData->j]);
                fprintf(fileData->file2, "%f ", fileData->matriz[fileData->i][fileData->j]);

                fileData->j++;
            } else {
                fprintf(fileData->file, "9999 \n");
                fprintf(fileData->file2, "9999 \n");
                fileData->i++;

                data->state = ROWS;
            }
        break;
        case OPEN_FILE:
            data->count = data->rows = data->cols = 0;
            data->i = data->j = data->start = data->row = 0;
            data->file = fopen(data->inputFilename, "r");
            if (!data->file) {
                perror("Error abriendo archivo");
                data->state = OPEN_FILE;
            } else {
                printf("txt abierto\n");
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

                // Verificar si ya existen y liberarlos primero
                if(data->x[data->i]) free(data->x[data->i]);
                if(data->y[data->i]) free(data->y[data->i]);
                if(data->z[data->i]) free(data->z[data->i]);

                data->x[data->i] = calloc(data->cols, sizeof(float));
                data->y[data->i] = calloc(data->cols, sizeof(float));
                data->z[data->i] = calloc(data->cols, sizeof(float));
                data->j = 0;
                data->state = LOAD_XYZ;

            } else {

                ctx->x = data->x;
                ctx->y = data->y;
                ctx->z = data->z;

                data->state = F_OPEN;
            }
            break;

        case LOAD_XYZ:
            if (data->j < data->cols) {
                float radius = data->r[data->i][data->j];

                if (isnan(radius) || isinf(radius) || radius < data->minDistance || radius > data->maxDistance) {
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
        case F_OPEN:
            
        sprintf(new_filename, "%d_%s", ctx->file_count, ctx->filename);
        printf("creando stl\n");
        ctx->f = fopen(new_filename, "wb");
        if (!ctx->f) {
            printf("esperando archivo");
        }
        else {
            char header[80] = "Created by Dynamite team";
            fwrite(header, 1, 80, ctx->f);
            int zero = 0;
            fwrite(&zero, sizeof(int), 1, ctx->f); // Placeholder for facet count
            data->state = ROWS_STL;
        }
        break;
        case ROWS_STL:
            if (ctx->rows <= 1 || ctx->cols <= 0) {  // Verificar dimensiones válidas
                fprintf(stderr, "Error: Dimensiones inválidas para STL (rows=%d, cols=%d)\n", ctx->rows, ctx->cols);
                data->state = LIBERATE_MEMORY;
                break;
            }

            if (ctx->i < ctx->rows - 1) {

                ctx->j = 0;
                data->state = COLS_STL;

            } else {
                fseek(ctx->f, 80, SEEK_SET);
                fwrite(&(ctx->nfacets), sizeof(int), 1, ctx->f);
                fclose(ctx->f);

                printf("triangulos cargados a stl: %d \n", ctx->nfacets);
                data->state = LIBERATE_MEMORY;
            }
        break;

        case COLS_STL:
            if (ctx->j < ctx->cols) {
                if (!ctx->x || !ctx->y || !ctx->z) {
                    fprintf(stderr, "Error: Punteros XYZ no inicializados\n");
                    data->state = LIBERATE_MEMORY;
                    break;
                }
                int j_next = (ctx->j + 1) % ctx->cols;

                float p1[3] = {ctx->x[ctx->i][ctx->j],     ctx->y[ctx->i][ctx->j],     ctx->z[ctx->i][ctx->j]};
                float p2[3] = {ctx->x[ctx->i][j_next],    ctx->y[ctx->i][j_next],    ctx->z[ctx->i][j_next]};
                float p3[3] = {ctx->x[ctx->i+1][j_next],  ctx->y[ctx->i+1][j_next],  ctx->z[ctx->i+1][j_next]};
                write_facet(ctx->f, p1, p2, p3);
                ++ctx->nfacets;

                float p4[3] = {ctx->x[ctx->i+1][ctx->j],   ctx->y[ctx->i+1][ctx->j],   ctx->z[ctx->i+1][ctx->j]};
                write_facet(ctx->f, p3, p4, p1);
                ++ctx->nfacets;

                ++ctx->j;

            } else {
                ++ctx->i;
                data->state = ROWS_STL;

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

        data->x = NULL;
        data->y = NULL;
        data->z = NULL;


                ctx->x = NULL;
                ctx->y = NULL;
                ctx->z = NULL;
                ctx->rows = 0;
                ctx->cols = 0;
                ctx->i = 0;
                ctx->j = 0;
                ctx->nfacets = 0;
                ctx->f = NULL;
        fileData->matriz = NULL;
        fileData->rows = 0;
        fileData->cols = 0;
        fileData->i = 0;
        fileData->j = 0;
        fileData->file = NULL;



        printf("STL generado y memoria liberada correctamente.\n");
        ctx->file_count++;
        data->state = FOPEN;
        break;

    }
}
