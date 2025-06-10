#include <windows.h>
#include <stdio.h>

#define ESPERANDO_LECTURA 6969.0f

HANDLE hSerial;

void initBluetooth(const char *comPort) {
    char fullPortName[20];
    sprintf(fullPortName, "\\\\.\\%s", comPort);  // e.g., "COM5" → "\\\\.\\COM5"

    hSerial = CreateFileA(fullPortName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hSerial == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Error abriendo puerto %s\n", comPort);
        exit(EXIT_FAILURE);
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        fprintf(stderr, "Error obteniendo configuración del puerto\n");
        exit(EXIT_FAILURE);
    }

    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity   = NOPARITY;
    SetCommState(hSerial, &dcbSerialParams);

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout         = 50;
    timeouts.ReadTotalTimeoutConstant    = 50;
    timeouts.ReadTotalTimeoutMultiplier  = 10;
    SetCommTimeouts(hSerial, &timeouts);
}

float leerDatoBluetooth() {
    char buffer[16] = {0};
    DWORD bytesRead;
    if (!ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
        return ESPERANDO_LECTURA;
    }

    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        return strtof(buffer, NULL); // convierte a float
    }

    return ESPERANDO_LECTURA;
}

void cerrarBluetooth() {
    if (hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial);
    }
}
//
// Created by User on 10/06/2025.
//
