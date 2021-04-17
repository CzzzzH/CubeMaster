
#include "SnakePlayer.h"

// 初始化: 设置蛇起始位置与长度 输出到dm
void SnakePlayer::init(uint8_t* buf, DotMatrix3D* dm)
{
	index[0] = 1;	// left/right
    index[1] = 5;	// front/back
    index[2] = 3;	// up/down

	for (int i=0; i<3; i++) {
		initMove[i] = buf[index[i]] > 0 ? int(buf[index[i] + 1]) : -int(buf[index[i] + 1]);
	}
	
	food.pos = Point(3,3,3);
	food.yes = 1;
	snake.set(0, Point(1,0,0));
	snake.set(1, Point(2,0,0));
	snake.set(2, Point(3,0,0));
	snake.len = 3;
	snake.life = true;
	snake.direction = Up;
	this->dm = dm;

	for(int i=0; i<snake.len; i++)
		dm->setDot(snake.body[i].x, snake.body[i].y, snake.body[i].z, true);

	dm->setDot(food.pos.x, food.pos.y, food.pos.z);
	timer = 0;
}

void SnakePlayer::update(uint8_t* buf)
{
	delay(10);
	timer += 10;
	if (timer % 100 == 0) {
		Shine_Food();
		getDirection(buf);
		// Auto_Direction1();
	}
	if (timer >= 800) {
		if (isLive()) {
			move();
			EatFood();
		}
		timer = 0;
	}
}

void SnakePlayer::getDirection(uint8_t* buf)
{
	// 这里读取摇杆方向
	// 以及按钮方向
	// 注意不可以往返方向走
	// if (Zigbee::getBuffer(buf)) {
		int currentMove[3];
		int diff[3];
		for (int i = 0; i < 3; i++)
		{
			currentMove[i] = buf[index[i]] > 0 ? int(buf[index[i] + 1]) : -int(buf[index[i] + 1]);
			diff[i] = currentMove[i] - initMove[i];
			if(diff[i] > 180) diff[i] -= 360;
			else if (diff[i] < -180) diff[i] += 360;
		}
		int target = -1;
		int value = 0;
		for (int i=0; i<3; i++) {
			if (abs(diff[i]) > 50 && abs(diff[i]) > abs(value)) {
				target = i;
				value = diff[i];
			}
		}
		if (target >= 0) {
			char new_direction; 
			switch (target) {
				case 0: {
					if (value > 0) new_direction = Left;
					else new_direction = Right; 
					break;
				}
				case 1: {
					if (value > 0) new_direction = Front;
					else new_direction = Back; 
					break;
				}
				case 2: {
					if (value > 0) new_direction = Up;
					else new_direction = Down; 
					break;
				}
			}
			if (new_direction != oppo(snake.direction)) {
				char old_direction = snake.direction;
				snake.direction = new_direction;
				if (!isLive()) snake.direction = old_direction;
				// Serial.write(int(new_direction));
				// Serial.write("hello");
			}
		}
		Serial.println(int(snake.direction));
	// }
}

// 依照目前方向 移动蛇 并输出到dm
char SnakePlayer::move()
{
	TailPoint = snake.body[0];	//保存蛇尾
	
  	for(int i=0; i<snake.len-1; i++)	//依次向尾部方向移动蛇各个节点的坐标
  	{
		snake.body[i] = snake.body[i+1];
	}	
	switch(snake.direction)
	{
		case Up:	{snake.body[snake.len-1].z += 1; break;}
		case Down:	{snake.body[snake.len-1].z -= 1; break;}
		case Left:	{snake.body[snake.len-1].x -= 1; break;}
		case Right:	{snake.body[snake.len-1].x += 1; break;}
		case Front:	{snake.body[snake.len-1].y += 1; break;}
		case Back:	{snake.body[snake.len-1].y -= 1; break;}
	}
	dm->setDot(TailPoint.x,TailPoint.y,TailPoint.z, false);
	dm->setDot(snake.body[snake.len-1].x, snake.body[snake.len-1].y, snake.body[snake.len-1].z, true);
	return 1;
}
 

bool SnakePlayer::isLive()
{
	//计算下一个点
	NextPoint = snake.body[snake.len-1];
	switch(snake.direction)
	{
		case Up:	{NextPoint.z += 1; break;}
		case Down:	{NextPoint.z -= 1; break;}
		case Left:	{NextPoint.x -= 1; break;}
		case Right:	{NextPoint.x += 1; break;}
		case Front:	{NextPoint.y += 1; break;}
		case Back:	{NextPoint.y -= 1; break;}
	}
	
	//判断蛇有没有撞到自身
	for(int i=0; i<=snake.len-1; i++)
  	{
		if(snake.body[i] == NextPoint)
		{
		   	snake.life = 0;
			return 0;
			break;
		}
	}
	//判断蛇有没有撞到墙
	if (	NextPoint.x >= 0 && NextPoint.x < SCALE
		&&	NextPoint.y >= 0 && NextPoint.y < SCALE
		&&	NextPoint.z >= 0 && NextPoint.z < SCALE)
		return 1;				
	else 
	{
		snake.life=0;
		return 0;
	}
}

// 自动往食物移动
bool SnakePlayer::Auto_Direction1()
{
	char olddir = snake.direction;	
	if(snake.body[snake.len-1].x < food.pos.x)
		snake.direction = Right;
	else if(snake.body[snake.len-1].x > food.pos.x)
		snake.direction = Left;
	else if(snake.body[snake.len-1].y <food.pos.y)
		snake.direction = Front;
	else if(snake.body[snake.len-1].y > food.pos.y)
		snake.direction = Back;
	else if(snake.body[snake.len-1].z > food.pos.z)
		snake.direction = Down;
	else if(snake.body[snake.len-1].z < food.pos.z)
		snake.direction = Up;		
	return 1;
}
 
//功能：吃食并且产生下一个实物的坐标
//返回值：1表示吃到食物，0表示未吃到食物
bool SnakePlayer::EatFood(void)
{
   	if(snake.body[snake.len-1] == food.pos)	//如果吃到食物
	{
		for(int i=snake.len; i>0; i--)  //依次向头部方向移动蛇各个节点的坐标
		{
			snake.body[i] = snake.body[i-1];
		}
		snake.body[0] = TailPoint;	//恢复尾巴坐标
		snake.len++;	 //节点个数加1
		//以下程序段为产生食物
		do {
			food.yes = 1;	//实物产生标志置1
			GetNewFoodLoc(food.pos);	//随机产生新的食物的坐标
			for(int i=0; i<snake.len-1; i++)
			{
				if(snake.body[i] == food.pos)	//判断食物出现的位置是否在蛇身上
				{
					food.yes = 0;	//食物产生标志置0
					break;
				}
		  	}
		} while (!food.yes);	//直到产生的食物不在蛇身上为止
		dm->setDot(food.pos.x, food.pos.y, food.pos.z, true);	//产生新的食物
		return 1;
	}
	return 0;
}
 
// 读取随机值来获得食物坐标，同时不能越界
void SnakePlayer::GetNewFoodLoc(Point &p)
{	
	p.x = random(0, SCALE);
	p.y = random(0, SCALE);
	p.z = random(0, SCALE);
}

// 随机移动
void SnakePlayer::Auto_Direction2()
{
	char olddir = snake.direction;	
	do {
		snake.direction = random() % 6 + 1;
	} while (snake.direction == olddir);
}

// 表示食物的LED灯闪烁
void SnakePlayer::Shine_Food()
{
	dm->setDot(food.pos.x, food.pos.y, food.pos.z, twinkle);
	twinkle = !twinkle;
}