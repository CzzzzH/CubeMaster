#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include "DotMatrix3D.h"

class Game
{
public:
    int frame;
    int win = 0;
    DotMatrix3D* dm;
    virtual void init(uint8_t* buf, DotMatrix3D* dm) = 0;
    virtual void update(uint8_t* buf) = 0;
};

#endif