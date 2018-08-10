#include "handle_msg.h"

void task_handle_msg()
{
	// STS_START();
	// while (1)
	// {
	// 	PRINT_LINE();
	// 	STS_WAIT_UNTIL(flag_uuid_sended);
	// 	PRINT_LINE();
	// 	char *pt = strstr(socket_recv_buf, "\n");
	// 	PRINT_LINE();
	// 	if (pt != NULL)
	// 	{
	// 		// TODO: 改掉这暴力的实现。。
	// 		pt[0] = '\0';
	// 		if (0 == strcmp(socket_recv_buf, "press_button"))
	// 		{
	// 			if (!flag_press_button)
	// 			{
	// 				PRINT_LINE();
	// 				flag_press_button = 1;
	// 				SOCKET_SEND_STR("BUTTON_PRESSED");
	// 			}
	// 			else
	// 			{
	// 				PRINT_LINE();
	// 				SOCKET_SEND_STR("BUTTON_CANNOT_PRESS");
	// 			}
	// 		}
	// 		clrbuf();
	// 	}
	// 	STS_WAIT_UNTIL(1);
	// }
	// STS_ENDED();
}
