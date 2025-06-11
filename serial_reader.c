
#include "serial_reader.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

HANDLE serial_handle = INVALID_HANDLE_VALUE;

// Initialize the serial port
int serial_init(const char *port_name) {
DWORD baud_rate = 115200;
serial_handle = CreateFileA(
port_name, GENERIC_READ | GENERIC_WRITE, 0, NULL,
OPEN_EXISTING, 0, NULL
);

if (serial_handle == INVALID_HANDLE_VALUE) {  
    fprintf(stderr, "Error opening serial port %s\n", port_name);  
    return -1;  
}  

COMMTIMEOUTS timeouts = { 0 };  
timeouts.ReadIntervalTimeout = 50;  
timeouts.ReadTotalTimeoutConstant = 50;  
timeouts.ReadTotalTimeoutMultiplier = 1;  
SetCommTimeouts(serial_handle, &timeouts);  

DCB dcb = { 0 };  
dcb.DCBlength = sizeof(DCB);  
if (!GetCommState(serial_handle, &dcb)) {  
    fprintf(stderr, "Error getting COM state\n");  
    CloseHandle(serial_handle);  
    return -1;  
}  

dcb.BaudRate = baud_rate;  
dcb.ByteSize = 8;  
dcb.StopBits = ONESTOPBIT;  
dcb.Parity   = NOPARITY;  

if (!SetCommState(serial_handle, &dcb)) {  
    fprintf(stderr, "Error setting COM state\n");  
    CloseHandle(serial_handle);  
    return -1;  
}  

return 0;

}

// Read a line from the serial port
int serial_read_line(char *buffer, int max_len) {
if (serial_handle == INVALID_HANDLE_VALUE) {
fprintf(stderr, "Serial port not initialized\n");
return -1;
}

DWORD bytes_read;  
char ch;  
int i = 0;  

while (i < max_len - 1) {  
    BOOL success = ReadFile(serial_handle, &ch, 1, &bytes_read, NULL);  
    if (!success || bytes_read == 0)  
        break;  

    if (ch == '\n')  // end of line  
        break;  

    if (ch != '\r')  // skip carriage return  
        buffer[i++] = ch;  
}  

buffer[i] = '\0';  
return i;

}

// Read a float number from the serial port
int serial_read_float(float *value) {
char line[64];  // increase if longer values are expected
int len = serial_read_line(line, sizeof(line));
if (len <= 0)
return -1;

*value = (float)atof(line);  
return 0;

}

// Close serial port
void serial_close() {
if (serial_handle != INVALID_HANDLE_VALUE) {
CloseHandle(serial_handle);
serial_handle = INVALID_HANDLE_VALUE;
}
}