// surf2stl.h

#ifndef SURF2STL_H
#define SURF2STL_H

#include <stdio.h>

typedef enum {
    F_OPEN,
    ROWS_STL,
    COLS_STL
} Surf2STLState;

typedef struct {
    Surf2STLState state;
    FILE *f;
    const char *filename;
    float **x, **y, **z;
    int rows, cols;
    int i, j;
    int nfacets;
} Surf2STLContext;

// Función para escribir un triángulo (facet) en un archivo STL (modo ascii o binario)
void write_facet(FILE *f, float *p1, float *p2, float *p3);

// Función principal que genera el archivo STL desde las matrices x, y, z
void surf2stl(Surf2STLContext *ctx);

#endif // SURF2STL_H