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
    FOPEN,//0
    ROWS,//1
    COLS,//2
    //procesamiento de puntos
    OPEN_FILE,//3
    FILE_LOAD,//4
    NEGATIVE,//5
    CREATE_R,//6
    LOAD_R,//7
    R_I,//8
    R_J,//9
    CREATE_XYZ,//10
    LOAD_XYZ,//11
    //stl
    F_OPEN,//12
    ROWS_STL,//13
    COLS_STL,//14
    LIBERATE_MEMORY//15
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
    FILE *file2;
    int *cols_por_fila; // Array to store the number of columns for each row
} FileData;
// Función para escribir un triángulo (facet) en un archivo STL (modo ascii o binario)
void write_facet(FILE *f, float *p1, float *p2, float *p3);

void processScanDistance_step(ScanData *data,Surf2STLContext *ctx,FileData *fileData,MatrixData *matrixData);

#endif
