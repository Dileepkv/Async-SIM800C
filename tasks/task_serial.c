#include "task_serial.h"
#include "../lib/common.h"
#include "../lib/serial.h"
#include <stdlib.h>

char buf_serial_recv[128] = {0};
THIS_IS_A_FIFO_BUFFER(buf_serial_recv);

int flag_updated = 0;

// // 声明串口
// char buf_serial_recv[128];
// char buf_serial_show[128];
// char *buf_serial_recv_pw = buf_serial_recv;
// char *buf_serial_recv_pr = buf_serial_recv;

// 串口读取线程，循环读取串口，并显示读取的内容
// TODO: 如果读取的长度超出 8192 会丢弃一部分内容

void task_serial()
{
	STS_START();
	while (1)
	{
	recv:;
		int read_len = serial_read(buf_serial_recv_write_pipe(),
								   buf_serial_recv_write_pipe_len());
		if (read_len)
		{
			// 写指针位移
			buf_serial_recv_wi += read_len;
			// 标记字符串尾部，防止字符串函数出界
			buf_serial_recv[buf_serial_recv_wi] = '\0';
			// 标记数据缓冲区已更新
			// flag_updated = 1;
			// 显示缓冲区数据
			buf_serial_recv_show();
		}
		STS_DOEVENTS();
	}
	STS_ENDED();
}


// char *lastFind = NULL;
void clrbuf()
{
	buf_serial_recv_wi = 0;
	buf_serial_recv[0] = '\0';
	// 重置 Find 函数结果
	// lastFind = NULL;
}
char *Find(char *str)
{
	// return lastFind = (char *)strstr(buf_serial_recv, str);
	buf_serial_recv_read(buf_serial_recv, 1);
	return (char *)strstr(buf_serial_recv, str);
}

// int prefix(const char *pre, const char *str)
// {
// 	if (str == NULL)
// 		return 0;
// 	return strncmp(pre, str, strlen(pre)) == 0;
// }
