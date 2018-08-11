/**
 * SIM800C 模块异步通信程序 v2018.08.10
 * Copyright (C) 2018 雪星实验室
 */

#include "sno_fifo_buffer.h"
#include "task_serial.h"
#pragma once

extern flag_t flag_socket_close;
extern flag_t flag_socket_ready;

// #ifdef WINDOWS
THIS_IS_A_FIFO_BUFFER_DECLARE(buf_socket_send);
THIS_IS_A_FIFO_BUFFER_DECLARE(buf_socket_recv);
// #endif

extern void task_sim800c();
extern void config_server(char *protocol, char *host, char *port);
