/**
 * 工作线程 v2018.08.10
 * Copyright (C) 2018 雪星实验室
 */

#include "task_work.h"
#include "common.h"
#include "printf.h"
#include "sno_printf_logging.h"
#include "sno_task_scheduler.h"
#include "task_sim800c.h"
#include "timer.h"
#include "task_hardware.h"

#define DEVICE_UUID "e9e5a455-3ca3-466f-83b3-ad0a10041ee5"

// extern char buf_socket_recv[];
// extern char buf_socket_send[];

flag_t flag_need_send_data;
flag_t flag_hello_sended;

// 逻辑线程

// void task_work_php()
// {
// 	STS_START();
// 	// static timer_t timer = 0;

// 	// 配置sim800c模块的连接动作
// 	config_server((char *)"TCP", (char *)"aliyun.snomiao.com", (char
// *)"80"); 	while (1)
// 	{
// 		// 等待接通服务器
// 		STS_WAIT_UNTIL(flag_socket_ready);
// 		// 在 flag_socket_ready 之后，等一会让网络稳定一下
// 		// DELAY(timer, 1000); // 算了不等了
// 		do
// 		{
// 			// 发送握手数据
// 			static char *str_send;
// 			str_send = (char *)"GET /smart-device/device/?uuid="
// 						  "e9e5a455-3ca3-466f-83b3-ad0a10041ee5"
// 						  "
// HTTP/1.1\r\nhost:aliyun.snomiao.com\r\n\r\n"; 			static uint remain_len;
// 			remain_len = strlen(str_send);
// 			PRINTF_INFO("工作组件开始发送握手数据（%d bytes)\n",
// remain_len);
// 			// 开始发送数据
// 			while (remain_len)
// 			{
// 				{
// 					uint writed_len;
// 					writed_len = buf_socket_send_write(str_send,
// remain_len); 					remain_len -= writed_len; 					str_send += writed_len;

// 					PRINTF_DEBUG("WORK/buf_socket_send\n");
// 					buf_socket_send_show();
// 				}
// 				STS_DOEVENTS();
// 			}
// 			// 标记数据发送完成
// 			flag_hello_sended = 1;
// 			// 开始处理数据
// 			PRINTF_INFO("工作组件开始处理服务器数据\n");
// 			// 如果 socket 断开就结束重来
// 			while (flag_hello_sended && flag_socket_ready)
// 			{
// 				char tmp;
// 				do
// 				{
// 					// 匹配指令
// 					char *lastFind = buf_socket_recv_find((char
// *)"PRESS_BUTTON"); 					if (lastFind)
// 					{
// 						buf_socket_recv_trimleft(lastFind -
// buf_socket_recv + 												 strlen("PRESS_BUTTON"));
// 						// 成功
// 						PRINTF_INFO("收到指令：PRESS_BUTTON");
// 					}
// 					// printf("%c", tmp);
// 					// 尝试吃掉一个字节
// 				} while (buf_socket_recv_read(&tmp, 1));
// 				STS_WAIT_UNTIL(1);
// 			}
// 		} while (0);
// 		// 等待连接断开
// 		STS_WAIT_UNTIL(!flag_hello_sended || !flag_socket_ready);
// 		flag_hello_sended = 0;
// 		STS_WAIT_UNTIL(1);
// 	}
// }

void task_work_python()
{
	STS_START();
	static char *str_send;
	static uint remain_len;
	static timer_t timer_heart_beat;
// static timer_t timer = 0;
// 配置sim800c模块的连接动作
config_connect:
	config_server((char *)"TCP", (char *)"aliyun.snomiao.com", (char *)"55946");
wait_socket_connected:
	flag_hello_sended = 0;
	// 等待接通服务器
	STS_WAIT_UNTIL(flag_socket_ready);

	PRINTF_INFO("工作组件开始发送握手数据\n");
	// 发送握手数据
	str_send = (char *)(DEVICE_UUID "\r\n");
	remain_len = strlen(str_send);
	// 开始发送数据
	while (remain_len)
	{
		do
		{
			uint writed_len;
			writed_len = buf_socket_send_write(str_send, remain_len);
			remain_len -= writed_len;
			str_send += writed_len;

			PRINTF_DEBUG("WORK/buf_socket_send\n");
			buf_socket_send_show();
		} while (0);
		STS_DOEVENTS();
	}
	// 标记数据发送完成
	flag_hello_sended = 1;
	// 开始处理数据
	PRINTF_INFO("工作组件开始处理服务器数据\n");
listening:;
	// 心跳包定时
	SETTIMER(timer_heart_beat);
	// 如果 socket 断开就结束重来
	while (flag_hello_sended && flag_socket_ready)
	{
		char tmp;
		// 心跳包
		if (TIMEOUT(timer_heart_beat, 10000))
		{
			PRINTF_INFO("秒10秒发送心跳包\n");
			// 发送数据
			str_send = (char *)("喵\r\n");
			remain_len = strlen(str_send);
			// 开始发送数据
			while (remain_len)
			{
				do
				{
					uint writed_len;
					writed_len = buf_socket_send_write(str_send, remain_len);
					remain_len -= writed_len;
					str_send += writed_len;
					PRINTF_DEBUG("WORK/buf_socket_send\n");
					buf_socket_send_show();
				} while (0);
				STS_DOEVENTS();
			}
			SETTIMER(timer_heart_beat);
		}
		do
		{
			// 匹配指令
			char *lastFind = buf_socket_recv_find((char *)"PRESS_BUTTON");
			if (lastFind)
			{
				buf_socket_recv_trimleft(lastFind - buf_socket_recv +
										 strlen("PRESS_BUTTON"));
				// 成功
				PRINTF_INFO("收到指令：PRESS_BUTTON\n");
				flag_press_button = 1;
			}
			// printf("%c", tmp);
			// 如果缓冲区满了就扔掉一个字节
			if (!buf_socket_recv_write_pipe_len())
				buf_socket_recv_read(&tmp, 1);
		} while(0);
		STS_WAIT_UNTIL(1);
	}
	goto wait_socket_connected;
	// 等待连接断开
	STS_WAIT_UNTIL(!flag_hello_sended || !flag_socket_ready);
	flag_hello_sended = 0;

	STS_WAIT_UNTIL(1);
	STS_ENDED();
}
void task_work() { task_work_python(); }
