// 中文
#include "task_sim800c.h"
#include "../lib/sno_task_scheduler.h"
#include "task_serial.h"
#include <stdlib.h>
#include <string.h>

char buf_socket_send[128] = {0};
char buf_socket_recv[128] = {0};
THIS_IS_A_FIFO_BUFFER(buf_socket_send);
THIS_IS_A_FIFO_BUFFER(buf_socket_recv);

/**
 * 配置连接服务器
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
 * 模块通信指令
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
        (buf_serial_recv_last_find = buf_serial_recv_find((char *)find_str)));

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
        (buf_serial_recv_last_find = buf_serial_recv_find((char *)find_str)));

/**
 * 循环任务
 */
void task_sim800c()
{
    STS_START();

check_at:
    printf("[INFO    ]"
           "正在AT(艾特)模块...\n");
    QUERY_AT(1000, "AT", "OK");
    if (flag_timeout)
        goto check_at; //测试AT通信状况

close_echo:
    printf("[INFO    ]"
           "正在关闭回显...\n");
    QUERY_AT(1000, "ATE0", "OK");
    if (flag_timeout)
        goto check_at; //关闭移动场景

ready:
    if (flag_server_configured)
    {
        goto register_network;
    }
    STS_DOEVENTS();
    goto ready;

register_network:
    printf("[INFO    ]"
           "正在注册到网络...\n");
    QUERY_AT(1000, "AT+CREG?", "+CREG: ");
    if (flag_timeout)
        goto check_at; //关闭移动场景
    // TODO: 低速连接安全检查
    do
    {
        char *lastFind = Find("+CREG: ");
        if (strlen(lastFind) > 9 && (lastFind[9] == '1' || lastFind[9] == '5'))
        {
            printf("[INFO    ]"
                   "已连接到网络，%s, 模式<%3.3s>\n",
                   lastFind[9] == '1' ? "本地流量" : "漫游流量", lastFind + 7);
            goto check_connect;
        }
        else
        {
            goto register_network;
        }
    } while (0);
connect_to_server:

    flag_socket_ready = 0;

    printf("[INFO    ]"
           "正在关闭移动场景...\n");
    QUERY_AT(2000, "AT+CIPSHUT", "SHUT OK");
    if (flag_timeout)
        goto register_network;

    printf("[INFO    ]"
           "正在设置GPRS移动台类别为B,支持包交换和数据交换...\n");
    QUERY_AT(2000, "AT+CGCLASS=\"B\"", "OK");
    if (flag_timeout)
        goto register_network;

    printf("[INFO    ]"
           "正在设置PDP上下文,互联网接协议,接入点等信息...\n");
    QUERY_AT(2000, "AT+CGDCONT=1,\"IP\",\"CMNET\"", "OK");
    if (flag_timeout)
        goto register_network;

    printf("[INFO    ]"
           "正在附着GPRS业务...\n");
    QUERY_AT(2000, "AT+CGATT=1", "OK");
    if (flag_timeout)
        goto register_network;

    printf("[INFO    ]"
           "正在设置为GPRS连接模式...\n");
    QUERY_AT(2000, "AT+CIPCSGP=1,\"CMNET\"", "OK");
    if (flag_timeout)
        goto register_network;

    printf("[INFO    ]"
           "正在设置接收数据显示IP头(方便判断数据来源,仅在单路连接有效)...\n");
    QUERY_AT(2000, "AT+CIPHEAD=1", "OK");
    if (flag_timeout)
        goto register_network;

    printf("[INFO    ]"
           "正在连接到服务器 {'协议': '%s', '主机': '%s', '端口': '%s'}\n",
           protocol, host, port);
    QUERY_DATA(0, "AT+CIPSTART=", strlen("AT+CIPSTART="), "");
    QUERY_DATA(0, protocol, strlen(protocol), "");
    QUERY_DATA(0, ",", strlen(","), "");
    QUERY_DATA(0, host, strlen(host), "");
    QUERY_DATA(0, ",", strlen(","), "");
    QUERY_DATA(0, port, strlen(port), "");
    QUERY_AT(3000, "\r", "OK");
    // 3000ms 是等待模块响应的时间
    static int timer_connecting = 0;
    SETTIMER(timer_connecting);
    if (flag_timeout)
        goto register_network;

check_connect:
    if (flag_socket_close)
    {
        printf("[INFO    ]"
               "正在断开连接...\n");
        QUERY_AT(1000, "AT+CIPCLOSE", "OK");
        flag_socket_close = 0;
        flag_socket_ready = 0;
        goto check_connect;
    }

    printf("[INFO    ]"
           "正在查看信号质量...\n");
    QUERY_AT(1000, "AT+CSQ", "+CSQ: ");
    if (flag_timeout)
        goto check_at;
    int xhzl1, xhzl2;

    sscanf(buf_serial_recv_last_find + 6, "%d,%d", &xhzl1, &xhzl2);
    printf("[INFO    ]"
           "信号质量<%d, %d>\n",
           xhzl1, xhzl2);

    printf("[INFO    ]"
           "正在确认连接质量...\n");
    QUERY_AT(1000, "AT+CIPSTATUS", "OK");
    if (flag_timeout)
        goto check_at;
    if (Find("CONNECT OK"))
    {
        printf("[INFO    ]"
               "确认已连接到服务器\n");
        flag_socket_ready = 1;
        goto communication;
    }
    if (Find("CONNECTING"))
    {
        // 8秒还没连上就重连
        if (TIMEOUT(timer_connecting, 8000))
        {
            printf("[WRANING ]"
                   "连接服务器超时，尝试重新连接\n");
            goto connect_to_server;
        }
        printf("[INFO    ]"
               "正在连接\n");
        goto check_connect;
    }
    if (Find("ERROR"))
    {
        printf("[ERROR   ]"
               "连接错误\n");
        goto connect_to_server;
    }
    if (Find("TCP CLOSED"))
    {
        printf("[ERROR   ]"
               "连接已断开\n");
        flag_socket_ready = 0;
        goto connect_to_server;
    }
    if (Find("IP INITIAL"))
    {
        printf("[INFO    ]"
               "尚未获取网络IP\n");
        goto connect_to_server;
    }
    if (Find("IP CONFIG"))
    {
        printf("[INFO    ]"
               "正在获取网络IP\n");
        goto check_connect;
    }

    printf("[INFO    ]"
           "未能确认连接状态，尝试重新连接服务器\n");
    goto connect_to_server;

communication:
    while (1)
    {
    watch_status:
        if (buf_serial_recv_find("CLOSED"))
        {
            printf("[DEBUG   ]"
                    "连接被服务器断开...\n");
            flag_socket_ready = 0;
            // 自动重连
            goto connect_to_server;
        }
    socket_recv:
        buf_serial_recv_last_find = buf_serial_recv_find("+IPD,");
        if (buf_serial_recv_last_find)
        {
            PRINT_LINE();
            // 收到的数据长度
            static unsigned int data_len;
            sscanf(buf_serial_recv_last_find + 5, "%d", &data_len);
            // 数据起始位置
            // TODO: 安全检查：如果没有找到冒号
            char *posColom = strstr(buf_serial_recv_last_find, ":");
            buf_serial_recv_last_find = posColom + 1;
            // 从缓冲区里移除数据之前的字符
            buf_serial_recv_trimleft(buf_serial_recv_last_find - buf_serial_recv);
            buf_serial_recv_show();
            PRINT_LINE();
            // 开始接收数据
            static unsigned int remain_data_len;
            remain_data_len = data_len;
            // 从 buf_serial_recv 转移对应长度的数据到 buf_socket_recv
            while (remain_data_len)
            {
                // 输出调试信息
                printf("[DEBUG   ]"
                       "从服务器接收数据 (%d/%d bytes)...\n",
                       data_len - remain_data_len, data_len);
                // 更详细的调试信息
                if (0)
                {
                    printf("[DEBUG   ]"
                           "buf_serial_recv_read_pipe_len: %d\n",
                           buf_serial_recv_read_pipe_len());
                    printf("[DEBUG   ]"
                           "buf_socket_recv_write_pipe_len: %d\n",
                           buf_socket_recv_write_pipe_len());
                }
                // 计算可以转移的缓冲区字节数
                unsigned int move_length = MIN(MIN(buf_serial_recv_read_pipe_len(),
                                                   buf_socket_recv_write_pipe_len()),
                                               remain_data_len);
                if (move_length)
                {
                    // 转移对应字节数
                    buf_socket_recv_write(buf_serial_recv_read_pipe(), move_length);
                    // 从 sim800c 缓冲区里移除已经转移的数据
                    buf_serial_recv_trimleft(move_length);
                    // 计算剩余需要转移的字节数
                    remain_data_len -= move_length;
                    // 调试
                    buf_serial_recv_show();
                    buf_socket_recv_show();
                }
                // 防止阻塞
                STS_DOEVENTS();
            }
            printf("[DEBUG   ]"
                   "从服务器接收数据 (完成)...\n",
                   data_len - remain_data_len, data_len);
        }
    socket_send:
        if (buf_socket_send_wi)
        {
            printf("[DEBUG   ]"
                   "正在给服务器发送数据...\n");
            QUERY_AT(2000, "AT+CIPSEND", ">");
            if (flag_timeout)
                goto check_connect;
            // 注意这里如果发送的内容带有 \x1A 的话会中断发送
            QUERY_DATA(1000, buf_socket_send, buf_socket_send_wi,
                       "");                      // Find("") 直接返回
            QUERY_AT(8000, "\x1A\0", "SEND OK"); // 使用 \x1A 结束数据发送
            if (flag_timeout)
                goto check_connect;
            buf_socket_send_wi = 0;
            printf("[DEBUG   ]"
                   "数据发送成功！\n");
        }
        STS_DOEVENTS();
    }
    STS_ENDED();
}
