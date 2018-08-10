// #include "hardware.h"


// // 硬件控制线程
// void task_hardware()
// {
// 	STS_START();
//     static long timer = 0;
// 	while (1)
// 	{
// 		STS_WAIT_UNTIL(flag_press_button);
// 		printf("PRESSING_BUTTON...");
// 		DELAY(timer, 5000);
// 		printf("PRESS_BUTTON_IS_DONE!");
// 		flag_press_button = 0;
// 	}
// 	STS_ENDED();
// }
