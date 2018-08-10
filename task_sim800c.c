//
#include "task_sim800c.h"
#include "common.h"
#include "printf.h"
#include "serial.h"
#include "sno_printf_logging.h"
#include "sno_task_scheduler.h"
#include "task_serial.h"
#include "timer.h"
#include <stdlib.h>
#include <string.h>


/**
 *  定义 SOCKET 接受缓冲区长度，不要小于需要匹配的指令的长度
 */
#define BUF_SOCKET_RECV_LENGTH 16


/**
 *  定义 SOCKET 发送缓冲区长度，不要小于需要匹配的指令的长度
 */
#define BUF_SOCKET_SEND_LENGTH 16

// 缓冲区定义
char buf_socket_recv[BUF_SOCKET_RECV_LENGTH] = {0};
THIS_IS_A_FIFO_BUFFER(buf_socket_recv);

char buf_socket_send[BUF_SOCKET_SEND_LENGTH] = {0};
THIS_IS_A_FIFO_BUFFER(buf_socket_send);

/**
 * TCP 连接
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
 * 模块指令
 */
long long timer_query = 0;
int flag_timeout = 0;
char *buf_serial_recv_last_find = NULL;
#define QUERY_AT(timeout, cmd, find_str)                             \
    buf_serial_recv_clrbuf();                                        \
    serial_write((char *)(cmd "\r"), sizeof(cmd "\r") - 1);          \
    PRINTF_DEBUG("SEND_CMD: '%s'\n", cmd);                           \
    SETTIMER(timer_query);                                           \
    STS_WAIT_UNTIL((flag_timeout = TIMEOUT(timer_query, timeout)) || \
                   (buf_serial_recv_last_find =                      \
                        buf_serial_recv_find((char *)(char *)find_str)));

#define QUERY_DATA(timeout, data, len, find_str)                     \
    buf_serial_recv_clrbuf();                                        \
    serial_write((char *)(data), len);                               \
    PRINTF_DEBUG("SENDDATA: '");                                     \
    show_repr((char *)(data), len);                                  \
    printf("'\n");                                                   \
    SETTIMER(timer_query);                                           \
    STS_WAIT_UNTIL((flag_timeout = TIMEOUT(timer_query, timeout)) || \
                   (buf_serial_recv_last_find =                      \
                        buf_serial_recv_find((char *)(char *)find_str)));

/**
 * 任务循环
 */
void task_sim800c()
{
    STS_START();

check_at:
    PRINTF_DEBUG("正在 AT 测试...\n");
    QUERY_AT(1000, "AT", "OK");
    if (flag_timeout)
    {
        PRINTF_ERROR("模块失联！\n");
        goto check_at;
    }

close_echo:
    PRINTF_DEBUG("正在关闭回显...\n");
    QUERY_AT(1000, "ATE0", "OK");
    if (flag_timeout)
        goto check_at;

ready:
    if (flag_server_configured)
    {
        PRINTF_DEBUG("服务器配置完成！\n");
        goto register_network;
    }
    PRINTF_WARN("服务器未配置！\n");
    STS_DOEVENTS();
    goto ready;

register_network:
    PRINTF_DEBUG("正在注册到网络...\n");
    QUERY_AT(1000, "AT+CREG?", "+CREG: ");
    if (flag_timeout)
        goto check_at;
    // TODO: 总感觉这里还有什么要优化掉的
    do
    {
        char *lastFind = buf_serial_recv_find((char *)"+CREG: ");
        if (strlen(lastFind) > 9 && (lastFind[9] == '1' || lastFind[9] == '5'))
        {
            if (lastFind[9] == '1')
            {
                PRINTF_INFO("已注册到网络，本地流量, 模式<%3.3s>\n", lastFind + 7);
            }
            if (lastFind[9] == '5')
            {
                PRINTF_INFO("已注册到网络，漫游流量, 模式<%3.3s>\n", lastFind + 7);
            }
            goto check_connect;
        }
        else
        {
            goto register_network;
        }
    } while (0);

socket_connect_to_server:
    flag_socket_ready = 0;

    PRINTF_INFO("正在关闭连接...\n");
    QUERY_AT(2000, "AT+CIPSHUT", "SHUT OK");
    if (flag_timeout)
        goto register_network;

    PRINTF_DEBUG("正在启用 GPRS ...\n");
    QUERY_AT(2000, "AT+CGCLASS=\"B\"", "OK");
    if (flag_timeout)
        goto register_network;

    PRINTF_DEBUG("正在启用 PDP 协议...\n");
    QUERY_AT(2000, "AT+CGDCONT=1,\"IP\",\"CMNET\"", "OK");
    if (flag_timeout)
        goto register_network;

    PRINTF_DEBUG("正在设置 GPRS 模式...\n");
    QUERY_AT(2000, "AT+CGATT=1", "OK");
    if (flag_timeout)
        goto register_network;

    PRINTF_DEBUG("正在设置 GPRS 网络...\n");
    QUERY_AT(2000, "AT+CIPCSGP=1,\"CMNET\"", "OK");
    if (flag_timeout)
        goto register_network;

    PRINTF_DEBUG("正在设置 IP 协议头...\n");
    QUERY_AT(2000, "AT+CIPHEAD=1", "OK");
    if (flag_timeout)
        goto register_network;

    PRINTF_INFO("正在连接服务器 {'协议': '%s', '主机': '%s', '端口': '%s'}\n",
                protocol, host, port);
    QUERY_DATA(0, "AT+CIPSTART=", strlen("AT+CIPSTART="), "");
    QUERY_DATA(0, protocol, strlen(protocol), "");
    QUERY_DATA(0, ",", strlen(","), "");
    QUERY_DATA(0, host, strlen(host), "");
    QUERY_DATA(0, ",", strlen(","), "");
    QUERY_DATA(0, port, strlen(port), "");
    QUERY_AT(3000, "\r", "OK"); // 这里 3000ms 是等待模块返回的时间，不是连接时间
    if (flag_timeout)
        goto register_network;

    // 开始连接计时
    static int timer_connecting = 0;
    SETTIMER(timer_connecting);
    goto check_connect;

check_connect:
    if (flag_socket_close)
    {
        PRINTF_INFO("主动断开连接...\n");
        QUERY_AT(1000, "AT+CIPCLOSE", "OK");
        if (flag_timeout)
            goto check_at;
        flag_socket_close = 0;
        flag_socket_ready = 0;
        goto check_connect;
    }

    PRINTF_DEBUG("正在检测信号质量...\n");
    QUERY_AT(1000, "AT+CSQ", "+CSQ: ");
    if (flag_timeout)
        goto check_at;
    int xhzl1, xhzl2;

    sscanf(buf_serial_recv_last_find + 6, "%d,%d", &xhzl1, &xhzl2);
    PRINTF_INFO("信号质量<%d, %d>\n", xhzl1, xhzl2);

    PRINTF_DEBUG("正在确认连接状态...\n");
    QUERY_AT(1000, "AT+CIPSTATUS", "OK");
    if (flag_timeout)
        goto check_at;
    if (buf_serial_recv_find((char *)"CONNECT OK"))
    {
        PRINTF_INFO("已连接到服务器\n");
        flag_socket_ready = 1;
        goto communication;
    }
    if (buf_serial_recv_find((char *)"CONNECTING"))
    {
        // 8诫还没较撅
        if (TIMEOUT(timer_connecting, 8000))
        {
            PRINTF_WARN("连接超时，尝试重新连接\n");
            goto socket_connect_to_server;
        }
        PRINTF_INFO("\n");
        goto check_connect;
    }
    if (buf_serial_recv_find((char *)"ERROR"))
    {
        PRINTF_ERROR("连接错误，尝试重新连接\n");
        goto socket_connect_to_server;
    }
    if (buf_serial_recv_find((char *)"TCP CLOSED"))
    {
        PRINTF_ERROR("连接断开，尝试重新连接\n");
        flag_socket_ready = 0;
        goto socket_connect_to_server;
    }
    if (buf_serial_recv_find((char *)"IP INITIAL"))
    {
        PRINTF_DEBUG("未获得IP，尝试重新连接\n");
        goto socket_connect_to_server;
    }
    if (buf_serial_recv_find((char *)"IP CONFIG"))
    {
        PRINTF_DEBUG("正在获取IP\n");
        goto check_connect;
    }

    PRINTF_ERROR("未能确认链接状态\n");
    goto socket_connect_to_server;

communication:
    while (1)
    {
    watch_status:
        if (buf_serial_recv_find((char *)"CLOSED"))
        {
            PRINTF_DEBUG("接憋较匡...\n");
            flag_socket_ready = 0;
            // 皆讹
            goto socket_connect_to_server;
        }
    socket_recv:
        buf_serial_recv_last_find = buf_serial_recv_find((char *)"+IPD,");
        if (buf_serial_recv_last_find)
        {
            // 读取数据长度
            static unsigned int data_len;
            sscanf(buf_serial_recv_last_find + 5, "%d", &data_len);
            // 始位
            // FIXME: 错误处理：如果找不到冒号呢
            char *posColom;
            posColom = strstr(buf_serial_recv_last_find, ":");
            buf_serial_recv_last_find = posColom + 1;
            // 缓冲区指向数据开始的地方
            buf_serial_recv_trimleft(buf_serial_recv_last_find - buf_serial_recv);
            //buf_serial_recv_show();
            // 计算要接收的数据长度
            static unsigned int remain_data_len;
            remain_data_len = data_len;
            // 从 buf_serial_recv 转移数据到 buf_socket_recv
            while (remain_data_len)
            {
                {
                    // 调试信息
                    PRINTF_DEBUG("正在从服务器接收数据 (%d/%d bytes)...\n",
                                 data_len - remain_data_len, data_len);
                    // 详细信息
                    if (0)
                    {
                        // 显示缓冲区长度
                        PRINTF_DEBUG("buf_serial_recv_read_pipe_len: %d\n",
                                     buf_serial_recv_read_pipe_len());
                        PRINTF_DEBUG("buf_socket_recv_write_pipe_len: %d\n",
                                     buf_socket_recv_write_pipe_len());
                    }
                    // 计算需要搬运的数据长度
                    unsigned int move_length;
                    move_length = MIN(MIN(buf_serial_recv_read_pipe_len(),
                                          buf_socket_recv_write_pipe_len()),
                                      remain_data_len);
                    if (move_length)
                    {
                        // 转移数据
                        buf_socket_recv_write(buf_serial_recv_read_pipe(), move_length);
                        // 处理缓冲区
                        buf_serial_recv_trimleft(move_length);
                        // 计算剩余数据长度
                        remain_data_len -= move_length;
                        // 显示缓冲区内容
                        //buf_serial_recv_show();
                        //buf_socket_recv_show();
                    }
                }
                // 切换到其它任务上，防止阻塞
                STS_DOEVENTS();
            }
            PRINTF_DEBUG("正在从服务器接收数据 (完成)...\n");
        }
    socket_send:
        if (buf_socket_send_wi)
        {
            PRINTF_DEBUG("正在向服务器发送数据...\n");
            QUERY_AT(2000, "AT+CIPSEND", ">");
            if (flag_timeout)
                goto check_connect;
            // 注偷荽 \x1A 侥伙叫断凤
            QUERY_DATA(1000, buf_socket_send, buf_socket_send_wi,
                       "");                      // buf_serial_recv_find((char*)"") 直接返回
            QUERY_DATA(8000, "\x1A", strlen("\x1A"), "SEND OK"); // 使用 \x1A 发送数据
            if (flag_timeout)
                goto check_connect;
            buf_socket_send_wi = 0;
            PRINTF_DEBUG("数据发送完成\n");
        }
        STS_DOEVENTS();
    }
    STS_ENDED();
}
