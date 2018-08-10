#include "task_work.h"
#include "sno_task_scheduler.h"
#include "task_sim800c.h"
#include "timer.h"
#include "common.h"
#include "printf.h"

#define WORK_MODE_PHP
// #define WORK_MODE_PYTHON

extern char buf_socket_recv[];
extern char buf_socket_send[];

extern int flag_socket_ready;
int flag_need_send_data;
int flag_uuid_sended;
int flag_press_button;

// 逻辑线程
void task_work()
{
	STS_START();
	static long timer = 0;

#ifdef WORK_MODE_PHP
	// 配置sim800c模块的连接动作
	while (1)
	{
		config_server((char *)"TCP", (char *)"aliyun.snomiao.com", (char *)"80");
		// 等待接通服务器
		STS_WAIT_UNTIL(flag_socket_ready);
		// 在 flag_socket_ready 之后，等一会让网络稳定一下
		// DELAY(timer, 1000); // 算了不等了

		// 连接服务器
		do
		{
			char *str;
			str = (char *)"GET /smart-device/device/?uuid="
						  "e9e5a455-3ca3-466f-83b3-ad0a10041ee5"
						  " HTTP/1.1\r\nhost:aliyun.snomiao.com\r\n\r\n";
			// char *str = "GET / HTTP/1.1\r\n\r\n";
			buf_socket_send_write(str, strlen(str));
			flag_uuid_sended = 1;
			// 开始处理数据
			printf("[DEBUG   ]"
				   "工作组件开始处理数据\n");
			while (flag_uuid_sended && flag_socket_ready)
			{
				char tmp;
				do
				{
					// 匹配指令
					char *lastFind = buf_socket_recv_find((char*)"PRESS_BUTTON");
					if (lastFind)
					{
						buf_socket_recv_trimleft(lastFind - buf_socket_recv +
												 strlen("PRESS_BUTTON"));
						// 成功
						printf("[INFO    ]"
							   "收到指令：PRESS_BUTTON");
					}
					// printf("%c", tmp);
					// 尝试吃掉一个字节
				} while (buf_socket_recv_read(&tmp, 1));
				STS_WAIT_UNTIL(1);
			}
		} while (0);
		STS_WAIT_UNTIL(!flag_uuid_sended || !flag_socket_ready);
		flag_uuid_sended = 0;
		STS_WAIT_UNTIL(1);
	}
#endif

#ifdef WORK_MODE_PYTHON
	// 配置sim800c模块的连接动作
	config_server((char *)"TCP", (char *)"aliyun.snomiao.com", (char *)"55946");
	while (1)
	{
		// printf("send..." "GET /im/3724a24a-df43-460d-ae83-924e752c5fb2\r\n");
		// if (!flag_uuid_sended)
		// {
		// 	// printf()
		// 	// int append_to_send_buf(char* data_append, int data_append_length){
		// 	//     while(data_append_length--)
		// 	// }
		// 	// socket_send_buf,
		// 	// TODO: 升级缓冲区，使用 FIFO 管理
		// 	STS_WAIT_UNTIL(flag_socket_send_ready);
		// 	DELAY(timer, 5000);
		// 	SOCKET_SEND_STR("GET /im/3724a24a-df43-460d-ae83-924e752c5fb2\r\n");
		// 	wait_timeout_until(10000,
		// 					   (lastFind = strstr(buf_serial_recv,
		// "UUID CHECKED"))); 	if (!lastFind)
		// 	{
		// 		// 未收到正确回复，断开连接
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
		STS_WAIT_UNTIL(!flag_uuid_sended || !flag_socket_ready);
		flag_uuid_sended = 0;
		STS_WAIT_UNTIL(1);
	}
#endif
	STS_ENDED();
}
