// Author Thomas Santiago Ramirez Moreno
#include <stdio.h>
#include "processScanDistance_mfs.h"
#include "distance2matrix.h"
#include "bluetooth_windows.c"

typedef enum {
    FOPEN_G,
    DATA_G,
    SCAN_G,
    END_G
} GlobalState;

int main() {
    initBluetooth("COM5");
    char *inputFile = "input.txt";
    char *outputFile = "SCAN.stl";
    float zDelta = 1.f;
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
    while (global_state != END_G) {
        switch (global_state) {
            case FOPEN_G:
                printf("Start...\n");
                global_state = DATA_G;
                break;

            case DATA_G:
                Distance2matrix(&matrixData);
                if (matrixData.rows == 2) {
                    global_state = SCAN_G;
                }
                if (matrixData.rows == 50) {
                    global_state = SCAN_G;
                }
                if (matrixData.rows > 78) {
                    global_state = SCAN_G;
                }

                break;

            case SCAN_G:

                processScanDistance_step(&scan, &ctx, &fileData, &matrixData);
                global_state = DATA_G;
                if (scan.state==FOPEN && matrixData.rows == 79) {
                    global_state = END_G;
                }

            break;
            case END_G:
                printf("finish, BY TEAM DINAMITA\n");
                break;
            default:
                fprintf(stderr, "Estado no reconocido\n");
                global_state = END_G;
                break;
        }
    }
    cerrarBluetooth();
    return 0;
}
