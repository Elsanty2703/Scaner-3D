//quiero dormir
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "surf2stl.c"

#define MAX_ROWS 500
#define MAX_COLS 500
#define MAX_RAW 250500

void thomas_dot_ramirezm_scanner3d() {
    typedef enum {
        FILE,
        RAWS,
        NEGATIVE,
        CREATE_R,
        LOAD_R,
        R_I,
        R_J,
        CREATE_XYZ,
        LOAD_XYZ,
        LIBERATE_MEMORY,
        WAIT_FOR_INPUT
    } STATE_T;

    static STATE_T state = WAIT_FOR_INPUT;

    // Constantes del escáner
    static float centerDistance = 10.3f;
    static float zDelta = 0.8f;
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
    static const char *inputFilename = "input.txt";
    static const char *outputFilename = "output.stl";

    static int i = 0, j = 0;
    static int start = 0, row = 0;

    float degrees_to_radians(float degrees) {
        return degrees * M_PI / 180.0f;
    }

    if (1) {
        switch (state) {

            case WAIT_FOR_INPUT:
                count = 0;
                rows = 0;
                cols = 0;
                i = j = start = row = 0;
                file = fopen(inputFilename, "r");
                if (!file) {
                    perror("Error abriendo archivo");
                    state = WAIT_FOR_INPUT;
                } else {
                    state = FILE;
                }
                break;

            case FILE:
                if (fscanf(file, "%d", &raw[count]) == 1) {
                    state = RAWS;
                } else {
                    fclose(file);
                    state = CREATE_R;
                }
                break;

            case RAWS:
                if (count < MAX_RAW) {
                    if (raw[count] < 0) {
                        state = NEGATIVE;
                    } else {
                        count++;
                        state = FILE;
                    }
                } else {
                    fclose(file);
                    state = CREATE_R;
                }
                break;

            case NEGATIVE:
                raw[count] = 0;
                count++;
                state = FILE;
                break;

            case CREATE_R:
                for (i = 0; i < MAX_ROWS; i++) {
                    r[i] = malloc(MAX_COLS * sizeof(float));
                }
                i = 0;
                row = 0;
                start = 0;
                state = LOAD_R;
                break;

            case LOAD_R:
                if (i < count) {
                    if (raw[i] == 9999) {
                        int len = i - start;
                        if (len > MAX_COLS) len = MAX_COLS;
                        for (j = 0; j < len; j++) {
                            r[row][j] = centerDistance - raw[start + j];
                        }
                        row++;
                        if (row >= MAX_ROWS) {
                            state = CREATE_XYZ;
                        } else {
                            start = i + 1;
                            state = R_I;
                        }
                    } else {
                        state = R_I;
                    }
                } else {
                    rows = row;
                    cols = (start > 0 && row > 0) ? (start / row) - 1 : 0;
                    state = CREATE_XYZ;
                }
                break;

            case R_I:
                i++;
                state = LOAD_R;
                break;

            case R_J:
                j++;
                state = LOAD_R;
                break;

            case CREATE_XYZ:
                x = malloc(rows * sizeof(float *));
                y = malloc(rows * sizeof(float *));
                z = malloc(rows * sizeof(float *));
                i = 0;
                state = LOAD_XYZ;
                break;

            case LOAD_XYZ:
                if (i < rows) {
                    x[i] = malloc(cols * sizeof(float));
                    y[i] = malloc(cols * sizeof(float));
                    z[i] = malloc(cols * sizeof(float));
                    for (j = 0; j < cols; j++) {
                        float radius = r[i][j];
                        if (radius < minDistance || radius > maxDistance) {
                            radius = NAN;
                        }
                        float theta = degrees_to_radians(360.0f - (360.0f * j / cols));
                        x[i][j] = radius * cosf(theta);
                        y[i][j] = radius * sinf(theta);
                        z[i][j] = i * zDelta;
                    }
                    i++;
                } else {
                    surf2stl(outputFilename, x, y, z, rows, cols, "binary");
                    state = LIBERATE_MEMORY;
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
                state = WAIT_FOR_INPUT;
                break;
        }
    }
}
