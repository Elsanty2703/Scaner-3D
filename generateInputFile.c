// generateInputFile.c

#include <stdio.h>
#include <stdlib.h>
#include "generateInputFile.h"

void generateInputFile(const int **matriz, int filas, int columnas, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error al abrir el archivo para escritura");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < filas; ++i) {
        for (int j = 0; j < columnas; ++j) {
            fprintf(file, "%d ", matriz[i][j]);
        }
        fprintf(file, "9999\n");  // Separador de fila, como en los datos del escáner
    }

    fclose(file);
}