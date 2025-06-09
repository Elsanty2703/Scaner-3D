// generateInputFile.c

#include <stdio.h>
#include <stdlib.h>
#include "generateInputFile.h"

void generateInputFile(FileData *fileData) {
    switch (fileData->state) {
        case FOPEN:
            fileData->file = fopen(fileData->filename, "w");
        if (!fileData->file) {
            perror("Esperando txt");
        }
        fileData->state = ROWS;
        break;

        case ROWS:
            if (fileData->i < fileData->rows) {
                fileData->j = 0;
                fileData->state = COLS;
            } else {
                fclose(fileData->file);
                fileData->file = NULL;
                fileData->state = FOPEN; // Fin
            }
        break;

        case COLS:
            if (fileData->j < fileData->cols) {
                fprintf(fileData->file, "%f ", fileData->matriz[fileData->i][fileData->j]);
                fileData->j++;

            } else {
                fprintf(fileData->file, "9999 ");
                fileData->i++;
                fileData->state = ROWS;
            }
        break;
    }
}