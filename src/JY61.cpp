#include <JY901.h>
#include <Wire.h>
#include <Arduino.h>
#include <string.h>
#include "JY61.h"
#include "math.h"

void JY61::read()
{
	static int count = 0;
	static double last_Angle = 0;
	while (Serial2.available())
		JY901.CopeSerialData(Serial2.read()); //Call JY901 data cope function
	for (int i = 0; i < 3; i++)
		Angle[i] = (double)JY901.stcAngle.Angle[i] / 32768 * 180;
}

double JY61::Angle[3];
