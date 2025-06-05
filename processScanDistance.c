#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "surf2stl.c"  // Asegúrate de tener esta función implementada por separado

#define _USE_MATH_DEFINES
#define MAX_ROWS 500
#define MAX_COLS 500 // Máximo número de filas y columnas
#define MAX_RAW 250500 // Máximo número de datos crudos

float degrees_to_radians(float degrees) {
    return degrees * M_PI / 180.0f;
}

int processScanDistance(const char *inputFilename, const char *outputFilename) { // el nombre del txt de entrada y el stl de salida
    FILE *file = fopen(inputFilename, "r");
    if (!file) {
        perror("Cannot open input file ;(");
        return 1;
    }

    float centerDistance = 10.3f;
    float zDelta = 0.8f;
    float maxDistance = 26.5f;
    float minDistance = 0.0f;

    int raw[MAX_RAW]; // Datos crudos leídos del archivo
    int count = 0;

    // Leer los datos crudos
    while (fscanf(file, "%d", &raw[count]) == 1) {
        if (raw[count] < 0) raw[count] = 0;
        ++count;
        if (count >= MAX_RAW) break;
    }
    fclose(file);

    // Separar por filas con base en los delimitadores 9999
    int start = 0, row = 0; //row es el número de filas procesadas
    float *r[MAX_ROWS];
    for (int i = 0; i < MAX_ROWS; ++i)
        r[i] = (float *)malloc(MAX_COLS * sizeof(float));

    for (int i = 0; i < count; ++i) {
        if (raw[i] == 9999) {
            int len = i - start;
            if (len > MAX_COLS) len = MAX_COLS;
            for (int j = 0; j < len; ++j)
                r[row][j] = centerDistance - raw[start + j];
            ++row;
            if (row >= MAX_ROWS) break;
            start = i + 1;
        }
    }

    int rows = row; //filas reales
    int cols = (start > 0 && row > 0) ? (start / row) - 1 : 0;

    // Asignar memoria para las matrices x, y, z
    float **x = malloc(rows * sizeof(float*));
    float **y = malloc(rows * sizeof(float*));
    float **z = malloc(rows * sizeof(float*)); //filas

    for (int i = 0; i < rows; ++i) {
        x[i] = malloc(cols * sizeof(float));
        y[i] = malloc(cols * sizeof(float));
        z[i] = malloc(cols * sizeof(float)); //columnas
        for (int j = 0; j < cols; ++j) {
            float radius = r[i][j];
            if (radius < minDistance || radius > maxDistance) {
                radius = NAN;
            }

            float theta = degrees_to_radians(360.0f - (360.0f * j / cols));
            x[i][j] = radius * cosf(theta);
            y[i][j] = radius * sinf(theta);
            z[i][j] = i * zDelta;
        }
    }

    // Exportar a STL
    surf2stl(outputFilename, x, y, z, rows, cols, "binary");

    // Liberar memoria
    for (int i = 0; i < rows; ++i) {
        free(x[i]);
        free(y[i]);
        free(z[i]);
        free(r[i]);
    }
    free(x);
    free(y);
    free(z);

    return 0;
}
