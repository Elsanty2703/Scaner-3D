// Author Thomas Santiago Ramirez Moreno
#include <stdio.h>
#include "processScanDistance_mfs.h"
#include "distance2matrix.h"
#include "serial_reader.h"

typedef enum {
    FOPEN_G,
    DATA_G,
    SCAN_G,
    END_G
} GlobalState;

int main() {
    serial_init("\\\\.\\COM11");
    char *inputFile = "input.txt";
    char *outputFile = "SCAN.stl";
    float zDelta = 0.1f;
    GlobalState global_state = FOPEN_G; // ← Corregido

    MatrixData matrixData = {
        .state = DATA,
        .matriz = NULL,
        .rows = 0,
        .cols = 0,
        .lectura_anterior = 6969.0f
    };

    FileData fileData = {
        .matriz = NULL,
        .rows = 0,
        .cols = 0,
        .filename = inputFile,
        .i = 0,
        .j = 0,
        .file = NULL
    };

    ScanData scan = {
        .state = FOPEN,
        .centerDistance = 14.5f,
        .maxDistance = 22.0f,
        .minDistance = 0.0f,
        .zDelta = zDelta,
        .inputFilename = inputFile,
        .outputFilename = outputFile,
        .file = NULL
    };

    Surf2STLContext ctx = {
        .filename = outputFile,
        .x = NULL,
        .y = NULL,
        .z = NULL,
        .rows = 0,
        .cols = 0,
        .i = 0,
        .j = 0,
        .nfacets = 0,
        .f = NULL,
        .file_count = 1
    };


    // Máquina de estados
    while (1) {
       
                Distance2matrix(&matrixData);
               
                if(matrixData.rows>10){
                                processScanDistance_step(&scan, &ctx, &fileData, &matrixData);
                                printf("Estado del escaneo: %d\n", scan.state);
                }
                

    }
    serial_close();
    return 0;
}
