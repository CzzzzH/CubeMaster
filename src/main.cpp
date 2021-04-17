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

#include "Controller_A3D8.h"
#include "JY61.h"
#include "Zigbee.h"
#include "SnakePlayer.h"
#include <Arduino.h>
#include "Dodger.h"

#define BYTE_LENGTH 64

extern HardwareSerial Serial1;

DotMatrix3D dm(1);
Controller_A3D8 cube(dm, Serial1);
Game* game;
byte * cache;
uint8_t initBuf[8];
bool gameStart = false;

void setup()
{   
    // Player's Controller Program Begin
    // Serial.begin(9600);
    // Serial2.begin(115200);
    // Serial3.begin(115200);
    // Player's Controller Program End

    // Cube Program Begin
	  cache = new byte(BYTE_LENGTH);
    Serial.begin(9600);
    Serial1.begin(57600);
    cube.sendBrightness(255);
    cube.sendMode(Controller_A3D8_Basic::OLD);
    dm.clear();
    cube.putDM();
    randomSeed(analogRead(0));
    while (!Zigbee::getBuffer(initBuf)) {}
    game = new Dodger();
    game->init(initBuf, &dm);
    // Cube Program End
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

void testAll()
{
    int now_x = 0, now_y = 0, now_z = 0;
    dm.setDot(0, 0, 0);
    cube.putDM();
    delay(50);
    for (auto x = 0; x < 8; ++x)
        for (auto y = 0; y < 8; ++y)
            for (auto z = 0; z < 8; ++z)
            {
                dm.setDot(now_x, now_y, now_z, false);
                now_x = x;
                now_y = y;
                now_z = z;
                dm.setDot(now_x, now_y, now_z);
                cube.putDM();
                delay(100);
            }
}

void loop()
{   
    // Player's Controller Program Begin
    // JY61::read();
    // unsigned char buf[8];
    // buf[0] = 233;
    // buf[2] = (unsigned char)((int)(abs(JY61::Angle[0])));
    // buf[4] = (unsigned char)((int)(abs(JY61::Angle[1])));
    // buf[6] = (unsigned char)((int)(abs(JY61::Angle[2])));
    // buf[7] = 234;
    // JY61::Angle[0] >= 0 ? buf[1] = 0 : buf[1] = 1;
    // JY61::Angle[1] >= 0 ? buf[3] = 0 : buf[3] = 1;
    // JY61::Angle[2] >= 0 ? buf[5] = 0 : buf[5] = 1;
    // while (Serial3.availableForWrite() < 8) {}
    // Serial3.write(buf, 8);
    // Player's Controller Program End

    // Cube Program Begin
    // testAll();
    unsigned char buf[8];
    if (Zigbee::getBuffer(buf))
    {   
        dm.clear();
        if (gameStart)
            game->update(buf);
        cube.putDM();
        delay(50);
    }
    // Cube Program End
}

