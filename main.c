// main.c

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Archivos fuente
#include "surf2stl.h"
#include "processScanDistance_mfs.h"
#include "generateInputFile.h"  // contiene generateInputFile()
#define ROWS 6
#define COLS 24
int main() {
    const char *inputFile = "input.txt";
    const char *outputFile = "SCAN.stl";
    float zDelta = 5.f;
    int rows = ROWS, cols = COLS;
    // Matriz de ejemplo
    int data[ROWS][COLS] = {
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10}
    };

    // Convertir a punteros para la función de entrada
    const int *matrixPointers[6];
    for (int i = 0; i < 6; i++) {
        matrixPointers[i] = data[i];
    }
    FileData fileData = {
        .state = FOPEN,
        .matriz = matrixPointers,
        .rows = rows,
        .cols = cols,
        .filename = inputFile,
        .i = 0,
        .j = 0,
        .file = NULL
    };

    printf("Generando archivo de entrada...\n");
    do {
        generateInputFile(&fileData);
    } while (fileData.state != FOPEN);

    // Configurar la estructura de estado
    ScanData scan = {
        .state = OPEN_FILE,
        .centerDistance = 14.0f,
        .maxDistance = 26.5f,
        .minDistance = 0.0f,
        .zDelta = zDelta,
        .inputFilename = inputFile,
        .outputFilename = outputFile,
        .file = NULL
    };

    // Ejecutar la máquina de estados hasta que vuelva a OPEN_FILE (indicando que terminó)
    do {
        processScanDistance_step(&scan);
    } while (scan.state != OPEN_FILE );

    printf("Proceso completo.\n");
    return 0;
}
