#include "Zigbee.h"
#include <Arduino.h>

bool Zigbee::getBuffer(unsigned char* buf)
{
	if (Serial3.available() >= 8)
    {
        Serial3.readBytes(buf, 8);
        if (buf[0] == 233 && buf[1] <= 1 && buf[2] < 200 && buf[3] <= 1 && buf[4] < 200 
            && buf[5] <= 1 && buf[6] < 200 && buf[7] == 234) return true;
    }
    return false;
}
