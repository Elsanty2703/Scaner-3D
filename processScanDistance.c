#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "surf2stl.c" // Incluye surf2stl

#define MAX_ROWS 500
#define MAX_COLS 500

float degrees_to_radians(float degrees) {
    return degrees * M_PI / 180.0;
}

int processScanDistance(const char *inputFilename, const char *outputFilename) {
    FILE *file = fopen(inputFilename, "r");
    if (!file) {
        perror("Cannot open input file");
        return 1;
    }

    float centerDistance = 10.3f;
    float zDelta = 0.1f;
    float maxDistance = 20.0f;
    float minDistance = 0.0f;

    int raw[100000];
    int count = 0;

    while (fscanf(file, "%d", &raw[count]) == 1) {
        if (raw[count] < 0) raw[count] = 0;
        ++count;
        if (count >= 100000) break; // Evita overflow
    }
    fclose(file);

    int start = 0, row = 0;
    float *r[MAX_ROWS];
    for (int i = 0; i < MAX_ROWS; ++i)
        r[i] = (float *)malloc(MAX_COLS * sizeof(float));

    for (int i = 0; i < count; ++i) {
        if (raw[i] == 9999) {
            int len = i - start;
            for (int j = 0; j < len; ++j)
                r[row][j] = centerDistance - raw[start + j];
            ++row;
            start = i + 1;
        }
    }

    int rows = row;
    int cols = (start > 0 && row > 0) ? ((start / row) - 1) : 0;

    float **x = malloc(rows * sizeof(float*));
    float **y = malloc(rows * sizeof(float*));
    float **z = malloc(rows * sizeof(float*));

    for (int i = 0; i < rows; ++i) {
        x[i] = malloc(cols * sizeof(float));
        y[i] = malloc(cols * sizeof(float));
        z[i] = malloc(cols * sizeof(float));
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

    surf2stl(outputFilename, x, y, z, rows, cols, "binary");

    for (int i = 0; i < rows; ++i) {
        free(x[i]); free(y[i]); free(z[i]); free(r[i]);
    }
    free(x); free(y); free(z);

    return 0;  // éxito
}
