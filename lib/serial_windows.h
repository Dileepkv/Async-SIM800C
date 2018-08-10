// #include <windows.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <winnt.h>
#pragma once

#define DEBUG_SERIAL_PORT "COM5"

void init_serial();
unsigned int serial_write(char *buffer, unsigned int buffer_length);
unsigned int serial_read(char *buffer, unsigned int buffer_length);
