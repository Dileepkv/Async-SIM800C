#ifdef _WINDOWS_
#include "serial_windows.h"
#pragma once
#include <windows.h>
HANDLE hCom;

void init_serial()
{
	hCom = CreateFile(
		TEXT(DEBUG_SERIAL_PORT),	  //�����ļ�
		GENERIC_READ | GENERIC_WRITE, //�����?
		0,							  //ָ���������ԣ����ڴ��ڲ��ܹ������Ըò�������Ϊ0
		NULL,						  //
		OPEN_EXISTING,				  //�򿪶����Ǵ���
		FILE_ATTRIBUTE_NORMAL,		  //������������ֵΪFILE_FLAG_OVERLAPPED����ʾʹ���첽I/O���ò���Ϊ0����ʾͬ��I/O����
		NULL);

	if (hCom == INVALID_HANDLE_VALUE)
	{
		printf("[DEBUG   ]" "��" DEBUG_SERIAL_PORT "ʧ��!\n");
		return;
	}
	else
	{
		printf("[DEBUG   ]" DEBUG_SERIAL_PORT "�򿪳ɹ���\n");
	}

	SetupComm(hCom, 1024, 1024); //���뻺����������������Ĵ�С����?1024
	/*********************************��ʱ����**************************************/
	COMMTIMEOUTS TimeOuts;
	//�趨����ʱ
	TimeOuts.ReadIntervalTimeout = MAXDWORD; //������?
	TimeOuts.ReadTotalTimeoutMultiplier = 0; //��ʱ��ϵ��
	TimeOuts.ReadTotalTimeoutConstant = 0;   //��ʱ�䳣��
	//�趨д��ʱ
	TimeOuts.WriteTotalTimeoutMultiplier = 1; //дʱ��ϵ��
	TimeOuts.WriteTotalTimeoutConstant = 1;   //дʱ�䳣��
	SetCommTimeouts(hCom, &TimeOuts);		  //���ó�ʱ

	/*****************************************���ô���***************************/
	DCB dcb;
	GetCommState(hCom, &dcb);
	dcb.BaudRate = 115200;	 //������Ϊ9600
	dcb.ByteSize = 8;		   //ÿ���ֽ���8λ
	dcb.Parity = NOPARITY;	 //����żУ��λ
	dcb.StopBits = ONESTOPBIT; //һ��ֹͣλ
	SetCommState(hCom, &dcb);
}

unsigned int serial_write(char *buffer, unsigned int buffer_length)
{
	DWORD wCount; // ʵ��д����ֽ���?
	unsigned int bWriteStat = WriteFile(hCom, buffer, buffer_length, &wCount, NULL);
	if (!bWriteStat)
	{
		printf("[DEBUG   ]" "д����ʧ��!");
		init_serial();
		return 0;
	}
	return wCount;
}

unsigned int serial_read(char *buffer, unsigned int buffer_length)
{
	DWORD wCount; //ʵ�ʶ�ȡ���ֽ���
	// PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR); //��ջ�����?
	unsigned int bReadStat = ReadFile(hCom, buffer, buffer_length, &wCount, NULL);
	if (!bReadStat)
	{
		printf("[DEBUG   ]" "������ʧ��!");
		init_serial();
		return 0;
	}
	return wCount;
	// CloseHandle(hCom);
}



#undef DEBUG_SERIAL_PORT

#endif
