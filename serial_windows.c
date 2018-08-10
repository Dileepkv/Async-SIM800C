#ifdef WINDOWS
#include "serial_windows.h"
#include "printf.h"
#include <windows.h>

HANDLE hCom;

void init_serial()
{
	hCom = CreateFile(TEXT(DEBUG_SERIAL_PORT), GENERIC_READ | GENERIC_WRITE, 0,
					  NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hCom == INVALID_HANDLE_VALUE)
	{
		PRINTF_ERROR("串口 %s 打开失败！\n", DEBUG_SERIAL_PORT);
		return;
	}
	else
	{
		PRINTF_DEBUG("串口 %s 打开成功！\n", DEBUG_SERIAL_PORT);
	}

	SetupComm(hCom, 1024, 1024);

	//
	COMMTIMEOUTS TimeOuts;

	TimeOuts.ReadIntervalTimeout = MAXDWORD;
	TimeOuts.ReadTotalTimeoutMultiplier = 0;
	TimeOuts.ReadTotalTimeoutConstant = 0;

	TimeOuts.WriteTotalTimeoutMultiplier = 1;
	TimeOuts.WriteTotalTimeoutConstant = 1;
	SetCommTimeouts(hCom, &TimeOuts);

	//
	DCB dcb;
	GetCommState(hCom, &dcb);
	dcb.BaudRate = 115200;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	SetCommState(hCom, &dcb);
}

unsigned int serial_write(char *buffer, unsigned int buffer_length)
{
	DWORD wCount;
	unsigned int bWriteStat =
		WriteFile(hCom, buffer, buffer_length, &wCount, NULL);
	if (!bWriteStat)
	{
		PRINTF_DEBUG("FAILED TO WRITE SERIAL!\n");
		init_serial();
		return 0;
	}
	return wCount;
}

unsigned int serial_read(char *buffer, unsigned int buffer_length)
{
	DWORD wCount;

	unsigned int bReadStat = ReadFile(hCom, buffer, buffer_length, &wCount, NULL);
	if (!bReadStat)
	{
		PRINTF_DEBUG("FAILED TO READ SERIAL!\n");
		init_serial();
		return 0;
	}
	return wCount;
}

#undef DEBUG_SERIAL_PORT

#endif
