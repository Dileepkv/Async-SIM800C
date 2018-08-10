/*
    This code should be pasted within the files where this function is needed.
    This function will not create any code conflicts.

    The function call is similar to printf: ardprintf("Test %d %s", 25,
   "string"); To print the '%' character, use '%%' This code was first posted on
   http://arduino.stackexchange.com/a/201
*/

// 更新(20180811) 增加了 \n 的支持

#ifndef ARDPRINTF
#define ARDPRINTF
#define ARDBUFFER 32
#include <Arduino.h>
#include <stdarg.h>

extern "C" int ardprintf(const char *str, ...)
{
	int i, count = 0, j = 0, flag = 0;
	char temp[ARDBUFFER + 1];
	for (i = 0; str[i] != '\0'; i++)
		if (str[i] == '%')
			count++;

	va_list argv;
	va_start(argv, count);
	for (i = 0, j = 0; str[i] != '\0'; i++)
	{
		if (str[i] == '\n')
		{
			temp[j] = '\0';
			Serial.print(temp);
			j = 0;
			temp[0] = '\0';
			Serial.println();
		}
		else if (str[i] == '%')
		{
			temp[j] = '\0';
			Serial.print(temp);
			j = 0;
			temp[0] = '\0';
			switch (str[++i])
			{
			case 'd':
				Serial.print(va_arg(argv, int));
				break;
			case 'l':
				Serial.print(va_arg(argv, long));
				break;
			case 'f':
				Serial.print(va_arg(argv, double));
				break;
			case 'c':
				Serial.print((char)va_arg(argv, int));
				break;
			case 's':
				Serial.print(va_arg(argv, char *));
				break;
			default:;
			};
		}
		else
		{
			temp[j] = str[i];
			j = (j + 1) % ARDBUFFER;
			if (j == 0)
			{
				temp[ARDBUFFER] = '\0';
				Serial.print(temp);
				temp[0] = '\0';
			}
		}
	};
	return count + 1;
}

#undef ARDBUFFER
#endif
