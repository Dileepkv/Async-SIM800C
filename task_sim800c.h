#include "sno_fifo_buffer.h"
#include "task_serial.h"
#pragma once


// #ifdef WINDOWS
THIS_IS_A_FIFO_BUFFER_DECLARE(buf_socket_send);
THIS_IS_A_FIFO_BUFFER_DECLARE(buf_socket_recv);
// #endif

extern void task_sim800c();
extern void config_server(char *protocol, char *host, char *port);
