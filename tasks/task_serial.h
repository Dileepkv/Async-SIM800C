#include "../lib/sno_fifo_buffer.h"
#include "../lib/sno_task_scheduler.h"

#pragma once

THIS_IS_A_FIFO_BUFFER_DECLARE(buf_serial_recv);

void task_serial();
void clrbuf();
char *Find(char *str);
