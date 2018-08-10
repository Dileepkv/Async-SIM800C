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

// #ifdef ARDUINO
// #include "ardprintf.cpp"
// #endif

#include "printf.h"
#include "serial.h"
extern "C"
{

/**
 * 我不知道怎么把这堆文件放到 lib、task 这样的文件夹里，当我这样做的时候 Arduino
 * 的编译就会出问题。希望有人能告诉我怎么办。
 */

#include "common.h"
#include "sno_printf_logging.h"
#include "task_heartbeat.h"
#include "task_serial.h"
#include "task_sim800c.h"
#include "task_work.h"
}

void setup()
{
    PRINTF_INFO("init serial...");
    init_serial();
    PRINTF_INFO("init done");
}

void loop()
{
    // 理论上任务顺序不分先后, TODO: 需要验证
    PRINTF_DEBUG("task_serial");
    task_serial();
    PRINTF_DEBUG("task_communicate");
    task_sim800c();
    PRINTF_DEBUG("task_work");
    task_work();
    // PRINTF_DEBUG( "task_hardware");
    // task_hardware();
    // PRINTF_DEBUG( "task_handle_msg");
    // task_handle_msg();
    PRINTF_DEBUG("task_heartbeat");
    task_heartbeat();
}
