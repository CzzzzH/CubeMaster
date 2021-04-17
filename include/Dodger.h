#ifndef DODGER_H
#define DODGER_H

#include "Game.h"

class Dodger: public Game
{
public:
    uint8_t index[3], rainState[112];
    int moveSign[3];
    int initMove[3], activeSequence[112];
    int life, last;
    float position[3], rainPosition[112][3];
    bool playerOn;
    void init(uint8_t* buf, DotMatrix3D* dm);
    void update(uint8_t* buf);
    void movePlayer(uint8_t* buf);
    void moveRain();
    void updateLED(int initX, int initZ, int num);
};

#endif