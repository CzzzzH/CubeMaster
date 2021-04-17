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
    moveSign[1] = -1;
    moveSign[2] = -1;
    life = 8;
    playerOn = true;
    for (int i = 0; i < 3; ++i)
        buf[index[i]] > 0 ? initMove[i] = - int(buf[index[i] + 1]) : initMove[i] = int(buf[index[i] + 1]);
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 7; ++j)
            for (int k = 0; k < 8; ++k)
            {
                int index = i * 56 + j * 8 + k;
                activeSequence[index] = index;
                rainState[index] = 0;
                if (i == 0) rainPosition[index][0] = 0;
                else rainPosition[index][0] = 7;
                rainPosition[index][1] = j;
                rainPosition[index][2] = k;
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
        if (abs(diff) < 50) continue;
        if (diff > 180) diff -= 360;
        else if (diff < -180) diff += 360;
        
        if (diff > 90) diff = 90;
        else if (diff < -90) diff = -90;

        newPosition[i] += (moveSign[i] * diff) / 250.0;
        int upperBase, lowerBase;
        if (i == 0)
        {
            upperBase = 6;
            lowerBase = 1;
        }
        else if (i == 1)
        {
            upperBase = 6;
            lowerBase = 0;
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
    if (frame % 5 == 0 && frame < 560)
        rainState[activeSequence[frame / 5]] = 1;
    int moveSigns = 1;
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 7; ++j)
            for (int k = 0; k < 8; ++k)
            {
                int index = i * 56 + j * 8 + k;
                if (rainState[index] != 1) continue; 
                if (frame % 2 == 0)
                {
                    int randomOffset = random(0, 10);
                    if (randomOffset == 3) rainPosition[index][1] += moveSigns;
                    if (randomOffset == 4) rainPosition[index][2] += moveSigns;
                    if (randomOffset == 5) rainPosition[index][1] -= moveSigns;
                    if (randomOffset == 6) rainPosition[index][2] += moveSigns;
                    rainPosition[index][0] += moveSigns;
                }

                if (rainPosition[index][0] > 7 || rainPosition[index][0] < 0)
                    rainState[index] = 2;
                else if (rainPosition[index][1] > 6 || rainPosition[index][1] < 0)
                    rainState[index] = 2;
                else if (rainPosition[index][2] > 7 || rainPosition[index][2] < 0)
                    rainState[index] = 2;
                else if (rainPosition[index][0] == int(position[0] + 0.5) 
                    && rainPosition[index][1] == int(position[1] + 0.5)
                        && rainPosition[index][2] == int(position[2] + 0.5))
                        {
                            rainState[index] = 2;
                            life -= 2;
                        }
            }
        moveSigns = -1;
    }
    last = 0;
    for (int i = 0; i < 112; ++i)
    {
        if (rainState[i] < 2)
        {
            dm->setDot(rainPosition[i][0], rainPosition[i][1], rainPosition[i][2]);
            last++;
        }
        
    }
    if (life <= 0) win = 2;
    else if (last == 0) win = 1;
}

void Dodger::updateLED(int initX, int initZ, int num)
{
    switch (num)
    {
    case 0:
        dm->setDot(initX, 7, initZ);
        dm->setDot(initX, 7, initZ + 1);
        dm->setDot(initX, 7, initZ + 2);
        dm->setDot(initX, 7, initZ + 3);
        dm->setDot(initX, 7, initZ + 4);
        dm->setDot(initX + 1, 7, initZ);
        dm->setDot(initX + 1, 7, initZ + 4);
        dm->setDot(initX + 2, 7, initZ);
        dm->setDot(initX + 2, 7, initZ + 1);
        dm->setDot(initX + 2, 7, initZ + 2);
        dm->setDot(initX + 2, 7, initZ + 3);
        dm->setDot(initX + 2, 7, initZ + 4);
        break;
    case 1:
        dm->setDot(initX, 7, initZ);
        dm->setDot(initX + 1, 7, initZ);
        dm->setDot(initX + 1, 7, initZ + 2);
        dm->setDot(initX + 1, 7, initZ + 4);
        dm->setDot(initX + 2, 7, initZ);
        break;
    case 2:
        dm->setDot(initX, 7, initZ);
        dm->setDot(initX, 7, initZ + 1);
        dm->setDot(initX, 7, initZ + 2);
        dm->setDot(initX, 7, initZ + 4);
        dm->setDot(initX + 1, 7, initZ);
        dm->setDot(initX + 1, 7, initZ + 2);
        dm->setDot(initX + 1, 7, initZ + 4);
        dm->setDot(initX + 2, 7, initZ);
        dm->setDot(initX + 2, 7, initZ + 2);
        dm->setDot(initX + 2, 7, initZ + 3);
        dm->setDot(initX + 2, 7, initZ + 4);
        break;
    case 3:
        dm->setDot(initX, 7, initZ);
        dm->setDot(initX, 7, initZ + 2);
        dm->setDot(initX, 7, initZ + 4);
        dm->setDot(initX + 1, 7, initZ);
        dm->setDot(initX + 1, 7, initZ + 2);
        dm->setDot(initX + 1, 7, initZ + 4);
        dm->setDot(initX + 2, 7, initZ);
        dm->setDot(initX + 2, 7, initZ + 1);
        dm->setDot(initX + 2, 7, initZ + 2);
        dm->setDot(initX + 2, 7, initZ + 3);
        dm->setDot(initX + 2, 7, initZ + 4);
        break;
    case 4:
        dm->setDot(initX, 7, initZ + 2);
        dm->setDot(initX, 7, initZ + 3);
        dm->setDot(initX, 7, initZ + 4);
        dm->setDot(initX + 1, 7, initZ + 2);
        dm->setDot(initX + 2, 7, initZ);
        dm->setDot(initX + 2, 7, initZ + 1);
        dm->setDot(initX + 2, 7, initZ + 2);
        dm->setDot(initX + 2, 7, initZ + 3);
        dm->setDot(initX + 2, 7, initZ + 4);
        break;
    case 5:
        dm->setDot(initX, 7, initZ);
        dm->setDot(initX, 7, initZ + 2);
        dm->setDot(initX, 7, initZ + 3);
        dm->setDot(initX, 7, initZ + 4);
        dm->setDot(initX + 1, 7, initZ);
        dm->setDot(initX + 1, 7, initZ + 2);
        dm->setDot(initX + 1, 7, initZ + 4);
        dm->setDot(initX + 2, 7, initZ);
        dm->setDot(initX + 2, 7, initZ + 1);
        dm->setDot(initX + 2, 7, initZ + 2);
        dm->setDot(initX + 2, 7, initZ + 4);
        break;
    case 6:
        dm->setDot(initX, 7, initZ);
        dm->setDot(initX, 7, initZ + 1);
        dm->setDot(initX, 7, initZ + 2);
        dm->setDot(initX, 7, initZ + 3);
        dm->setDot(initX, 7, initZ + 4);
        dm->setDot(initX + 1, 7, initZ);
        dm->setDot(initX + 1, 7, initZ + 2);
        dm->setDot(initX + 1, 7, initZ + 4);
        dm->setDot(initX + 2, 7, initZ);
        dm->setDot(initX + 2, 7, initZ + 1);
        dm->setDot(initX + 2, 7, initZ + 2);
        dm->setDot(initX + 2, 7, initZ + 4);
        break;
    case 7:
        dm->setDot(initX, 7, initZ + 4);
        dm->setDot(initX + 1, 7, initZ + 4);
        dm->setDot(initX + 2, 7, initZ);
        dm->setDot(initX + 2, 7, initZ + 1);
        dm->setDot(initX + 2, 7, initZ + 2);
        dm->setDot(initX + 2, 7, initZ + 3);
        dm->setDot(initX + 2, 7, initZ + 4);
        break;
    case 8:
        dm->setDot(initX, 7, initZ);
        dm->setDot(initX, 7, initZ + 1);
        dm->setDot(initX, 7, initZ + 2);
        dm->setDot(initX, 7, initZ + 3);
        dm->setDot(initX, 7, initZ + 4);
        dm->setDot(initX + 1, 7, initZ);
        dm->setDot(initX + 1, 7, initZ + 2);
        dm->setDot(initX + 1, 7, initZ + 4);
        dm->setDot(initX + 2, 7, initZ);
        dm->setDot(initX + 2, 7, initZ + 1);
        dm->setDot(initX + 2, 7, initZ + 2);
        dm->setDot(initX + 2, 7, initZ + 3);
        dm->setDot(initX + 2, 7, initZ + 4);
        break;
    case 9:
        dm->setDot(initX, 7, initZ);
        dm->setDot(initX, 7, initZ + 2);
        dm->setDot(initX, 7, initZ + 3);
        dm->setDot(initX, 7, initZ + 4);
        dm->setDot(initX + 1, 7, initZ);
        dm->setDot(initX + 1, 7, initZ + 2);
        dm->setDot(initX + 1, 7, initZ + 4);
        dm->setDot(initX + 2, 7, initZ);
        dm->setDot(initX + 2, 7, initZ + 1);
        dm->setDot(initX + 2, 7, initZ + 2);
        dm->setDot(initX + 2, 7, initZ + 3);
        dm->setDot(initX + 2, 7, initZ + 4);
        break;
    default:
        break;
    }
}

void Dodger::update(uint8_t* buf)
{
    dm->clear();
    movePlayer(buf);
    moveRain();
    for (int i = 0; i < life; ++i)
        dm->setDot(i, 7, 7);
    if (last < 100)
    {
        updateLED(1, 0, last / 10);
        updateLED(4, 0, last % 10);
    }
    frame++;
}