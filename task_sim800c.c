//
#include "task_sim800c.h"
#include "sno_task_scheduler.h"
#include "task_serial.h"
#include <stdlib.h>
#include <string.h>

#pragma once
char buf_socket_send[128] = {0};
char buf_socket_recv[128] = {0};
THIS_IS_A_FIFO_BUFFER(buf_socket_send);
THIS_IS_A_FIFO_BUFFER(buf_socket_recv);

/**
 * 接凤
 */
int flag_socket_close = 0;
int flag_socket_ready = 0;
int flag_server_configured = 0;
char *protocol, *host, *port;
inline void config_server(char *_protocol, char *_host, char *_port)
{
    protocol = _protocol;
    host = _host;
    port = _port;
    flag_server_configured = 1;
}

/**
 * 模通指
 */
long long timer_query = 0;
int flag_timeout = 0;
char *buf_serial_recv_last_find = NULL;
#define QUERY_AT(timeout, cmd, find_str)                    \
    buf_serial_recv_clrbuf();                               \
    serial_write((char *)(cmd "\r"), sizeof(cmd "\r") - 1); \
    printf("[DEBUG   ]"                                     \
           "SEND_CMD: '%s'\n",                              \
           cmd);                                            \
    SETTIMER(timer_query);                                  \
    STS_WAIT_UNTIL(                                         \
        (flag_timeout = TIMEOUT(timer_query, timeout)) ||   \
        (buf_serial_recv_last_find = buf_serial_recv_find((char*)(char *)find_str)));

#define QUERY_DATA(timeout, data, len, find_str)          \
    buf_serial_recv_clrbuf();                             \
    serial_write((char *)(data), len);                    \
    printf("[DEBUG   ]"                                   \
           "SENDDATA: '");                                \
    show_repr((char *)(data), len);                       \
    printf("'\n");                                        \
    SETTIMER(timer_query);                                \
    STS_WAIT_UNTIL(                                       \
        (flag_timeout = TIMEOUT(timer_query, timeout)) || \
        (buf_serial_recv_last_find = buf_serial_recv_find((char*)(char *)find_str)));

/**
 * 循
 */
void task_sim800c()
{
    STS_START();

check_at:
    printf("[INFO    ]"
           "AT 测试...\n");
    QUERY_AT(1000, "AT", "OK");
    if (flag_timeout)
        goto check_at; //AT通状

close_echo:
    printf("[INFO    ]"
           "节关闭伙...\n");
    QUERY_AT(1000, "ATE0", "OK");
    if (flag_timeout)
        goto check_at; //截憋狡讹

ready:
    if (flag_server_configured)
    {
        goto register_network;
    }
    STS_DOEVENTS();
    goto ready;

register_network:
    printf("[INFO    ]"
           "注册到网络...\n");
    QUERY_AT(1000, "AT+CREG?", "+CREG: ");
    if (flag_timeout)
        goto check_at; //截憋狡讹
    // TODO: 接帮全
    do
    {
        char *lastFind = buf_serial_recv_find((char*)"+CREG: ");
        if (strlen(lastFind) > 9 && (lastFind[9] == '1' || lastFind[9] == '5'))
        {
            printf("[INFO    ]"
                   "接碉界，%s, 模式<%3.3s>\n",
                   lastFind[9] == '1' ? "" : "", lastFind + 7);
            goto check_connect;
        }
        else
        {
            goto register_network;
        }
    } while (0);
socket_connect_to_server:

    flag_socket_ready = 0;

    printf("[INFO    ]"
           "正在关闭连接...\n");
    QUERY_AT(2000, "AT+CIPSHUT", "SHUT OK");
    if (flag_timeout)
        goto register_network;

    printf("[INFO    ]"
           "正在GPRS狡讹台轿狟,支街帮捷...\n");
    QUERY_AT(2000, "AT+CGCLASS=\"B\"", "OK");
    if (flag_timeout)
        goto register_network;

    printf("[INFO    ]"
           "正在PDP,协议...\n");
    QUERY_AT(2000, "AT+CGDCONT=1,\"IP\",\"CMNET\"", "OK");
    if (flag_timeout)
        goto register_network;

    printf("[INFO    ]"
           "正在节革GPRS业...\n");
    QUERY_AT(2000, "AT+CGATT=1", "OK");
    if (flag_timeout)
        goto register_network;

    printf("[INFO    ]"
           "正在为GPRS模式...\n");
    QUERY_AT(2000, "AT+CIPCSGP=1,\"CMNET\"", "OK");
    if (flag_timeout)
        goto register_network;

    printf("[INFO    ]"
           "正在矫示IP头(叫讹源,节碉路效)...\n");
    QUERY_AT(2000, "AT+CIPHEAD=1", "OK");
    if (flag_timeout)
        goto register_network;

    printf("[INFO    ]"
           "正在连接服务器 {'协议': '%s', '主机': '%s', '端口': '%s'}\n",
           protocol, host, port);
    QUERY_DATA(0, "AT+CIPSTART=", strlen("AT+CIPSTART="), "");
    QUERY_DATA(0, protocol, strlen(protocol), "");
    QUERY_DATA(0, ",", strlen(","), "");
    QUERY_DATA(0, host, strlen(host), "");
    QUERY_DATA(0, ",", strlen(","), "");
    QUERY_DATA(0, port, strlen(port), "");
    QUERY_AT(3000, "\r", "OK");
    // 3000ms 角等达模应时
    static int timer_connecting = 0;
    SETTIMER(timer_connecting);
    if (flag_timeout)
        goto register_network;

check_connect:
    if (flag_socket_close)
    {
        printf("[INFO    ]"
               "节断匡...\n");
        QUERY_AT(1000, "AT+CIPCLOSE", "OK");
        flag_socket_close = 0;
        flag_socket_ready = 0;
        goto check_connect;
    }

    printf("[INFO    ]"
           "节查看脚猴...\n");
    QUERY_AT(1000, "AT+CSQ", "+CSQ: ");
    if (flag_timeout)
        goto check_at;
    int xhzl1, xhzl2;

    sscanf(buf_serial_recv_last_find + 6, "%d,%d", &xhzl1, &xhzl2);
    printf("[INFO    ]"
           "脚猴<%d, %d>\n",
           xhzl1, xhzl2);

    printf("[INFO    ]"
           "确...\n");
    QUERY_AT(1000, "AT+CIPSTATUS", "OK");
    if (flag_timeout)
        goto check_at;
    if (buf_serial_recv_find((char*)"CONNECT OK"))
    {
        printf("[INFO    ]"
               "确接碉\n");
        flag_socket_ready = 1;
        goto communication;
    }
    if (buf_serial_recv_find((char*)"CONNECTING"))
    {
        // 8诫还没较撅
        if (TIMEOUT(timer_connecting, 8000))
        {
            printf("[WRANING ]"
                   "接凤时\n");
            goto socket_connect_to_server;
        }
        printf("[INFO    ]"
               "\n");
        goto check_connect;
    }
    if (buf_serial_recv_find((char*)"ERROR"))
    {
        printf("[ERROR   ]"
               "接达\n");
        goto socket_connect_to_server;
    }
    if (buf_serial_recv_find((char*)"TCP CLOSED"))
    {
        printf("[ERROR   ]"
               "窖断匡\n");
        flag_socket_ready = 0;
        goto socket_connect_to_server;
    }
    if (buf_serial_recv_find((char*)"IP INITIAL"))
    {
        printf("[INFO    ]"
               "未取IP\n");
        goto socket_connect_to_server;
    }
    if (buf_serial_recv_find((char*)"IP CONFIG"))
    {
        printf("[INFO    ]"
               "节伙取IP\n");
        goto check_connect;
    }

    printf("[INFO    ]"
           "未确状态接凤\n");
    goto socket_connect_to_server;

communication:
    while (1)
    {
    watch_status:
        if (buf_serial_recv_find((char*)"CLOSED"))
        {
            printf("[DEBUG   ]"
                   "接憋较匡...\n");
            flag_socket_ready = 0;
            // 皆讹
            goto socket_connect_to_server;
        }
    socket_recv:
        buf_serial_recv_last_find = buf_serial_recv_find((char*)"+IPD,");
        if (buf_serial_recv_last_find)
        {
            PRINT_LINE();
            // 秸碉捷筹
            static unsigned int data_len;
            sscanf(buf_serial_recv_last_find + 5, "%d", &data_len);
            // 始位
            // TODO: 全椋猴没揭碉冒
            char *posColom;
            posColom = strstr(buf_serial_recv_last_find, ":");
            buf_serial_recv_last_find = posColom + 1;
            // 接伙狡筹之前街凤
            buf_serial_recv_trimleft(buf_serial_recv_last_find - buf_serial_recv);
            buf_serial_recv_show();
            PRINT_LINE();
            // 始
            static unsigned int remain_data_len;
            remain_data_len = data_len;
            //  buf_serial_recv 转狡讹应饺碉捷碉 buf_socket_recv
            while (remain_data_len)
            {
                {
                    // 较
                    printf("[DEBUG   ]"
                           "接凤 (%d/%d bytes)...\n",
                           data_len - remain_data_len, data_len);
                    // 细侥碉息
                    if (0)
                    {
                        printf("[DEBUG   ]"
                               "buf_serial_recv_read_pipe_len: %d\n",
                               buf_serial_recv_read_pipe_len());
                        printf("[DEBUG   ]"
                               "buf_socket_recv_write_pipe_len: %d\n",
                               buf_socket_recv_write_pipe_len());
                    }

                    // 阶频幕纸
                    unsigned int move_length;
                    move_length = MIN(MIN(buf_serial_recv_read_pipe_len(),
                                          buf_socket_recv_write_pipe_len()),
                                      remain_data_len);
                    if (move_length)
                    {
                        // 转狡讹应街
                        buf_socket_recv_write(buf_serial_recv_read_pipe(), move_length);
                        //  sim800c 狡筹窖撅转狡碉
                        buf_serial_recv_trimleft(move_length);
                        // 剩要转狡碉街
                        remain_data_len -= move_length;
                        //
                        buf_serial_recv_show();
                        buf_socket_recv_show();
                    }
                }
                // 止
                STS_DOEVENTS();
            }
            printf("[DEBUG   ]"
                   "接凤 ()...\n",
                   data_len - remain_data_len, data_len);
        }
    socket_send:
        if (buf_socket_send_wi)
        {
            printf("[DEBUG   ]"
                   "节革...\n");
            QUERY_AT(2000, "AT+CIPSEND", ">");
            if (flag_timeout)
                goto check_connect;
            // 注偷荽 \x1A 侥伙叫断凤
            QUERY_DATA(1000, buf_socket_send, buf_socket_send_wi,
                       "");                      // buf_serial_recv_find((char*)"") 直接凤
            QUERY_AT(8000, "\x1A\0", "SEND OK"); // 使 \x1A 捷凤
            if (flag_timeout)
                goto check_connect;
            buf_socket_send_wi = 0;
            printf("[DEBUG   ]"
                   "捷凤酵成癸\n");
        }
        STS_DOEVENTS();
    }
    STS_ENDED();
}
