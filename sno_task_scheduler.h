/**
 * 雪星任务规划库（精简） v2018.08.10
 * Copyright (C) 2018 雪星实验室
 */
#pragma once

typedef unsigned short sts_status_t;

#define __STS_START_LINE__ ((sts_status_t)(0))
#define __STS_ENDED_LINE__ ((sts_status_t)(-1))
#define STS_START()                                             \
    static sts_status_t STS_PAUSED_LINE = __STS_START_LINE__; \
    switch (STS_PAUSED_LINE)                                    \
    {                                                           \
    case __STS_START_LINE__:
#define STS_ENDED()                           \
    case __STS_ENDED_LINE__:                  \
        STS_PAUSED_LINE = __STS_ENDED_LINE__; \
        break;                                \
        }                                     \
        return;
#define STS_DOEVENTS()          \
    STS_PAUSED_LINE = __LINE__; \
    return;                     \
    case __LINE__:
#define STS_WAIT_UNTIL(expr) \
    STS_DOEVENTS();          \
    if (!(expr))             \
        return;
