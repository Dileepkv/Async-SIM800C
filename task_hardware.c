#include "task_hardware.h"

flag_t flag_press_button = 0;
// // 硬件控制线程
void task_hardware()
{
	STS_START();
    static timer_t timer = 0;
	while (1)
	{
		STS_WAIT_UNTIL(flag_press_button);
		printf("PRESSING_BUTTON...");
        digitalWrite(12, HIGH);		 // sets the digital pin 12 on
		DELAY(timer, 5000);
        digitalWrite(12, LOW);		 // sets the digital pin 12 off
		DELAY(timer, 1000);
		printf("PRESS_BUTTON_IS_DONE!");
		flag_press_button = 0;
	}
	STS_ENDED();
}
