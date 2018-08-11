#include "task_hardware.h"
#include "sno_task_scheduler.h"
#include "timer.h"
#include "common.h"
#include "printf.h"
#include "sno_printf_logging.h"

#define RELAY_PIN 4

flag_t flag_press_button = 0;
// 硬件控制线程

#ifdef ARDUINO
#include <Arduino.h>
void task_hardware()
{
    STS_START();
    static timer_t timer = 0;

	pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH); // sets the digital pin RELAY_PIN off
    while (1)
    {
        STS_WAIT_UNTIL(flag_press_button);
        PRINTF_INFO("PRESSING_BUTTON...");
        digitalWrite(RELAY_PIN, LOW); // sets the digital pin RELAY_PIN on
        DELAY(timer, 5000);
        digitalWrite(RELAY_PIN, HIGH); // sets the digital pin RELAY_PIN off
        DELAY(timer, 1000);
        PRINTF_INFO("PRESS_BUTTON_IS_DONE!");
        flag_press_button = 0;
    }
    STS_ENDED();
}

#endif // ARDUINO

#ifdef WINDOWS
void task_hardware()
{
    return;
}
#endif // WINDOWS
