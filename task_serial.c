#include <stdlib.h>
#include "task_serial.h"
#include "common.h"
#include "sno_fifo_buffer.h"
#include "printf.h"
// 这句放到最后
#include "serial.h"

/**
 *  定义串口接收缓冲区长度，不要小于返回指令的长度
 */
#define BUF_SERIAL_RECV_LENGTH 32

char buf_serial_recv[BUF_SERIAL_RECV_LENGTH] = {0};
THIS_IS_A_FIFO_BUFFER(buf_serial_recv);

flag_t flag_updated = 0;
void task_serial()
{
	STS_START();
	while (1)
	{
	// recv:
		do
		{
			int read_len;
			read_len = serial_read(buf_serial_recv_write_pipe(),
								   buf_serial_recv_write_pipe_len());
			if (read_len)
			{
				buf_serial_recv_wi += read_len;
				buf_serial_recv[buf_serial_recv_wi] = '\0';
				buf_serial_recv_show();
			}
		} while (0);
		STS_DOEVENTS();
	}
	STS_ENDED();
}
