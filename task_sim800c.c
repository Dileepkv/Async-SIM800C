// ����
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
 * �������ӷ�����
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
 * ģ��ͨ��ָ��
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
 * ѭ������
 */
void task_sim800c()
{
    STS_START();

check_at:
    printf("[INFO    ]"
           "����AT(����)ģ��...\n");
    QUERY_AT(1000, "AT", "OK");
    if (flag_timeout)
        goto check_at; //����ATͨ��״��

close_echo:
    printf("[INFO    ]"
           "���ڹرջ���...\n");
    QUERY_AT(1000, "ATE0", "OK");
    if (flag_timeout)
        goto check_at; //�ر��ƶ�����

ready:
    if (flag_server_configured)
    {
        goto register_network;
    }
    STS_DOEVENTS();
    goto ready;

register_network:
    printf("[INFO    ]"
           "����ע�ᵽ����...\n");
    QUERY_AT(1000, "AT+CREG?", "+CREG: ");
    if (flag_timeout)
        goto check_at; //�ر��ƶ�����
    // TODO: �������Ӱ�ȫ���
    do
    {
        char *lastFind = buf_serial_recv_find((char*)"+CREG: ");
        if (strlen(lastFind) > 9 && (lastFind[9] == '1' || lastFind[9] == '5'))
        {
            printf("[INFO    ]"
                   "�����ӵ����磬%s, ģʽ<%3.3s>\n",
                   lastFind[9] == '1' ? "��������" : "��������", lastFind + 7);
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
           "���ڹر��ƶ�����...\n");
    QUERY_AT(2000, "AT+CIPSHUT", "SHUT OK");
    if (flag_timeout)
        goto register_network;

    printf("[INFO    ]"
           "��������GPRS�ƶ�̨���ΪB,֧�ְ����������ݽ���...\n");
    QUERY_AT(2000, "AT+CGCLASS=\"B\"", "OK");
    if (flag_timeout)
        goto register_network;

    printf("[INFO    ]"
           "��������PDP������,��������Э��,��������Ϣ...\n");
    QUERY_AT(2000, "AT+CGDCONT=1,\"IP\",\"CMNET\"", "OK");
    if (flag_timeout)
        goto register_network;

    printf("[INFO    ]"
           "���ڸ���GPRSҵ��...\n");
    QUERY_AT(2000, "AT+CGATT=1", "OK");
    if (flag_timeout)
        goto register_network;

    printf("[INFO    ]"
           "��������ΪGPRS����ģʽ...\n");
    QUERY_AT(2000, "AT+CIPCSGP=1,\"CMNET\"", "OK");
    if (flag_timeout)
        goto register_network;

    printf("[INFO    ]"
           "�������ý���������ʾIPͷ(�����ж�������Դ,���ڵ�·������Ч)...\n");
    QUERY_AT(2000, "AT+CIPHEAD=1", "OK");
    if (flag_timeout)
        goto register_network;

    printf("[INFO    ]"
           "�������ӵ������� {'Э��': '%s', '����': '%s', '�˿�': '%s'}\n",
           protocol, host, port);
    QUERY_DATA(0, "AT+CIPSTART=", strlen("AT+CIPSTART="), "");
    QUERY_DATA(0, protocol, strlen(protocol), "");
    QUERY_DATA(0, ",", strlen(","), "");
    QUERY_DATA(0, host, strlen(host), "");
    QUERY_DATA(0, ",", strlen(","), "");
    QUERY_DATA(0, port, strlen(port), "");
    QUERY_AT(3000, "\r", "OK");
    // 3000ms �ǵȴ�ģ����Ӧ��ʱ��
    static int timer_connecting = 0;
    SETTIMER(timer_connecting);
    if (flag_timeout)
        goto register_network;

check_connect:
    if (flag_socket_close)
    {
        printf("[INFO    ]"
               "���ڶϿ�����...\n");
        QUERY_AT(1000, "AT+CIPCLOSE", "OK");
        flag_socket_close = 0;
        flag_socket_ready = 0;
        goto check_connect;
    }

    printf("[INFO    ]"
           "���ڲ鿴�ź�����...\n");
    QUERY_AT(1000, "AT+CSQ", "+CSQ: ");
    if (flag_timeout)
        goto check_at;
    int xhzl1, xhzl2;

    sscanf(buf_serial_recv_last_find + 6, "%d,%d", &xhzl1, &xhzl2);
    printf("[INFO    ]"
           "�ź�����<%d, %d>\n",
           xhzl1, xhzl2);

    printf("[INFO    ]"
           "����ȷ����������...\n");
    QUERY_AT(1000, "AT+CIPSTATUS", "OK");
    if (flag_timeout)
        goto check_at;
    if (buf_serial_recv_find((char*)"CONNECT OK"))
    {
        printf("[INFO    ]"
               "ȷ�������ӵ�������\n");
        flag_socket_ready = 1;
        goto communication;
    }
    if (buf_serial_recv_find((char*)"CONNECTING"))
    {
        // 8�뻹û���Ͼ�����
        if (TIMEOUT(timer_connecting, 8000))
        {
            printf("[WRANING ]"
                   "���ӷ�������ʱ��������������\n");
            goto connect_to_server;
        }
        printf("[INFO    ]"
               "��������\n");
        goto check_connect;
    }
    if (buf_serial_recv_find((char*)"ERROR"))
    {
        printf("[ERROR   ]"
               "���Ӵ���\n");
        goto connect_to_server;
    }
    if (buf_serial_recv_find((char*)"TCP CLOSED"))
    {
        printf("[ERROR   ]"
               "�����ѶϿ�\n");
        flag_socket_ready = 0;
        goto connect_to_server;
    }
    if (buf_serial_recv_find((char*)"IP INITIAL"))
    {
        printf("[INFO    ]"
               "��δ��ȡ����IP\n");
        goto connect_to_server;
    }
    if (buf_serial_recv_find((char*)"IP CONFIG"))
    {
        printf("[INFO    ]"
               "���ڻ�ȡ����IP\n");
        goto check_connect;
    }

    printf("[INFO    ]"
           "δ��ȷ������״̬�������������ӷ�����\n");
    goto connect_to_server;

communication:
    while (1)
    {
    watch_status:
        if (buf_serial_recv_find((char*)"CLOSED"))
        {
            printf("[DEBUG   ]"
                   "���ӱ��������Ͽ�...\n");
            flag_socket_ready = 0;
            // �Զ�����
            goto connect_to_server;
        }
    socket_recv:
        buf_serial_recv_last_find = buf_serial_recv_find((char*)"+IPD,");
        if (buf_serial_recv_last_find)
        {
            PRINT_LINE();
            // �յ������ݳ���
            static unsigned int data_len;
            sscanf(buf_serial_recv_last_find + 5, "%d", &data_len);
            // ������ʼλ��
            // TODO: ��ȫ��飺���û���ҵ�ð��
            char *posColom;
            posColom = strstr(buf_serial_recv_last_find, ":");
            buf_serial_recv_last_find = posColom + 1;
            // �ӻ��������Ƴ�����֮ǰ���ַ�
            buf_serial_recv_trimleft(buf_serial_recv_last_find - buf_serial_recv);
            buf_serial_recv_show();
            PRINT_LINE();
            // ��ʼ��������
            static unsigned int remain_data_len;
            remain_data_len = data_len;
            // �� buf_serial_recv ת�ƶ�Ӧ���ȵ����ݵ� buf_socket_recv
            while (remain_data_len)
            {
                {
                    // ���������Ϣ
                    printf("[DEBUG   ]"
                           "�ӷ������������� (%d/%d bytes)...\n",
                           data_len - remain_data_len, data_len);
                    // ����ϸ�ĵ�����Ϣ
                    if (0)
                    {
                        printf("[DEBUG   ]"
                               "buf_serial_recv_read_pipe_len: %d\n",
                               buf_serial_recv_read_pipe_len());
                        printf("[DEBUG   ]"
                               "buf_socket_recv_write_pipe_len: %d\n",
                               buf_socket_recv_write_pipe_len());
                    }

                    // �������ת�ƵĻ������ֽ���
                    unsigned int move_length;
                    move_length = MIN(MIN(buf_serial_recv_read_pipe_len(),
                                          buf_socket_recv_write_pipe_len()),
                                      remain_data_len);
                    if (move_length)
                    {
                        // ת�ƶ�Ӧ�ֽ���
                        buf_socket_recv_write(buf_serial_recv_read_pipe(), move_length);
                        // �� sim800c ���������Ƴ��Ѿ�ת�Ƶ�����
                        buf_serial_recv_trimleft(move_length);
                        // ����ʣ����Ҫת�Ƶ��ֽ���
                        remain_data_len -= move_length;
                        // ����
                        buf_serial_recv_show();
                        buf_socket_recv_show();
                    }
                }
                // ��ֹ����
                STS_DOEVENTS();
            }
            printf("[DEBUG   ]"
                   "�ӷ������������� (���)...\n",
                   data_len - remain_data_len, data_len);
        }
    socket_send:
        if (buf_socket_send_wi)
        {
            printf("[DEBUG   ]"
                   "���ڸ���������������...\n");
            QUERY_AT(2000, "AT+CIPSEND", ">");
            if (flag_timeout)
                goto check_connect;
            // ע������������͵����ݴ��� \x1A �Ļ����жϷ���
            QUERY_DATA(1000, buf_socket_send, buf_socket_send_wi,
                       "");                      // buf_serial_recv_find((char*)"") ֱ�ӷ���
            QUERY_AT(8000, "\x1A\0", "SEND OK"); // ʹ�� \x1A �������ݷ���
            if (flag_timeout)
                goto check_connect;
            buf_socket_send_wi = 0;
            printf("[DEBUG   ]"
                   "���ݷ��ͳɹ���\n");
        }
        STS_DOEVENTS();
    }
    STS_ENDED();
}
