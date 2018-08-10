#ifdef _WINDOWS_
#include "serial_windows.h"
#endif
#ifdef _ARDUINO_
#include "serial_arduino.h"
#endif

#pragma once

// 串口函数
void init_serial();

/**
 *  return length of successfuly write buffer
 */
unsigned int serial_write(char *buffer, unsigned int buffer_length);

/**
 *  return length of successfuly read buffer
 */
unsigned int serial_read(char *buffer, unsigned int buffer_length);
