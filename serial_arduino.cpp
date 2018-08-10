#include "serial_arduino.h"

#ifdef ARDUINO
#include <Arduino.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); // RX=2,TX=3
void init_serial()
{
    mySerial.begin(115200);
    Serial.begin(115200);
}

unsigned int serial_write(char *buffer, unsigned int buffer_length)
{
    mySerial.write(buffer, buffer_length);
    return buffer_length;
}

unsigned int serial_read(char *buffer, unsigned int buffer_length)
{
    unsigned int read_len = 0;
    while (mySerial.available() && (read_len++ < buffer_length))
    {
        *buffer++ = mySerial.read();
    }
    return read_len;
}

#endif
