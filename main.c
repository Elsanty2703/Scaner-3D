#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h> // Para usleep
#include "processScanDistance_mfs.h"
#include "distance2matrix.h"

typedef enum {
    FOPEN_G,
    DATA_G,
    SCAN_G,
    END_G
} GlobalState;

int main() {
    char *inputFile = "input.txt";
    char *outputFile = "SCAN.stl";
    float zDelta = 5.f;
    GlobalState global_state = FOPEN_G; // ← Corregido

    MatrixData matrixData = {
        .state = DATA,
        .matriz = NULL,
        .rows = 0,
        .cols = 0
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
        .centerDistance = 14.0f,
        .maxDistance = 26.5f,
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
        switch (global_state) {
            case FOPEN_G:
                printf("Start...\n");
                global_state = DATA_G;
                break;

            case DATA_G:
                Distance2matrix(&matrixData);
                if (matrixData.rows >= 2) {
                    global_state = SCAN_G;
                }

                break;

            case SCAN_G:

                processScanDistance_step(&scan, &ctx, &fileData, &matrixData);
                global_state = DATA_G;


            break;

            default:
                fprintf(stderr, "Estado no reconocido\n");
                global_state = END_G;
                break;
        }
    }

    return 0;
}
