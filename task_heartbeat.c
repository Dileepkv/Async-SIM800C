/**
 * 心跳线程 v2018.08.10
 * Copyright (C) 2018 雪星实验室
 */

#include "task_heartbeat.h"
#include "timer.h"
#include "common.h"
#include "printf.h"

void task_heartbeat()
{
	STS_START();
	static long timer = 0;
	while (1)
	{
		printf("[DEBUG   ] 心跳线程\n");
		DELAY(timer, 5000);
	}
	STS_ENDED();
}
