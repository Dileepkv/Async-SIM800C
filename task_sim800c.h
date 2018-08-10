#include "sno_fifo_buffer.h"
#pragma once


#ifdef _WINDOWS_
THIS_IS_A_FIFO_BUFFER_DECLARE(buf_socket_send);
THIS_IS_A_FIFO_BUFFER_DECLARE(buf_socket_recv);
#endif

void task_sim800c();
void config_server(char *protocol, char *host, char *port);
