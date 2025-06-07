#ifndef GENERATE_INPUT_FILE_H
#define GENERATE_INPUT_FILE_H

typedef enum {
    FOPEN,
    ROWS,
    COLS
} FileState;

typedef struct {
    FileState state;
    const int **matriz;
    int rows;
    int cols;
    const char *filename;
    int i;
    int j;
    FILE *file;
} FileData;

void generateInputFile(FileData *fileData);

#endif
