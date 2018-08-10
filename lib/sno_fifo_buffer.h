/**
 * 雪星管道库 v2018.08.10
 * Copyright (C) 2018 Snowstar
 */
#include "common.h"
#pragma once
typedef unsigned int uint;

/**
 * 注意不要在声明的地方加 inline
 */
#define THIS_IS_A_FIFO_BUFFER_DECLARE(BUFFER)                    \
    extern char BUFFER[];                                        \
    extern uint BUFFER##_wi;                                     \
    extern uint BUFFER##_ri;                                     \
    uint BUFFER##_read(unsigned char *buffer, uint buffer_len);  \
    uint BUFFER##_write(unsigned char *buffer, uint buffer_len); \
    void BUFFER##_clrbuf(void);                                  \
    unsigned char *(BUFFER##_find)(unsigned char *str);          \
    void BUFFER##_show(void);                                    \
    unsigned char *BUFFER##_read_pipe(void);                     \
    uint BUFFER##_read_pipe_len(void);                           \
    unsigned char *BUFFER##_write_pipe(void);                    \
    uint BUFFER##_write_pipe_len(void);                          \
    void BUFFER##_trimleft(uint len);

void show_repr(char *data, uint len);

uint fifo_buffer_read(char *fifo_buffer, uint *wi, uint *ri, uint len,
                      char *buffer, uint buffer_len);
uint fifo_buffer_write(char *fifo_buffer, uint *wi, uint *ri, uint len,
                       char *buffer, uint buffer_len);
void fifo_buffer_clrbuf(char *fifo_buffer, uint *wi, uint *ri, uint len);
char *fifo_buffer_find(char *fifo_buffer, uint *wi, uint *ri, uint len,
                       char *str);
