//
// Created by User on 8/06/2025.
//
// Author Thomas Santiago Ramirez Moreno

#include "distance2matrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "bluetooth_windows.c"

#define ESPERANDO_LECTURA 6969.0f



void Distance2matrix(MatrixData *data) {
    float **matriz = data->matriz;

    switch (data->state) {
        case DATA: {
            // Inicialización si es la primera vez
            if (matriz == NULL) {
                data->rows = 0;
                data->cols = 0;
                data->cols_ult = 0;
                data->contador = 0; // Inicializa contador
                data->matriz = malloc(sizeof(float*));
                data->cols_por_fila = malloc(sizeof(int));
                if (!data->matriz || !data->cols_por_fila) {
                    fprintf(stderr, "Error al asignar memoria inicial para matriz o cols_por_fila\n");
                    exit(EXIT_FAILURE);
                }
                data->matriz[0] = NULL;
                matriz = data->matriz;
            }

            float lectura = leerDatoBluetooth();

            // Solo marca como ESPERANDO_LECTURA si es igual a la lectura anterior
            if (lectura == data->lectura_anterior) {
                lectura = ESPERANDO_LECTURA;
            } else {
                data->lectura_anterior = lectura; // actualiza solo si fue diferente
            }
            if (lectura == ESPERANDO_LECTURA) {//por si acaso se imprime algo erroneo
                data->state = DATA;
            } else {
                int fila = data->rows;
                int nueva_columna = data->cols + 1;

                float* nuevaFilaDatos = realloc(matriz[fila], nueva_columna * sizeof(float));
                if (!nuevaFilaDatos) {
                    fprintf(stderr, "Error en realloc para fila Distance2Matrix %d\n", fila);
                    exit(EXIT_FAILURE);
                }

                matriz[fila] = nuevaFilaDatos;
                matriz[fila][nueva_columna - 1] = lectura;
                data->cols = nueva_columna;
                data->cols_por_fila[data->rows] = data->cols;
                data->matriz = matriz;
                data->contador++;  // Aumenta contador
                printf("Distancia guarda:\n", lectura);
                if (data->contador >= 80) {
                    data->state = N_ROWS;
                    data->contador = 0;  // Reinicia contador
                    printf("Fila %d completada con %d columnas.\n", data->rows, data->cols);
                }
            }
            break;
        }

        case N_ROWS: {
                int nueva_fila = data->rows + 1;
                float **newMatriz = realloc(matriz, (nueva_fila + 1) * sizeof(float*));
                // Ampliar cols_por_fila para una fila más
                int *newColsPorFila = realloc(data->cols_por_fila, (nueva_fila + 1) * sizeof(int));
                if (!newMatriz || !newColsPorFila) {
                    fprintf(stderr, "Error al reasignar memoria para cols_por_fila\n");
                    exit(EXIT_FAILURE);
                }
                data->cols_por_fila = newColsPorFila;

                newMatriz[nueva_fila] = NULL;

                data->matriz = newMatriz;
                data->cols_por_fila = newColsPorFila;

                data->rows = nueva_fila;
                data->cols_ult = data->cols;
                data->cols = 0;
                data->state = DATA;

            break;
        }

        default:
            fprintf(stderr, "Estado desconocido\n");
            break;
    }
}
