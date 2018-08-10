// #include <windows.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <winnt.h>
#pragma once

#ifdef WINDOWS

#define DEBUG_SERIAL_PORT "COM5"

extern void init_serial();
extern unsigned int serial_write(char *buffer, unsigned int buffer_length);
extern unsigned int serial_read(char *buffer, unsigned int buffer_length);

#endif
