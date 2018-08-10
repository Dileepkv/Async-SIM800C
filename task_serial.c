#include "task_serial.h"
#include "common.h"
#include "serial.h"
#include <stdlib.h>
#pragma once

char buf_serial_recv[128] = {0};
THIS_IS_A_FIFO_BUFFER(buf_serial_recv);

int flag_updated = 0;

// // ��������
// char buf_serial_recv[128];
// char buf_serial_show[128];
// char *buf_serial_recv_pw = buf_serial_recv;
// char *buf_serial_recv_pr = buf_serial_recv;

// ���ڶ�ȡ�̣߳�ѭ����ȡ���ڣ�����ʾ��ȡ������
// TODO: �����ȡ�ĳ��ȳ��� 8192 �ᶪ��һ��������

void task_serial()
{
	STS_START();
	while (1)
	{
	recv:
		do
		{
			int read_len;
			read_len = serial_read(buf_serial_recv_write_pipe(),
								   buf_serial_recv_write_pipe_len());
			if (read_len)
			{
				// дָ��λ��
				buf_serial_recv_wi += read_len;
				// ����ַ���β������ֹ�ַ�����������
				buf_serial_recv[buf_serial_recv_wi] = '\0';
				// ������ݻ������Ѹ���
				// flag_updated = 1;
				// ��ʾ����������
				buf_serial_recv_show();
			}
		} while (0);
		STS_DOEVENTS();
	}
	STS_ENDED();
}


// int prefix(const char *pre, const char *str)
// {
// 	if (str == NULL)
// 		return 0;
// 	return strncmp(pre, str, strlen(pre)) == 0;
// }
