// processScanDistance_mfs.h
// Author Thomas Santiago Ramirez Moreno
#ifndef PROCESS_SCAN_DISTANCE_MFS_H
#define PROCESS_SCAN_DISTANCE_MFS_H

#include <stdio.h>
#include "Distance2matrix.h"

#define MAX_ROWS 250
#define MAX_COLS 100
#define MAX_RAW 25000

typedef enum {
    //Generacion de archivo
    FOPEN,
    ROWS,
    COLS,
    //procesamiento de puntos
    OPEN_FILE,
    FILE_LOAD,
    NEGATIVE,
    CREATE_R,
    LOAD_R,
    R_I,
    R_J,
    CREATE_XYZ,
    LOAD_XYZ,
    //stl
    F_OPEN,
    ROWS_STL,
    COLS_STL,
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
    char *inputFilename;
    char *outputFilename;

    // Datos RAW
    float raw[MAX_RAW];
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
typedef struct {
    FILE *f;
    char *filename;
    float **x, **y, **z;
    int rows, cols;
    int i, j;
    int nfacets;
    int file_count;
}Surf2STLContext;
typedef struct {
    float **matriz;
    int rows;
    int cols;
    char *filename;
    int i;
    int j;
    FILE *file;
} FileData;
// Función para escribir un triángulo (facet) en un archivo STL (modo ascii o binario)
void write_facet(FILE *f, float *p1, float *p2, float *p3);

void processScanDistance_step(ScanData *data,Surf2STLContext *ctx,FileData *fileData,MatrixData *matrixData);

#endif
