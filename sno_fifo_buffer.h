/**
 * 雪星管道库 v2018.08.10
 * Copyright (C) 2018 Snowstar
 */
#include "common.h"
#include <string.h>
#pragma once
typedef unsigned int uint;

/**
 * 基础函数定义
 */

extern uint fifo_buffer_read(char *fifo_buffer, uint *wi, uint *ri, uint len,
                      char *buffer, uint buffer_len);
extern uint fifo_buffer_write(char *fifo_buffer, uint *wi, uint *ri, uint len,
                       char *buffer, uint buffer_len);
extern void fifo_buffer_clrbuf(char *fifo_buffer, uint *wi, uint *ri, uint len);
extern char *fifo_buffer_find(char *fifo_buffer, uint *wi, uint *ri, uint len,
                       char *str);
extern void show_repr(char *data, uint len);

/**
 * 注意不要在声明的地方加 inline
 */
#define THIS_IS_A_FIFO_BUFFER_DECLARE(BUFFER)                  \
    extern char BUFFER[];                                      \
    extern uint BUFFER##_wi;                                   \
    extern uint BUFFER##_ri;                                   \
    extern uint BUFFER##_read(char *buffer, uint buffer_len);  \
    extern uint BUFFER##_write(char *buffer, uint buffer_len); \
    extern void BUFFER##_clrbuf(void);                         \
    extern char *(BUFFER##_find)(char *str);                   \
    extern void BUFFER##_show(void);                           \
    extern char *BUFFER##_read_pipe(void);                     \
    extern uint BUFFER##_read_pipe_len(void);                  \
    extern char *BUFFER##_write_pipe(void);                    \
    extern uint BUFFER##_write_pipe_len(void);                 \
    extern void BUFFER##_trimleft(uint len);

/**
 *  缓冲区定义
 *  以后升级环形缓冲区才用得到ri
 */
#define THIS_IS_A_FIFO_BUFFER(BUFFER)                                     \
    uint BUFFER##_wi = 0;                                                 \
    uint BUFFER##_ri = 0;                                                 \
    /** BASE FUNCTION */                                                  \
    uint BUFFER##_read(char *buffer, uint buffer_len)                     \
    {                                                                     \
        return fifo_buffer_read(BUFFER, &(BUFFER##_wi), &(BUFFER##_ri),   \
                                ARR_LEN(BUFFER), buffer, buffer_len);     \
    }                                                                     \
    uint BUFFER##_write(char *buffer, uint buffer_len)                    \
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
    inline char *(BUFFER##_find)(char *str)                               \
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
    inline char *BUFFER##_read_pipe(void)                                 \
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
    inline char *BUFFER##_write_pipe(void)                                \
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
