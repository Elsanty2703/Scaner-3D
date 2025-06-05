#ifndef PROCESS_SCAN_DISTANCE_H
#define PROCESS_SCAN_DISTANCE_H

#include <stdio.h>

// Definición de M_PI si no existe
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MAX_ROWS 500
#define MAX_COLS 500
#define MAX_RAW 250500

// Convierte grados a radianes
float degrees_to_radians(float degrees);

// Procesa el archivo de distancias para generar un archivo STL
void processScanDistance(const char *inputFilename, float zDelta);

#endif // PROCESS_SCAN_DISTANCE_H
