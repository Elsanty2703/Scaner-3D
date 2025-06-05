// generateInputFile.h

#ifndef GENERATE_INPUT_FILE_H
#define GENERATE_INPUT_FILE_H

// Función para guardar una matriz de enteros como archivo de entrada del escáner
// matriz: puntero a arreglo de punteros de enteros (int **)
// filas: número de filas
// columnas: número de columnas
// filename: nombre del archivo a guardar
void generateInputFile(const int **matriz, int filas, int columnas, const char *filename);

#endif
