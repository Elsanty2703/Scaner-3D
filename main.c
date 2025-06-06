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
    float zDelta = 5.f;
    // Matriz de ejemplo
    int data[6][24] = {
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10}
    };

    // Convertir a puntero doble (int **) para pasarlo a la función
    const int *matrixPointers[6];
    for (int i = 0; i < 6; i++) {
        matrixPointers[i] = data[i];
    }

    printf("Generando archivo de entrada...\n");
    generateInputFile(matrixPointers, 6, 24, "input.txt");

    printf("Procesando escaneo y generando archivo STL...\n");
    processScanDistance(inputFile, zDelta);

    printf("Proceso completo.\n");
    return 0;
}
