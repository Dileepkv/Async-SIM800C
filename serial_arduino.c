#include "serial_arduino.h"
#pragma once

#ifdef _ARDUINO_

#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); // RX=2,TX=3
void init_serial()
{
	mySerial.begin(115200);
	Serial.begin(115200);
}

uint serial_write(char *buffer, uint buffer_length)
{
	mySerial.write(buffer, buffer_length);
	return buffer_length;
}

uint serial_read(char *buffer, uint buffer_length)
{
	uint read_len = 0;
	while (mySerial.available() && (read_len++ < buffer_length))
	{
		*buffer++ = mySerial.read();
	}
	return read_len;
}

#endif
