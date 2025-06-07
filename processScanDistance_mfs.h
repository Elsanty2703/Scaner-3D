// processScanDistance_mfs.h
#ifndef PROCESS_SCAN_DISTANCE_MFS_H
#define PROCESS_SCAN_DISTANCE_MFS_H

#include <stdio.h>

#define MAX_ROWS 500
#define MAX_COLS 500
#define MAX_RAW 250500

typedef enum {
    OPEN_FILE,
    FILE_LOAD,
    NEGATIVE,
    CREATE_R,
    LOAD_R,
    R_I,
    R_J,
    CREATE_XYZ,
    LOAD_XYZ,
    LIBERATE_MEMORY
} ScanState;

typedef struct {
    // Estados
    ScanState state;

    // Parámetros del escáner
    float centerDistance;
    float maxDistance;
    float minDistance;
    float zDelta;

    // Archivos
    FILE *file;
    const char *inputFilename;
    const char *outputFilename;

    // Datos RAW
    int raw[MAX_RAW];
    int count;

    // Matrices de datos
    float *r[MAX_ROWS];
    float **x;
    float **y;
    float **z;

    // Dimensiones
    int rows;
    int cols;

    // Contadores
    int i, j;
    int start, row, len;

} ScanData;

void processScanDistance_step(ScanData *data);

#endif
