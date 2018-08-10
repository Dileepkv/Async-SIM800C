/**
 * 可用的输出调试等级：
 * ALL  : 是最低等级的, 用于打开所有日志记录.
 * DEBUG: 指出细粒度信息事件对调试应用程序是非常有帮助的, 就是输出 debug 的信息.
 * INFO : 表明消息在粗粒度级别上突出强调应用程序的运行过程, 就是输出提示信息.
 * WARN : 表明会出现潜在错误的情形, 就是显示警告信息.
 * ERROR: 指出虽然发生错误事件, 但仍然不影响系统的继续运行. 就是显示错误信息.
 * FATAL: 指出每个严重的错误事件将会导致应用程序的退出.
 * OFF  : 是最高等级的, 用于关闭所有日志记录.
 *
 * ref: https://blog.csdn.net/qq_27088383/article/details/50714973
 */
#pragma once

#ifdef WINDOWS
#define LOGGING_LEVEL_DEBUG
#endif
#ifdef ARDUINO
#define LOGGING_LEVEL_INFO
#endif

/**
 * 在这里选择一个调试等级来取消注释
 */
// #define LOGGING_LEVEL_ALL
// #define LOGGING_LEVEL_DEBUG
// #define LOGGING_LEVEL_INFO
// #define LOGGING_LEVEL_WARN
// #define LOGGING_LEVEL_ERROR
// #define LOGGING_LEVEL_FATAL
// #define LOGGING_LEVEL_OFF

#include "printf.h"

#ifdef LOGGING_LEVEL_ALL
#define LOGGING_LEVEL_DEBUG
#else
// NOTHING
#endif

#ifdef LOGGING_LEVEL_DEBUG
#define LOGGING_LEVEL_INFO
#define PRINTF_DEBUG(fmt, ...)      \
    do                              \
    {                               \
        printf("[DEBUG]");          \
        printf(fmt, ##__VA_ARGS__); \
    } while (0)
#else
#define PRINTF_DEBUG(fmt, ...) ({})
#endif

#ifdef LOGGING_LEVEL_INFO
#define LOGGING_LEVEL_WARN
#define PRINTF_INFO(fmt, ...)       \
    do                              \
    {                               \
        printf("[INFO ]");          \
        printf(fmt, ##__VA_ARGS__); \
    } while (0)
#else
#define PRINTF_INFO(fmt, ...) ({})
#endif

#ifdef LOGGING_LEVEL_WARN
#define LOGGING_LEVEL_ERROR
#define PRINTF_WARN(fmt, ...)       \
    do                              \
    {                               \
        printf("[WARN ]");          \
        printf(fmt, ##__VA_ARGS__); \
    } while (0)
#else
#define PRINTF_WARN(fmt, ...) ({})
#endif

#ifdef LOGGING_LEVEL_ERROR
#define LOGGING_LEVEL_FATAL
#define PRINTF_ERROR(fmt, ...)      \
    do                              \
    {                               \
        printf("[ERROR]");          \
        printf(fmt, ##__VA_ARGS__); \
    } while (0)
#else
#define PRINTF_ERROR(fmt, ...) ({})
#endif

#ifdef LOGGING_LEVEL_FATAL
#define LOGGING_LEVEL_OFF
#define PRINTF_FATAL(fmt, ...)      \
    do                              \
    {                               \
        printf("[FATAL]");          \
        printf(fmt, ##__VA_ARGS__); \
    } while (0)
#else
#define PRINTF_FATAL(fmt, ...) ({})
#endif

#ifdef LOGGING_LEVEL_OFF
// NOTHING
#else
// NOTHING
#endif
