/**
 * This is an iot project with SIM800C module, arduino...
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

#ifndef ARDUINO
#define WINDOWS
#endif

#ifdef WINDOWS
#include "common.h"
#include "printf.h"
#include "serial.h"
#include "task_heartbeat.h"
#include "task_serial.h"
#include "task_sim800c.h"
#include "task_work.h"
#include <windows.h>
/**
 * main
 */
void setup()
{
    printf("[INFO    ]"
           "init serial...\n");
    init_serial();
    printf("[INFO    ]"
           "init done\n");
}

void loop()
{
    // 理论上任务顺序不分先后, TODO: 需要验证
    printf("[DEBUG   ]"
           "task_serial\n");
    task_serial();
    printf("[DEBUG   ]"
           "task_communicate\n");
    task_sim800c();
    printf("[DEBUG   ]"
           "task_work\n");
    task_work();
    // printf("[DEBUG   ]" "task_hardware\n");
    // task_hardware();
    // printf("[DEBUG   ]" "task_handle_msg\n");
    // task_handle_msg();
    printf("[DEBUG   ]"
           "task_heartbeat\n");
    task_heartbeat();
}

/**
 * 入口函数
 */
void main()
{
    // 使用 UTF8 编码的终端
    system("chcp 65001");
    setup();
    while (1)
    {
        loop();
        Sleep(200); // 让CPU休息 1ms 避免CPU占用率过高
    }
}
#endif

/**
 * hack...
 */
#ifdef WINDOWS
#include "serial_windows.c"
#include "sno_fifo_buffer.c"
#include "task_heartbeat.c"
#include "task_serial.c"
#include "task_sim800c.c"
#include "task_work.c"
#endif
