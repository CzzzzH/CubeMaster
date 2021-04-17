#include "Dodger.h"
#include "math.h"
#include <algorithm> 

using namespace std;

void Dodger::init(uint8_t* buf, DotMatrix3D* dm)
{
    this->dm = dm;
    for (int i = 0; i < 3; ++i)
        position[i] = 4;
    index[0] = 1;
    index[1] = 5;
    index[2] = 3;
    moveSign[0] = 1;
    moveSign[1] = 1;
    moveSign[2] = 1;
    playerOn = true;
    for (int i = 0; i < 3; ++i)
        buf[index[i]] > 0 ? initMove[i] = - int(buf[index[i] + 1]) : initMove[i] = int(buf[index[i] + 1]);
    for (int i = 0; i < 1; ++i)
        for (int j = 0; j < 6; ++j)
            for (int k = 0; k < 7; ++k)
            {
                activeSequence[i * 56 + j * 8 + k] = i * 56 + j * 8 + k;
                rainState[i * 56 + j * 8 + k] = 0;
                if (i == 0) rainPosition[i * 56 + j * 8 + k][0] = 0;
                else rainPosition[i * 56 + j * 8 + k][0] = 7;
                rainPosition[i * 56 + j * 8 + k][1] = j + 1;
                rainPosition[i * 56 + j * 8 + k][2] = k;
            }

    for (int i = 0; i < 112; ++i)
    {
        int j = random(i, 112);
        swap(activeSequence[i], activeSequence[j]);
    }
    frame = 0;
}

void Dodger::movePlayer(uint8_t* buf)
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
        int upperBase, lowerBase;
        if (i == 0)
        {
            upperBase = 6;
            lowerBase = 1;
        }
        else if (i == 1)
        {
            upperBase = 7;
            lowerBase = 1;
        }
        else
        {
            upperBase = 7;
            lowerBase = 0;
        }
        if (newPosition[i] <= lowerBase) newPosition[i] = lowerBase + 0.001;
        if (newPosition[i] >= upperBase) newPosition[i] = upperBase - 0.001;
    }
    dm->setDot(int(newPosition[0] + 0.5), int(newPosition[1] + 0.5), int(newPosition[2] + 0.5), playerOn);
    if (playerOn == true) playerOn = false;
    else playerOn = true;
    for (int i = 0; i < 3; ++i)
        position[i] = newPosition[i];
}

void Dodger::moveRain()
{
    if (frame % 10 == 0)
        rainState[activeSequence[frame / 10]] = 1;
    int moveSign = 1;
    for (int i = 0; i < 1; ++i)
    {
        for (int j = 0; j < 6; ++j)
            for (int k = 0; k < 7; ++k)
            {
                rainState[i * 56 + j * 8 + k] = 0;
                if (i == 0) rainPosition[i * 56 + j * 8 + k][0] = 0;
                else rainPosition[i * 56 + j * 8 + k][0] = 7;
                rainPosition[i * 56 + j * 8 + k][1] = j + 1;
                rainPosition[i * 56 + j * 8 + k][2] = k;
            }
        moveSign = -1;
    }
    for (int i = 0; i < 112; ++i)
        if (rainState[i] < 2) dm->setDot(rainPosition[i][0], rainPosition[i][1], rainPosition[i][2]);
}

void Dodger::update(uint8_t* buf)
{
    movePlayer(buf);
    moveRain();
    frame++;
}