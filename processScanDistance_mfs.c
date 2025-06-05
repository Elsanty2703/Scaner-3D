//quiero dormir
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "surf2stl.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MAX_ROWS 500
#define MAX_COLS 500
#define MAX_RAW 250500

float degrees_to_radians(float degrees) {
        return degrees * M_PI / 180.0f;
    }

void processScanDistance(const char *inputFilename , float zDelta) {
    typedef enum {
        OPEN_FILE,
        FILE_LOAD,
        NEGATIVE,
        CREATE_R,
        LOAD_R,
        R_I,
        R_J,
        CREATE_XYZ,
        LOAD_XYZ,
        LIBERATE_MEMORY
    } STATE_T;

    static STATE_T state = OPEN_FILE;

    // Constantes del escáner
    static float centerDistance = 10.3f;
    static float maxDistance = 26.5f;
    static float minDistance = 0.0f;
    // Variables de trabajo
    static int raw[MAX_RAW];
    static int count = 0;

    static float *r[MAX_ROWS];
    static int rows = 0;
    static int cols = 0;

    static float **x = NULL, **y = NULL, **z = NULL;

    static FILE *file = NULL;
    static const char *outputFilename = "output.stl";

    static int i = 0, j = 0;
    static int start = 0, row = 0, len = 0, wait = 0;


    while (state!=LIBERATE_MEMORY) {
        switch (state) {

            case OPEN_FILE:
                count = 0;
                rows = 0;
                cols = 0;
                i = j = start = row = 0;
                file = fopen(inputFilename, "r");
                if (!file) {
                    perror("Error abriendo archivo");
                    state = OPEN_FILE;
                } else {
                    state = FILE_LOAD;
                }
                break;

            case FILE_LOAD:
                if (fscanf(file, "%d", &raw[count]) == 1) {
                    count++;
                    state = NEGATIVE;
                } else {
                    fclose(file);
                    state = CREATE_R;
                }
                break;

            case NEGATIVE:
                if (count < MAX_RAW) {
                    if (raw[count] < 0) {
                        raw[count] = 0;
                        state = FILE_LOAD;
                    } else {
                        state = FILE_LOAD;
                    }
                } else {
                    fclose(file);
                    state = CREATE_R;
                }
                break;

            case CREATE_R:
                if(i<MAX_ROWS) {
                    r[i] = (float*)malloc(MAX_COLS * sizeof(float));
                    i++;
                    state = CREATE_R;

                } else {
                    i=0;
                    state = LOAD_R;
                }
                break;

            case LOAD_R:
                if (i < count) {
                    state = R_I;
                } else {
                    rows = row;
                    cols = (start > 0 && row > 0) ? (start / row) - 1 : 0;
                    i= 0;
                    j = 0;
                    x = malloc(rows * sizeof(float *));
                    y = malloc(rows * sizeof(float *));
                    z = malloc(rows * sizeof(float *));
                    state = CREATE_XYZ;
                }
                break;

            case R_I:
                if(row >= MAX_ROWS){
                    rows = row;
                    cols = (start > 0 && row > 0) ? (start / row) - 1 : 0;
                    i= 0;
                    j = 0;
                    x = malloc(rows * sizeof(float *));
                    y = malloc(rows * sizeof(float *));
                    z = malloc(rows * sizeof(float *));
                    state = CREATE_XYZ;
                }
                if(raw[i] == 9999) {
                    len = i - start;
                    j = 0;
                    state = R_J;
                } else {
                    i++;
                        state = LOAD_R;
                }
                break;

            case R_J:
                if(len > MAX_COLS) {
                    len = MAX_COLS;
                }
                if(j < len){
                    r[row][j] = centerDistance - raw[start + j];
                    j++;printf("i: %d \n", i);
                    state = R_J;
                }
                else{
                    ++i;
                    ++row;
                    start = i + 1;
                    state = R_I;
                }
                break;

            case CREATE_XYZ:

                if(i < rows){
                    x[i] = malloc(cols * sizeof(float));
                    y[i] = malloc(cols * sizeof(float));
                    z[i] = malloc(cols * sizeof(float));
                    j = 0;
                    state = LOAD_XYZ;
                    printf("depuring");
                } else {
                    printf("depuring");
                    surf2stl(outputFilename, x, y, z, rows, cols, "binary");
                    state = LIBERATE_MEMORY;
                }
                break;

            case LOAD_XYZ:
                if(j < cols) {
                    float radius = r[i][j];
                if (radius < minDistance || radius > maxDistance) {
                    radius = NAN;
                }
                float theta = degrees_to_radians(360.0f - (360.0f * j / cols));
                x[i][j] = radius * cosf(theta);
                y[i][j] = radius * sinf(theta);
                z[i][j] = i * zDelta;
                    ++j;
                    state = LOAD_XYZ;
                }
                else{
                    ++i;
                    state = CREATE_XYZ;
                }
                break;

            case LIBERATE_MEMORY:
                for (i = 0; i < rows; i++) {
                    free(x[i]);
                    free(y[i]);
                    free(z[i]);
                    free(r[i]);
                }
                free(x);
                free(y);
                free(z);
                printf("STL generado y memoria liberada correctamente.\n");
                state = OPEN_FILE;
                break;

        }
    }
}
