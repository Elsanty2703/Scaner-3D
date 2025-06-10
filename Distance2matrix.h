//
// Created by User on 8/06/2025.
//

#ifndef DISTANCE2MATRIX_H
#define DISTANCE2MATRIX_H

typedef enum {
    DATA,
    N_ROWS
} MatrixState;

typedef struct {
    MatrixState state;
    float **matriz;
    int rows;
    int cols;
    int *cols_por_fila;  // ← NUEVO: columnas reales por fila
    int cols_ult;
    int contador;
} MatrixData;

void Distance2matrix(MatrixData *data);


#endif //DISTANCE2MATRIX_H
