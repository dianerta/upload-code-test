// 夜夜爱消除.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <graphics.h>
#include <conio.h>
#include <time.h>

typedef struct position
{
	int x;
	int y;
}posType;

#define KEY_DOWN(vk_c)(GetAsyncKeyState(vk_c)&0x8000)//按下那一段时间为1，其余时间为0

void init(void);//初始化界面
void gamebegain(void);//游戏开始界面
void gameplay();//玩儿游戏
void close();//释放资源（用EXE开发的时候，都要在程序的最后边加上两句函数getch();closegraph();）
void drawcursor(posType , COLORREF );//重绘位置
void drawtime(int );//重绘时间
void drawscore(int);//重绘分数
void getsamecolorballs(posType , COLORREF );//统计重复的颜色
int isValid(posType , COLORREF );//递归的结束条件
void ballsfall();
void turn();

COLORREF colorArr[6] = { RGB(200,0,0),RGB(0,200,0) ,RGB(0,0,200),
					RGB(200,200,0),RGB(200,0,200),RGB(0,200,200) };

posType cur;//定义全局变量当前光标所在位置
posType ballsArr[180];//同色小球的坐标
int index;//同色小球的个数

int main()
{
	init();
	gamebegain();
	gameplay();
	close();
	
    return 0;
}

void init()
{
	initgraph(1000, 700);//
}

void gamebegain()
{
	//绘制边框(坐标的计算方法是依据小球的半径20和个数12*15和居中（左上角为坐标原点）推出来的)
	setlinecolor(RGB(50, 50, 50));
	setlinestyle(PS_SOLID,10);//函数中有默认参数的可以不传参，因此后两个参数不写
	rectangle(255,45,745,655);//由于设置了线宽，内部实际范围变小，因此将(260,50,740,650）扩大，保证内部实际大小不变
	//绘制小球
	setlinestyle(PS_SOLID);
	srand((unsigned)time(NULL));
	for (int x = 280; x <= 740; x += 40)
	{
		for (int y = 70; y < 650; y += 40)
		{
			COLORREF c1=colorArr[rand() % 6];//实现随机数0-5，上面的用来初始化，这个用来生成随机数
			setlinecolor(c1);
			setfillcolor(c1);
			fillcircle(x, y, 18);
		}
	}
	//绘制光标
	cur.x = 480;
	cur.y = 390;
	drawcursor(cur, RGB(255, 255, 255));
	//绘制时间
	//drawtime(30);
	//绘制分数
	drawscore(0);
}

void gameplay()
{
	int score=0;
	for (int i = 50; i>-1;i--)//hold住,1s循环十次
	{
		if (i % 10 == 0)
		{
			drawtime(i / 10);
		}
		if (KEY_DOWN(VK_UP)&&cur.y>70)//防止出去框，边界限定
		{
			drawcursor(cur, RGB(0,0,0));
			cur.y -= 40;
			drawcursor(cur, RGB(255, 255, 255));
		}
		else if (KEY_DOWN(VK_DOWN) && cur.y<630)
		{
			drawcursor(cur, RGB(0, 0, 0));
			cur.y += 40;
			drawcursor(cur, RGB(255, 255, 255));
		}		
		else if (KEY_DOWN(VK_LEFT) && cur.x>280)
		{
			drawcursor(cur, RGB(0, 0, 0));
			cur.x -= 40;
			drawcursor(cur, RGB(255, 255, 255));
		}
		else if (KEY_DOWN(VK_RIGHT) && cur.x<720)
		{
			drawcursor(cur, RGB(0, 0, 0));
			cur.x += 40;
			drawcursor(cur, RGB(255, 255, 255));
		}
		else if (KEY_DOWN(VK_RETURN) || KEY_DOWN(VK_SPACE))
		{
			//获得光标所在位置周围的同色小球坐标，存入数组
			getsamecolorballs(cur,getpixel(cur.x,cur.y));

			if (index > 1)
			{
				//将数组的元素依次置黑一段时间//将数组的元素依次置黑一段时间
				for (int k = 0; k < index; k++)
				{
					setlinecolor(RGB(0, 0, 0));
					setfillcolor(RGB(0, 0, 0));
					fillcircle(ballsArr[k].x, ballsArr[k].y, 18);
				}
				Sleep(500);//看到下落的过程
				//小球下落
				ballsfall();
				//刷新分数
				score += index;
				drawscore(score);
			}
			index = 0;
		}
		Sleep(100);//延迟,单位ms，协调循环与同步
	}
	//结束
	cleardevice();
	settextstyle(80, 0, _T("黑体"));
	settextcolor(RGB(255, 0, 0));
	TCHAR s[30];
	_stprintf_s(s, _T("GAME OVER！"));
	outtextxy(400, 330, s);
	drawscore(score);
}

void close()
{
	_getch();
	closegraph();
}

void drawcursor(posType pos, COLORREF c1)
{
	setlinecolor(c1);
	rectangle(pos.x-20, pos.y-20,pos.x + 20, pos.y + 20);
}

void drawtime(int sec)
{
	settextstyle(25, 0, _T("黑体"));
	settextcolor(RGB(255, 255, 0));
	TCHAR s[30];
	_stprintf_s(s, _T("剩余时间：%2d s"), sec);    
	outtextxy(20, 50, s);
}

void drawscore(int score)
{
	settextstyle(25, 0, _T("黑体"));
	settextcolor(RGB(255, 0, 0));
	TCHAR sco[30];
	_stprintf_s(sco, _T("分数：%d "), score);
	outtextxy(30, 600, sco);
}

void getsamecolorballs(posType cur, COLORREF c1)//记录了周围相同颜色的数量以及他们的位置
{
	ballsArr[index].x = cur.x;
	ballsArr[index].y = cur.y;
	index++;

	posType temPos;
	for (int k = 0; k < 4; k++)
	{
		switch (k)
		{
			case 0:temPos.x = cur.x; temPos.y = cur.y - 40; break;
			case 1:temPos.x = cur.x; temPos.y = cur.y + 40; break;
			case 2:temPos.x = cur.x - 40; temPos.y = cur.y; break;
			case 3:temPos.x = cur.x + 40; temPos.y = cur.y; break;
		}

		if (isValid(temPos, c1))
		{
			getsamecolorballs(temPos, c1);
		}
	}
}

int isValid(posType cur, COLORREF c1)
{
	if (getpixel(cur.x, cur.y) != c1)
	{
		return 0;
	}
	else
	{
		for (int i = 0; i < index; i++)
		{
			if (cur.x == ballsArr[i].x&&cur.y == ballsArr[i].y)//颜色相等也不行，必须满足不是重复的
			{
				return 0;
			}
		}
		return 1;
	}
}

void turn()
{
	int i, j;
	posType temp;
	for(j=0;j<index-1;j++)
	{ 
		for (i = 0; i < index - 1 - j; i++)
		{
			if (ballsArr[i].x > ballsArr[i + 1].x)
			{
				temp = ballsArr[i];
				ballsArr[i]=ballsArr[i+1];
				ballsArr[i+1] = temp;
			}
			if(ballsArr[i].y > ballsArr[i + 1].y)
			{
				temp = ballsArr[i];
				ballsArr[i] = ballsArr[i + 1];
				ballsArr[i + 1] = temp;
			}
		}

	}
}

void ballsfall()
{
	turn();
	for (int i = 0; i < index; i++)
	{
		for (int k = ballsArr[i].y; k > 70; k -= 40)
		{
			COLORREF c1 = getpixel(ballsArr[i].x,k-40);
			setlinecolor(c1);
			setfillcolor(c1);
			fillcircle(ballsArr[i].x, k, 18);
		}
		COLORREF c1 = colorArr[rand() % 6];//给顶部的黑色一个随机颜色
		setlinecolor(c1);
		setfillcolor(c1);
		fillcircle(ballsArr[i].x, 70, 18);
	}
}
