/**
 * 雪星管道库 v2018.08.10
 * Copyright (C) 2018 Snowstar
 */
#include "sno_fifo_buffer.h"
#include "common.h"

/**
 *  缓冲区定义
 *  以后升级环形缓冲区才用得到ri
 */
#define THIS_IS_A_FIFO_BUFFER(BUFFER)                                     \
    uint BUFFER##_wi = 0;                                                 \
    uint BUFFER##_ri = 0;                                                 \
    /** BASE FUNCTION */                                                  \
    uint BUFFER##_read(unsigned char *buffer, uint buffer_len)            \
    {                                                                     \
        return fifo_buffer_read(BUFFER, &(BUFFER##_wi), &(BUFFER##_ri),   \
                                ARR_LEN(BUFFER), buffer, buffer_len);     \
    }                                                                     \
    uint BUFFER##_write(unsigned char *buffer, uint buffer_len)           \
    {                                                                     \
        return fifo_buffer_write(BUFFER, &(BUFFER##_wi), &(BUFFER##_ri),  \
                                 ARR_LEN(BUFFER), buffer, buffer_len);    \
    }                                                                     \
    /** EXTEND FUNCTION */                                                \
    inline void BUFFER##_clrbuf(void)                                     \
    {                                                                     \
        return fifo_buffer_clrbuf(BUFFER, &(BUFFER##_wi), &(BUFFER##_ri), \
                                  ARR_LEN(BUFFER));                       \
    }                                                                     \
    inline unsigned char *(BUFFER##_find)(unsigned char *str)             \
    {                                                                     \
        return fifo_buffer_find(BUFFER, &(BUFFER##_wi), &(BUFFER##_ri),   \
                                ARR_LEN(BUFFER), str);                    \
    }                                                                     \
    inline void BUFFER##_show(void)                                       \
    {                                                                     \
        printf("[DEBUG   ]" #BUFFER "='");                                \
        show_repr(BUFFER, BUFFER##_wi);                                   \
        printf("'\n");                                                    \
    }                                                                     \
    inline unsigned char *BUFFER##_read_pipe(void)                        \
    {                                                                     \
        return BUFFER + BUFFER##_ri;                                      \
    }                                                                     \
    inline uint BUFFER##_read_pipe_len(void)                              \
    {                                                                     \
        uint buf_len = ARR_LEN(BUFFER) - 1;                               \
        uint used_len = BUFFER##_wi;                                      \
        uint remain_len = buf_len - used_len;                             \
        return used_len;                                                  \
    }                                                                     \
    inline unsigned char *BUFFER##_write_pipe(void)                       \
    {                                                                     \
        return BUFFER + BUFFER##_wi;                                      \
    }                                                                     \
    inline uint BUFFER##_write_pipe_len(void)                             \
    {                                                                     \
        uint buf_len = ARR_LEN(BUFFER) - 1;                               \
        uint used_len = BUFFER##_wi;                                      \
        uint remain_len = buf_len - used_len;                             \
        return remain_len;                                                \
    }                                                                     \
    inline void BUFFER##_trimleft(uint len)                               \
    {                                                                     \
        uint buf_len = ARR_LEN(BUFFER) - 1;                               \
        uint used_len = BUFFER##_wi;                                      \
        uint remain_len = buf_len - used_len;                             \
        uint rightlen = buf_len - MIN(len, used_len);                     \
        memmove(BUFFER, BUFFER + len, rightlen);                          \
        BUFFER##_wi -= len;                                               \
        BUFFER[BUFFER##_wi] = '\0';                                       \
    }

/**
 * 基本功能
 */

inline uint fifo_buffer_read(char *fifo_buffer, uint *wi, uint *ri, uint len,
                             char *buffer, uint buffer_len)
{
    /* TODO: 升级环形缓冲区 */
    /* 保险起见，最后一位留给 \0 ，防止字符串函数出界 */
    uint fifo_buffer_len = len - 1;
    uint used_len = *wi;
    uint empty_len = fifo_buffer_len - used_len;
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
    uint remain_len = buf_len - used_len;
    uint rightlen = buf_len - MIN(trim_len, used_len);
    memmove(fifo_buffer, fifo_buffer + trim_len, rightlen);
    *wi -= len;
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
