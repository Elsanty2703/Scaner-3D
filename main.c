// main.c

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Incluir directamente los archivos fuente
#include "surf2stl.h"
#include "processScanDistance_mfs.h"
#include "generateInputFile.h"  // crea este con la función generateInputFile()

int main() {
    const char *inputFile = "input.txt";
    float zDelta = 10.f;
    // Matriz de ejemplo ya hecha (3 filas x 4 columnas)
    int data[2][5] = {
        {15, 15, 15, 15 ,20},
        {28, 40, 42, 35 , 60}
    };

    // Convertir a puntero doble (int **) para pasarlo a la función
    const int *matrixPointers[2];
    for (int i = 0; i < 2; i++) {
        matrixPointers[i] = data[i];
    }

    printf("Generando archivo de entrada...\n");
    generateInputFile(matrixPointers, 2, 5, "input.txt");

    printf("Procesando escaneo y generando archivo STL...\n");
    processScanDistance(inputFile, zDelta);

    printf("Proceso completo.\n");
    return 0;
}
