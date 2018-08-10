#pragma once
// 提供时间函数

#ifdef WINDOWS
#include <windows.h>
#define NOW() ((unsigned long)GetTickCount())
#endif

#ifdef ARDUINO
extern unsigned long millis();
#define NOW() ((unsigned long)millis())
#endif

#define SETTIMER(timer) (timer = NOW())
#define TIMEOUT(timer, timeout) (NOW() - timer > (timeout))

#define DELAY(timer, interval)                    \
    do                                            \
    {                                             \
        SETTIMER(timer);                          \
        STS_WAIT_UNTIL(TIMEOUT(timer, interval)); \
    } while (0)

