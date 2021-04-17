#ifndef PLAYER_H
#define PLAYER_H

#include <Arduino.h>
#include "DotMatrix3D.h"

class Player
{
public:
    uint8_t index[3], moveSign[3];
    float position[3];
    int initMove[3];
    bool on;
    void init(uint8_t* buf, DotMatrix3D* dm);
    void movePlayer(uint8_t* buf, DotMatrix3D* dm);
};

#endif