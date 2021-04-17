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

#include <Arduino.h>
#include "Controller_A3D8.h"
#include "JY61.h"
#include "Zigbee.h"
#include "SnakePlayer.h"
#include "Dodger.h"

#define BYTE_LENGTH 64

extern HardwareSerial Serial1;

DotMatrix3D dm(1);
Controller_A3D8 cube(dm, Serial1);
Game* game;
byte * cache;
uint8_t initBuf[8], gameChoice = 0;
bool gameStart = false;

const uint8_t PROGMEM PATTERN_LOVE[] =
{ 0x00, 0x81, 0x81, 0xFF, // I
		0x38, 0xFC, 0xFE, 0x3F, //heart
		0x00, 0xFF, 0xFF, 0x01, // U
		};

const uint8_t PROGMEM PATTERN_ARROW[] =
{ 0x08, 0x14, 0x22, 0x77, 0x14, 0x14, 0x14, 0x14, 0x14, 0x1c, };

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
    Serial3.begin(115200);
    cube.sendBrightness(255);
    cube.sendMode(Controller_A3D8_Basic::OLD);
    dm.clear();
    cube.putDM();
    randomSeed(analogRead(0));
    while (!Zigbee::getBuffer(initBuf)) {}
    // Cube Program End
}

void animationFlash(word k)
{
	byte value = k & 0x01 ? 0xff : 0x00;
	cube.sendGlobal(value);
}

void animationBreathe(word k)
{
	k = k % 0x200;
	byte brightness = k < 0x100 ? 0xff - k : k - 0xff;

	cube.sendGlobal(0xff);
	cube.sendBrightness(brightness);
}

void animationFacetScan(word k)
{
	byte value = _BV(k%8);
	cube.sendGlobal(value);
}

void animationBlockScan(word k)
{
	k = k % 8;
	byte value = _BV(k+1) - 1;
	cube.sendGlobal(value);
}

void animationFlowZPosi(word k)
{
	dm.setMoveDirection(DotMatrix3D::Z_POSI);
	dm.move(false);
	for (byte i = 0; i < random(4); i++)
	{
		byte x = random(8);
		byte y = random(8);
		dm.setDot(x, y, 0);
	}
	cube.putDM();
}

void animationMoveSide(word k)
{
	k %= 144;
	if (k == 0 || k == 72)
		dm.clear(0x01);

	byte index;

	for (word i = dm.countBytes(); i;)
	{
		i--;
		byte temp = dm.getByte(i) & 0x81;
		dm.setByte(i, ((dm.getByte(i) & 0x7e) << 1) | temp);
	}

	if (bitRead(dm.orValue(),0) == true)
	{
		do
		{
			index = random(64);
		} while (dm.getByte(index) != 0x01);
		dm.setByte(index, 0x02);
	}

	k < 72 ? cube.putDM() : cube.putDMrevZ();

}

void animationMoveSideQuick(word k)
{
	k %= 16;

	if (k % 8 == 0)
	{
		for (byte i = 0; i < BYTE_LENGTH; i++)
		{
			cache[i] = _BV(random(8));
		}
	}

	if (k < 8)
		for (byte i = 0; i < BYTE_LENGTH; i++)
		{
			byte temp = dm.getByte(i);
			if (dm.getByte(i) < cache[i])
				dm.setByte(i, temp << 1);
		}
	else
	{
		for (byte i = 0; i < BYTE_LENGTH; i++)
		{
			byte temp = dm.getByte(i);
			if (dm.getByte(i) < 0x80)
				dm.setByte(i, temp << 1);
		}
	}

	cube.putDM();
}

void animationWave2D(word k)
{
	k %= 16;
	byte value = k < 8 ? k : 15 - k;

	dm.setMoveDirection(DotMatrix3D::Y_POSI);
	dm.move(false);

	dm.setByte(value, 0xff);
	cube.putDM();
}

void animationWave3D(word k)
{
	k %= 16;
	byte value = k < 8 ? k : 15 - k;

	for (byte i = 0; i < 8; i++)
		cache[i] = dm.getByte(i);

	dm.setMoveDirection(DotMatrix3D::Y_POSI);
	dm.move(false);

	for (byte i = 0; i < 8; i++)
		dm.setByte(i, cache[i]);

	for (byte i = 7; i; i--)
		dm.setByte(i, dm.getByte(i - 1));

	dm.setByte(0x00, _BV(value));

	cube.putDM();
}

void animationWaveShake(word k)
{
	k %= 16;
	byte value = k < 8 ? k : 15 - k;

	dm.setMoveDirection(DotMatrix3D::X_POSI);
	dm.move(false);
	dm.setLine(0, value, 7, 7 - value);

	cube.putDM();
}

void animationWaveRotate(word k)
{
	k %= 28;
	dm.setMoveDirection(DotMatrix3D::X_POSI);
	dm.move(false);

	byte temp = k % 7;
	cache[0] = 0;
	cache[1] = temp;
	cache[2] = 7;
	cache[3] = 7 - temp;

	byte i = k / 7;

	dm.setLine(cache[i % 4], cache[(i + 1) % 4], cache[(i + 2) % 4],
			cache[(i + 3) % 4]);

	cube.putDM();
}

void animationDance(word k)
{
	if (k == 0)
	{
		cache[0] = 0;
		cache[1] = 0;
	}

	char x = cache[0];
	char y = cache[1];

	dm.setMoveDirection(DotMatrix3D::Z_POSI);
	dm.move();

	char vx = random(3) - 1;
	char vy = random(3) - 1;

	x += vx;
	y += vy;

	if (x >= 8 || x < 0)
	{
		x -= vx + vx;
	}

	if (y >= 8 || y < 0)
	{
		y -= vy + vy;
	}

	cache[0] = x;
	cache[1] = y;

	dm.setDot(x, y, 0);
	cube.putDM();
}

void animationRotateArrow(word k)
{
	k %= 28;
	dm.rotate(0, false, false);
	if (k < 10)
		dm.setByte(0, pgm_read_byte_near(PATTERN_ARROW+k));
	cube.putDM();

}

void animationRotateLove(word k)
{
	k %= 28 * 6;

	if (k % 56 == 0)
	{
		for (byte i = 0; i < 4; i++)
		{
			byte value = DotMatrix::reverseByte(
					pgm_read_byte_near(PATTERN_LOVE+(k/56)*4+i));
			dm.setByte(0x20 + i, value);
			dm.setByte(0x27 - i, value);
			dm.setByte(0x18 + i, value);
			dm.setByte(0x1f - i, value);
		}
	}

	dm.rotateSync(true, true);

	cube.putDM();
}

void animationOneByOne(word k)
{
	if (k % 512 == 0)	dm.clear(0x00);

	byte col, row;

	if (dm.andValue() != 0xff)
	{
		do
		{
			col = random(64);
			row = random(8);
		} while (dm.getDot(col, row) == true);
		dm.DotMatrix::setDot(col, row, true);
	}

	k /= 512;
	if (k & 0x01)
		cube.putReverseDM();
	else
		cube.putDM();
}

void callAnimation(void (*p)(word), word span, word times, byte init_value,
		Controller_A3D8_Basic::InputMode mode)
{
	static word frame_id = 0;

	cube.sendMode(mode);
	dm.clear(init_value);

	while (times--)
	{
		(*p)(frame_id++);
		delay(span);
	}
	frame_id = 0;
}

void callAnimationInModes(void (*p)(word), word span, word times,
		byte init_value)
{
	for (byte i = 0; i < 3; i++)
		callAnimation(p, span, times, init_value,
				(Controller_A3D8_Basic::InputMode) i);
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

void showResult(int win)
{   
    if (win == 2)
    {
        for (int i = 0; i < 8; ++i)
        {
            dm.setDot(i, 7, i);
            dm.setDot(i, 7, 7 - i);
        }
    }
    else
    {
        callAnimation(animationFlash, 0xF0, 0x08, 0x00, Controller_A3D8_Basic::XYZ);
        callAnimation(animationBreathe, 0x08, 0xff * 4, 0xff, Controller_A3D8_Basic::XYZ);
        callAnimationInModes(animationFacetScan, 0x40, 0x08, 0x00);
        callAnimationInModes(animationBlockScan, 0x40, 0x08, 0x00);
        callAnimationInModes(animationFlowZPosi, 0x40, 0x40, 0x00);

        callAnimationInModes(animationMoveSide, 0x30, 144, 0x00);
        callAnimationInModes(animationMoveSideQuick, 0x40, 0x10, 0x01);

        callAnimationInModes(animationWave2D, 0x40, 14 * 4, 0x00);
        callAnimation(animationWave3D, 0x40, 14 * 8, 0x00, Controller_A3D8_Basic::ZXY);
        callAnimation(animationWaveShake, 0x40, 14 * 8, 0x00, Controller_A3D8_Basic::XYZ);

        callAnimation(animationWaveRotate, 0x20, 14 * 8, 0x00, Controller_A3D8_Basic::XYZ);
        callAnimation(animationWaveRotate, 0x20, 14 * 8, 0x00, Controller_A3D8_Basic::YZX);

        callAnimation(animationDance, 0x20, 0x80, 0x00, Controller_A3D8_Basic::ZXY);

        callAnimation(animationRotateArrow, 0x20, 28 * 3, 0x00, Controller_A3D8_Basic::YZX);
        callAnimation(animationRotateArrow, 0x30, 28 * 3, 0x00, Controller_A3D8_Basic::ZXY);

        callAnimation(animationRotateLove, 0x30, 28 * 6, 0x00, Controller_A3D8_Basic::ZXY);

        callAnimation(animationOneByOne, 0x10, 0x400, 0x00, Controller_A3D8_Basic::XYZ);
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
    // // Serial.println(buf[2]);
    // // Serial.println(buf[4]);
    // // Serial.println(buf[6]);
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
        if (gameStart)
        {
            game->update(buf);
            if (game->win != 0)
            {
                showResult(game->win);
                gameStart = false;
            }
        }
        else
        {
            dm.clear();
            int zPos, diff, currentMove, initMove;
            initBuf[3] > 0 ? initMove = -int(initBuf[4]) : initMove = int(initBuf[4]);
            buf[3] > 0 ? currentMove = -int(buf[4]) : currentMove = int(buf[4]);
            diff = currentMove - initMove;
            if (diff > 180) diff -= 360;
            else if (diff < -180) diff += 360;

            if (abs(diff) > 30)
            {   
                if (diff < 0) gameChoice = 1;
                else gameChoice = 0;
            }

            initBuf[5] > 0 ? initMove = -int(initBuf[6]) : initMove = int(initBuf[6]);
            buf[5] > 0 ? currentMove = -int(buf[6]) : currentMove = int(buf[6]);
            diff = currentMove - initMove;
            if (diff > 180) diff -= 360;
            else if (diff < -180) diff += 360;

            if (abs(diff) > 60)
            {   
                if (gameChoice == 0) game = new Dodger();
                else game = new Dodger();
                game->init(initBuf, &dm);
                gameStart = true;
                return;
            }

            if (gameChoice == 0) zPos = 0;
            else zPos = 7;
            for (int i = 0; i < 7; ++i)
            {
                dm.setDot(i, 0, zPos);
                dm.setDot(i, 7, zPos);
                dm.setDot(0, i, zPos);
                dm.setDot(7, i, zPos);
            }
            // 1
            for (int i = 2; i < 6; ++i)
                dm.setDot(4, i, 0);
            dm.setDot(3, 4, 0);
            // 2
            for (int i = 2; i < 6; ++i)
                dm.setDot(i, 1, 7);
            dm.setDot(2, 2, 7);
            dm.setDot(3, 3, 7);
            dm.setDot(4, 4, 7);
            dm.setDot(5, 5, 7);
            dm.setDot(2, 5, 7);
            dm.setDot(3, 6, 7);
            dm.setDot(4, 6, 7);
        }
        
        cube.putDM();
        delay(50);
    }
    // Cube Program End
}

