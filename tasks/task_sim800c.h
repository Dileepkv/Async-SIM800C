#include "../lib/sno_fifo_buffer.h"
#pragma once

THIS_IS_A_FIFO_BUFFER_DECLARE(buf_socket_send);
THIS_IS_A_FIFO_BUFFER_DECLARE(buf_socket_recv);

void task_sim800c();
void config_server(char *protocol, char *host, char *port);
