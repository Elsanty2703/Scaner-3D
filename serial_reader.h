#ifndef SERIAL_READER_H
#define SERIAL_READER_H

int serial_init(const char *port_name);
int serial_read_line(char *buffer, int max_len);
int serial_read_float(float *value);
void serial_close();


#endif // SERIAL_READER_H