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
    const float **matriz;
    int rows;
    int cols;
    int cols_ult;
} MatrixData;

float sensorgamboa();
int nuevaFila();

void Distance2matrix(MatrixData *data);


#endif //DISTANCE2MATRIX_H
