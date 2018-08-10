/**
 * 雪星任务规划库（精简）配套的计时器 v2018.08.10
 * Copyright (C) 2018 雪星实验室
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
