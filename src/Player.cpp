#include "Player.h"
#include "math.h"

void Player::init(uint8_t* buf, DotMatrix3D* dm)
{
    for (int i = 0; i < 3; ++i)
        position[i] = 4;
    index[0] = 1;
    index[1] = 5;
    index[2] = 3;
    moveSign[0] = 1;
    moveSign[1] = 1;
    moveSign[2] = 1;
    on = true;
    for (int i = 0; i < 3; ++i)
        buf[index[i]] > 0 ? initMove[i] = - int(buf[index[i] + 1]) : initMove[i] = int(buf[index[i] + 1]);
    dm->setDot(int(position[0] + 0.5), int(position[1] + 0.5), int(position[2] + 0.5), true);
}

void Player::movePlayer(uint8_t* buf, DotMatrix3D* dm)
{
    int currentMove[3];
    float newPosition[3];
    for (int i = 0; i < 3; ++i)
    {
        newPosition[i] = position[i];
        buf[index[i]] > 0 ? currentMove[i] = -int(buf[index[i] + 1]) : currentMove[i] = int(buf[index[i] + 1]);
        int diff = currentMove[i] - initMove[i];
        if (abs(diff) < 10) continue;
        if (diff > 180) diff -= 360;
        else if (diff < -180) diff += 360;
        
        if (diff > 90) diff = 90;
        else if (diff < -90) diff = -90;

        newPosition[i] += moveSign[i] * diff / 200.0;
        if (newPosition[i] >= 7) newPosition[i] = 6.99;
        if (newPosition[i] <= 0) newPosition[i] = 0.01;
    }
    dm->setDot(int(position[0] + 0.5), int(position[1] + 0.5), int(position[2] + 0.5), false);
    dm->setDot(int(newPosition[0] + 0.5), int(newPosition[1] + 0.5), int(newPosition[2] + 0.5), on);
    on = ~on;
    for (int i = 0; i < 3; ++i)
        position[i] = newPosition[i];
}
