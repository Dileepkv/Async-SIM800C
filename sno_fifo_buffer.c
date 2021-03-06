/**
 * 雪星管道库 v2018.08.10
 * Copyright (C) 2018 雪星实验室
 */
#include "sno_fifo_buffer.h"
#include "common.h"
#include "printf.h"
#include <string.h>

/**
 * 基本功能
 */

inline uint fifo_buffer_read(char *fifo_buffer, uint *wi, uint *ri, uint len,
                             char *buffer, uint buffer_len)
{
    /* TODO: 升级环形缓冲区 */
    uint used_len = *wi;
    uint read_len = MIN(buffer_len, used_len);
    if (read_len)
    {
        /* 读出数据 */
        memcpy(buffer, fifo_buffer + *wi, read_len);
        /* 维护缓冲区（左移） */
        memmove(fifo_buffer, fifo_buffer + read_len, used_len - read_len);
        /* 写指针位移 */
        *wi -= read_len;
        /* 标记字符串尾部置0，防止字符串函数出界 */
        fifo_buffer[*wi] = '\0';
        /* 标记数据缓冲区已更新 */
        /* flag_updated = 1; */
        /* 显示缓冲区数据 */
        /* buf_serial_recv_show(); */
    }
    return read_len;
}

inline uint fifo_buffer_write(char *fifo_buffer, uint *wi, uint *ri, uint len,
                              char *buffer, uint buffer_len)
{
    /* TODO: 升级环形缓冲区 */
    /* 保险起见，最后一位留给 \0 ，防止字符串函数出界 */
    uint fifo_buffer_len = len - 1;
    uint used_len = *wi;
    uint empty_len = fifo_buffer_len - used_len;
    uint write_len = MIN(buffer_len, empty_len);
    if (write_len)
    {
        /* 往缓冲区写数据 */
        memcpy(fifo_buffer + *wi, buffer, write_len);
        /* 写指针位移 */
        *wi += write_len;
        /* 标记字符串尾部置0，防止字符串函数出界 */
        fifo_buffer[*wi] = '\0';
        /* 标记数据缓冲区已更新 */
        /* flag_updated = 1; */
        /* 显示缓冲区数据 */
        /* buf_serial_recv_show(); */
    }
    return write_len;
}

/**
 * 其它功能
 */

inline void fifo_buffer_clrbuf(char *fifo_buffer, uint *wi, uint *ri,
                               uint len)
{
    /* 写指针归零 */
    *wi = 0;
    /* 标记字符串尾部置0，防止字符串函数出界 */
    fifo_buffer[*wi] = '\0';
}

inline char *fifo_buffer_find(char *fifo_buffer, uint *wi, uint *ri, uint len,
                              char *str)
{
    /* 标记字符串尾部置0，防止字符串函数出界 */
    fifo_buffer[*wi] = '\0';
    return (char *)strstr(fifo_buffer, str);
}

inline char *fifo_buffer_trimleft(char *fifo_buffer, uint *wi, uint *ri,
                                  uint len, uint trim_len)
{
    uint buf_len = len - 1;
    uint used_len = *wi;
    uint rightlen = buf_len - MIN(trim_len, used_len);
    memmove(fifo_buffer, fifo_buffer + trim_len, rightlen);
    *wi -= trim_len;
    fifo_buffer[*wi] = '\0';
}

/**
 * 调试用的函数
 */
void show_repr(char *data, uint len)
{
    uint i = 0;
    while (i < len)
    {
        char ch = data[i];
        if (ch == '\r')
            printf("\\r");
        else if (ch == '\n')
            printf("\\n");
        else if (ch == '\0')
            printf("\\0");
        else if (ch == '\x1A')
            printf("\\x1A");
        else
            printf("%c", ch);
        i++;
    }
}
