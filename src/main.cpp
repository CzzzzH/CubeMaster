/*
 *  A3D8_Controller_Advanced.ino
 * 
 *	animation with the support of dot-matrix library
 *	Advanced animation sample on arduino managing 3D8S 8x8x8 led cubic display
 *	by connecting the serial port on 3D8S board (P2)
 *
 *	P2.1 -> VCC (5V)
 *	P2.2 -> #1 (TX)
 *	P2.3 -> #0 (RX)
 *	P2.4 -> GND (0V)
 *
 *	Disconnection would be needed when upload the sketch to arduino.
 *
 *  Created on: 2012-7-3
 *	Author: Weihong Guan
 *	Blog: http://aguegu.net
 *	E-mail: weihong.guan@gmail.com
 *
 *	Code license: Attribution-NonCommercial-ShareAlike 3.0 Unported (CC BY-NC-SA 3.0)
 *	http://creativecommons.org/licenses/by-nc-sa/3.0/
 *	source host: https://github.com/aguegu/dot-matrix
 *
 */

#include "DotMatrix3D.h"
#include "Controller_A3D8.h"

#define BYTE_LENGTH 64

extern HardwareSerial Serial1;

DotMatrix3D dm(1);
Controller_A3D8 cube(dm, Serial1);

byte * cache;

const uint8_t PROGMEM PATTERN_LOVE[] =
{ 0x00, 0x81, 0x81, 0xFF, // I
		0x38, 0xFC, 0xFE, 0x3F, //heart
		0x00, 0xFF, 0xFF, 0x01, // U
		};

const uint8_t PROGMEM PATTERN_ARROW[] =
{ 0x08, 0x14, 0x22, 0x77, 0x14, 0x14, 0x14, 0x14, 0x14, 0x1c, };

void setup()
{
	delay(0x200);

	cache = new byte(BYTE_LENGTH);
	Serial1.begin(57600);
	cube.sendBrightness(255);
}

// 测试背景白灯模式
void testBackgroundLED()
{
    for (auto i = 0; i < 7; ++i)
    {
        cube.sendBgLed(i);
        delay(1000);
    }
}

// 显示XYZ轴方向的测试函数
void testXYZ(Controller_A3D8_Basic::InputMode mode)
{
    cube.sendMode(mode);
    dm.clear();
    cube.putDM();
    for (auto x = 0; x < 8; ++x)
    {
        dm.setDot(x, 0, 0);
        cube.putDM();
        delay(50);
    }
    delay(1000);
    for (auto y = 0; y < 8; ++y)
    {
        dm.setDot(0, y, 0);
        cube.putDM();
        delay(50);
    }
    delay(1000);
    for (auto z = 0; z < 8; ++z)
    {
        dm.setDot(0, 0, z);
        cube.putDM();
        delay(50);
    }
}

void loop()
{
    testBackgroundLED();
    testXYZ(Controller_A3D8_Basic::OLD);
    delay(3000);
}

