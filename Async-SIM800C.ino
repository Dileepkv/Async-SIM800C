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

// #ifdef ARDUINO
// #include "ardprintf.cpp"
// #endif


#include "printf.h"
#include "serial.h"
extern "C"
{
#include "common.h"
#include "task_heartbeat.h"
#include "task_serial.h"
#include "task_sim800c.h"
#include "task_work.h"
}

/**
 * hack...
 */
#ifdef ARDUINO
// #include "serial_arduino.c"
// #include "sno_fifo_buffer.c"
// #include "task_heartbeat.c"
// #include "task_serial.c"
// #include "task_sim800c.c"
// #include "task_work.c"
#endif

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
 * 我不知道怎么把这堆文件放到文件夹里，当我这样做的时候 Arduino 的编译就会出问题
 */

// #include "main.c"

// #include "common.h"
// #include "serial.h"
// #include "task_heartbeat.h"
// #include "task_serial.h"
// #include "task_sim800c.h"
// #include "task_work.h"
// #include "printf.h"
// /**
//  * main
//  */
// void setup()
// {
//     printf("[INFO    ]"
//            "init serial...\n");
//     init_serial();
//     printf("[INFO    ]"
//            "init done\n");
// }

// void loop()
// {
//     // 理论上任务顺序不分先后, TODO: 需要验证
//     printf("[DEBUG   ]"
//            "task_serial\n");
//     task_serial();
//     printf("[DEBUG   ]"
//            "task_communicate\n");
//     task_sim800c();
//     printf("[DEBUG   ]"
//            "task_work\n");
//     task_work();
//     // printf("[DEBUG   ]" "task_hardware\n");
//     // task_hardware();
//     // printf("[DEBUG   ]" "task_handle_msg\n");
//     // task_handle_msg();
//     printf("[DEBUG   ]"
//            "task_heartbeat\n");
//     task_heartbeat();
// }
