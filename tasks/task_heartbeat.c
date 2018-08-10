#include "task_heartbeat.h"
#include "../lib/timer.h"
#include "../lib/common.h"

void task_heartbeat()
{
	STS_START();
	static long timer = 0;
	while (1)
	{
		printf("[DEBUG   ] ÐÄÌøÏß³Ì\n");
		DELAY(timer, 5000);
	}
	STS_ENDED();
}
