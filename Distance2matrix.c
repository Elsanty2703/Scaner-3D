//
// Created by User on 8/06/2025.
//

#include "distance2matrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "processScanDistance_mfs.h"

#define ESPERANDO_LECTURA 6969.0f
// Funciones externas (deben estar definidas en otro archivo)
extern float sensorgamboa();   // Devuelve un float con el dato leído
extern int nuevaFila();        // Devuelve 1 si se debe cambiar de fila

void Distance2matrix(MatrixData *data) {

    float **matriz = data->matriz;
    int fila = data->rows;
    int columna = data->cols;
switch (data->state) {

    case DATA:

        // Asegurar al menos una fila inicial
        if (matriz == NULL) {
            data->matriz = malloc(sizeof(float*));
            data->matriz[0] = NULL;
            matriz = data->matriz; // <- ACTUALIZA
            data->rows = 0;
            data->cols = 0;
        }
        float lectura = sensorgamboa();

        if (lectura == ESPERANDO_LECTURA) {
            data->state = N_ROWS;
        }
        else {
            // Agregar valor a la fila actual
            columna++;
            float* nuevaFilaDatos = realloc(matriz[fila], columna * sizeof(float));
            if (nuevaFilaDatos == NULL) {
                // Error en realloc
                fprintf(stderr, "Error al reasignar memoria para fila %d\n", fila);
                exit(EXIT_FAILURE);
            }
            matriz[fila] = nuevaFilaDatos;
            matriz[fila][columna - 1] = lectura;

            data->cols=columna;
            data->matriz = matriz;
            data->state = DATA;
        }

        break;
        case N_ROWS:
            // Cambio de fila
                if (nuevaFila()) {
                    fila++;

                    matriz = realloc(matriz, (fila + 1) * sizeof(float*));
                    if (matriz == NULL) {
                        fprintf(stderr, "Error al reasignar memoria para matriz\n");
                        exit(EXIT_FAILURE);
                    }
                    matriz[fila] = NULL;
                    columna = 0;
                    data->rows = fila ;
                    data->cols_ult = data->cols;
                    data->cols = 0;
                    data->state = DATA;
                    data->matriz = matriz;
                }
            else {
                data->state = DATA;
            }

            break;
    default:
        fprintf(stderr, "Estado desconocido\n");
    break;



}
}
