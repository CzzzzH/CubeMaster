#ifndef ZIGBEE_H
#define ZIGBEE_H

#include <deque>

using namespace std;

class Zigbee
{
public:
	static bool getBuffer(unsigned char* buf);
};

#endif