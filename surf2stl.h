// surf2stl.h

#ifndef SURF2STL_H
#define SURF2STL_H

#include <stdio.h>


// Función para escribir un triángulo (facet) en un archivo STL (modo ascii o binario)
void write_facet(FILE *f, float *p1, float *p2, float *p3, const char *mode);

// Función principal que genera el archivo STL desde las matrices x, y, z
void surf2stl(const char *filename, float **x, float **y, float **z, int rows, int cols, const char *mode);

#endif // SURF2STL_H