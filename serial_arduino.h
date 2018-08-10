#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
    extern void init_serial();
    extern unsigned int serial_write(char *buffer, unsigned int buffer_length);
    extern unsigned int serial_read(char *buffer, unsigned int buffer_length);
#ifdef __cplusplus
}
#endif
