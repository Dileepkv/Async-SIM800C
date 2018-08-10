
#pragma once

#ifdef WINDOWS
#include "serial_windows.h"
#endif
#ifdef ARDUINO
#include "serial_arduino.h"
#endif


// 串口函数
extern void init_serial();

/**
 *  return length of successfuly read buffer
 */
extern unsigned int serial_read(char *buffer, unsigned int buffer_length);

/**
 *  return length of successfuly write buffer
 */
extern unsigned int serial_write(char *buffer, unsigned int buffer_length);
