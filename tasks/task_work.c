#include "task_work.h"
#include "../lib/sno_task_scheduler.h"
#include "../lib/timer.h"
#include "task_sim800c.h"

extern int flag_socket_ready;
int flag_need_send_data;
int flag_uuid_sended;
int flag_press_button;

// �߼��߳�
void task_work()
{
	STS_START();
	static long timer = 0;

	// ����sim800cģ������Ӷ���
	config_server("TCP", "aliyun.snomiao.com", "80");
	while (1)
	{
		// �ȴ���ͨ������
		STS_WAIT_UNTIL(flag_socket_ready);
		// �� flag_socket_ready ֮�󣬵�һ���������ȶ�һ��
		// DELAY(timer, 1000);// ���˲�����

		// ���ӷ�����
		char *str = "GET /smart-device/device/?uuid="
					"e9e5a455-3ca3-466f-83b3-ad0a10041ee5"
					" HTTP/1.1\r\nhost:aliyun.snomiao.com\r\n\r\n";
		// char *str = "GET / HTTP/1.1\r\n\r\n";
		buf_socket_send_write(str, strlen(str));
		flag_uuid_sended = 1;
		// ��ʼ��������
		printf("[DEBUG   ]"
			   "���������ʼ��������\n");
		while (flag_uuid_sended && flag_socket_ready)
		{
			unsigned char tmp;
			do
			{
				// ƥ��ָ��
				char* lastFind = buf_socket_recv_find("PRESS_BUTTON");
				if (lastFind)
				{
					buf_socket_recv_trimleft(lastFind - buf_socket_recv + strlen("PRESS_BUTTON"));
					// �ɹ�
					printf("[INFO    ]"
						   "�յ�ָ�PRESS_BUTTON");
				}
				// printf("%c", tmp);
				// ���ԳԵ�һ���ֽ�
			} while (buf_socket_recv_read(&tmp, 1));
			STS_WAIT_UNTIL(1);
		}
		STS_WAIT_UNTIL(!flag_uuid_sended || !flag_socket_ready);
		flag_uuid_sended = 0;
		// printf("send..." "GET /im/3724a24a-df43-460d-ae83-924e752c5fb2\r\n");
		// if (!flag_uuid_sended)
		// {
		// 	// printf()
		// 	// int append_to_send_buf(char* data_append, int data_append_length){
		// 	//     while(data_append_length--)
		// 	// }
		// 	// socket_send_buf,
		// 	// TODO: ������������ʹ�� FIFO ����
		// 	STS_WAIT_UNTIL(flag_socket_send_ready);
		// 	DELAY(timer, 5000);
		// 	SOCKET_SEND_STR("GET /im/3724a24a-df43-460d-ae83-924e752c5fb2\r\n");
		// 	wait_timeout_until(10000,
		// 					   (lastFind = strstr(buf_serial_recv,
		// "UUID CHECKED"))); 	if (!lastFind)
		// 	{
		// 		// δ�յ���ȷ�ظ����Ͽ�����
		// 		flag_socket_ready = 0;
		// 		flag_socket_close = 1;
		// 	}
		// 	else
		// 	{
		// 		PRINT_LINE();
		// 		lastFind = NULL;
		// 		flag_uuid_sended = 1;
		// 	}
		// }
		STS_WAIT_UNTIL(1);
	}

	STS_ENDED();
}
