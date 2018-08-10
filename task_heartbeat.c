#include "task_heartbeat.h"
#include "timer.h"
#include "common.h"
#pragma once
void task_heartbeat()
{
	STS_START();
	static long timer = 0;
	while (1)
	{
		printf("[DEBUG   ] �����߳�\n");
		DELAY(timer, 5000);
	}
	STS_ENDED();
}
