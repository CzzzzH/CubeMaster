#ifndef SNAKEPLAYER_H
#define SNAKEPLAYER_H

#include <Arduino.h>
#include "DotMatrix3D.h"

#define SCALE 8	        //边长
#define MaxLength 512   //总长度

#define Up 6
#define Down 5
#define Left 4
#define Right 3
#define Front 2
#define Back 1

class SnakePlayer
{
public:

    struct Point
    {
        char x, y, z;
        Point(): x(0), y(0), z(0) {} 
        Point(char x, char y, char z): x(x), y(y), z(z) {}
        bool operator==(const Point &p) const {
            return (x==p.x && y==p.y && z==p.z);
        }
    };
    struct Food
    {
        Point pos;
        bool yes;
    };
    struct Snake
    {
        Point body[MaxLength];
        char len;
        char direction;
        bool life;
        void set(int rnk, const Point &pos) {
            body[rnk].x = pos.x;
            body[rnk].y = pos.y;
            body[rnk].z = pos.z;
        }
    };

    Snake snake;
    Food food;
    Point TailPoint;    //存放蛇尾巴节点的临时变量
    Point NextPoint;    //存放蛇下一次移动的位置
    DotMatrix3D* dm;
    int timer;

    void init(uint8_t* buf, DotMatrix3D* dm);
    void update(uint8_t* buf, DotMatrix3D* dm);
private:
    void getDirection(uint8_t* buf);
    char move();
    bool isLive();
    bool Auto_Direction1();
    bool EatFood(void);
    void GetNewFoodLoc(Point &p);
    void Auto_Direction2();
    // void Shine_Food();
};

#endif