/**
 * This is an iot project with SIM800C module, arduino...
 * Copyright (C) 2018 Snowstar
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

#define _WINDOWS_
#include "lib/common.h"
#include "lib/serial.h"
#include "tasks/task_heartbeat.h"
#include "tasks/task_serial.h"
#include "tasks/task_sim800c.h"
#include "tasks/task_work.h"

void setup()
{
    printf("[INFO    ]" "init serial...\n");
    init_serial();
    printf("[INFO    ]" "init done\n");
}

void loop()
{
    // 理论上任务顺序不分先后, TODO: 需要验证
    printf("[DEBUG   ]" "task_serial\n");
    task_serial();
    printf("[DEBUG   ]" "task_communicate\n");
    task_sim800c();
    printf("[DEBUG   ]" "task_work\n");
    task_work();
    // printf("[DEBUG   ]" "task_hardware\n");
    // task_hardware();
    // printf("[DEBUG   ]" "task_handle_msg\n");
    // task_handle_msg();
    printf("[DEBUG   ]" "task_heartbeat\n");
    task_heartbeat();
}
// 入口函数
#ifdef _WINDOWS_
#include <windows.h>
void main()
{
    setup();
    while (1)
    {
        loop();
        Sleep(200); // 让CPU休息 1ms 避免CPU占用率过高
    }
}
#endif

// hack...
#include "lib/serial_windows.c"
#include "lib/sno_fifo_buffer.c"
#include "tasks/task_heartbeat.c"
#include "tasks/task_serial.c"
#include "tasks/task_sim800c.c"
#include "tasks/task_work.c"
