#define _CRT_SECURE_NO_WARNINGS 1
#include<stdio.h>
#include<easyx.h>
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")

#define FONDSIZE 24
#define SIZE 60
#define ROW 10
#define COL 9

enum Pieces   //枚举棋子
{
	NONE = -1,
	車, 馬, 象, 士, 将, 砲, 卒,   //红方
	俥, 马, 相, 仕, 帥, 炮, 兵,   //黑方
	BEGIN , END
};
//给id赋值
enum Pieces redChess[] = { 車, 馬, 象, 士, 将, 砲, 卒 };
enum Pieces blackChess[] = { 俥, 马, 相, 仕, 帥, 炮, 兵 };

//绘制时转化成字符串
const char* ChessName[] = { "車", "馬", "象", "士", "将", "砲", "卒", "俥", "马", "相", "仕", "帥", "炮", "兵" };

//每一个棋子的属性
struct Chess
{
	enum Pieces id;  //棋子名称
	DWORD type;      //棋子类型  红？黑
	short x;
	short y;
	bool isRiver;         //是否过了河
};
//游戏地图
struct Chess map[ROW][COL];

//背景音乐
void BKBGM()
{
	mciSendString("open ./image/三消游戏-游戏内背景音乐.mp3 alias BGM", 0, 0, 0);
	mciSendString("open ./image/5390.mp3", 0, 0, 0);
	mciSendString("play BGM repeat", 0, 0, 0);
}

struct state {
	int begr;
	int begc;
	int endr;
	int endc;
	int state;
	DWORD color;

}state={-1,-1,-1,-1,BEGIN , RED};
//打印数组
void show()
{
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			printf("%2d ", map[i][j].id);
		}
		printf("\n");
	}
}
//初始化数据
void init()
{
	//遍历地图
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			map[i][j].id = NONE;
			if (i <= 4)  //黑棋
			{
				map[i][j].type = BLACK;
				if (i == 0)
				{
					//0 1 2 3 4
					if (j <= 4)
					{
						map[i][j].id = blackChess[j];
					}
					else
					{
						//3 2 1 0
						map[i][j].id = blackChess[COL - j - 1];
					}
				}
				if (i == 2 && (j == 1 || j == 7))
				{
					map[i][j].id = blackChess[5];
				}
				if (i == 3 && j % 2 == 0)
				{
					map[i][j].id = blackChess[6];
				}
			}
			else          //红棋
			{
				map[i][j].type = RED;
				if (i == ROW-1)
				{
					//0 1 2 3 4
					if (j <= 4)
					{
						map[i][j].id = redChess[j];
					}
					else
					{
						//3 2 1 0
						map[i][j].id = redChess[COL - j - 1];
					}
				}
				if (i == 7 && (j == 1 || j == 7))
				{
					map[i][j].id = redChess[5];
				
				}
				if (i == 6 && j % 2 == 0)
				{
					map[i][j].id = redChess[6];
				}
			}
			map[i][j].isRiver = false;
			map[i][j].x = (j + 1) * SIZE;
			map[i][j].y = (i + 1) * SIZE;
		}
	}
}

//绘制
void draw(IMAGE* img_board)
{

	//设置背景颜色
	setbkcolor(WHITE);
	cleardevice();
	//贴棋盘
	/*IMAGE img_board;
	loadimage(&img_board, "./image/chessboard.jpg", 8 * SIZE, 9 * SIZE);*/
	putimage(SIZE, SIZE, img_board);

	
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			if (map[i][j].id != NONE)
			{
				//绘制棋子
				setlinecolor(map[i][j].type);
				settextcolor(map[i][j].type);
				fillcircle(map[i][j].x, map[i][j].y, SIZE*2/5);
				fillcircle(map[i][j].x, map[i][j].y, SIZE/3);
				outtextxy(map[i][j].x -SIZE/4, map[i][j].y - SIZE / 4, ChessName[map[i][j].id]);
			}
			
		}
	}
}
void setred()
{
	map[state.endr][state.endc].id = map[state.begr][state.begc].id;
	map[state.endr][state.endc].isRiver = map[state.begr][state.begc].isRiver;
	map[state.endr][state.endc].type = map[state.begr][state.begc].type;
	if (state.endr < 5)
	{
		map[state.endr][state.endc].isRiver = true;
	}
	map[state.begr][state.begc].id = NONE;
	state.color = BLACK;
}
void setblack()
{
	map[state.endr][state.endc].id = map[state.begr][state.begc].id;
	map[state.endr][state.endc].isRiver = map[state.begr][state.begc].isRiver;
	map[state.endr][state.endc].type = map[state.begr][state.begc].type;
	if (state.endr > 4)
	{
		map[state.endr][state.endc].isRiver = true;
	}
	map[state.begr][state.begc].id = NONE;
	state.color = RED;
}

//移动棋子
void chessmove()
{
	if (state.state == BEGIN && (map[state.endr][state.endc].id == NONE ||
		map[state.endr][state.endc].type != map[state.begr][state.begc].type))
	{
		switch (map[state.begr][state.begc].id)
		{
			// 红色棋子
		case 車:
			if (state.begc == state.endc)
			{
				if (state.begr < state.endr)
				{
					for (int i = state.begr + 1; i < state.endr; i++)
					{
						if (map[i][state.begc].id != NONE)
						{
							return;
						}
					}
				}
				else {
					for (int i = state.endr + 1; i < state.begr; i++)
					{
						if (map[i][state.begc].id != NONE)
						{
							return;
						}
					}
				}
			}
			else if (state.begr == state.endr)
			{
				if (state.begc < state.endc)
				{
					for (int i = state.begc + 1; i < state.endc; i++)
					{
						if (map[state.begr][i].id != NONE)
						{
							return;
						}
					}
				}
				else {
					for (int i = state.endc + 1; i < state.begc; i++)
					{
						if (map[state.begr][i].id != NONE)
						{
							return;
						}
					}
				}
			}
			setred();
			break;
		case 馬:
			if ((state.begc - state.endc) * (state.begc - state.endc) + (state.begr - state.endr) * (state.begr - state.endr) != 5
				|| (abs(state.begr - state.endr) == 2 && map[(state.begr + state.endr) / 2][state.begc].id != NONE)
				|| (abs(state.begc - state.endc) == 2 && map[state.begr][(state.begc + state.endc) / 2].id != NONE))
			{
				return;
			}
			setred();
			break;
		case 象:
			if (state.endr < 5 || map[(state.begr + state.endr) / 2][(state.begc + state.endc) / 2].id != NONE
				|| (state.begc - state.endc) * (state.begc - state.endc) + (state.begr - state.endr) * (state.begr - state.endr) != 8)
			{
				return;
			}
			setred();
			break;
		case 士:
			if (state.endr<7 || state.endc<3 ||state.endc>5 
				|| (state.begc - state.endc) * (state.begc - state.endc) + (state.begr - state.endr) * (state.begr - state.endr) != 2)
			{
				return;
			}
			setred();
			break;
		case 将:
			if (state.endr < 7 || state.endc < 3 || state.endc>5
				|| (state.begc - state.endc) * (state.begc - state.endc) + (state.begr - state.endr) * (state.begr - state.endr) != 1)
			{
				return;
			}
			setred();
			break;
		case 砲:
			if (state.begc != state.endc && state.begr != state.endr)
			{
				return;
			}
			else if (state.begc == state.endc)
			{
				if (state.begr < state.endr)
				{
					int p = 0;
					for (int i = state.begr + 1; i < state.endr; i++)
					{
						if (map[i][state.begc].id != NONE)
						{
							p++;
						}
					}
					if ((p == 0 && map[state.endr][state.endc].id != NONE)
						|| p == 1 && map[state.endr][state.endc].id == NONE || p > 1)
					{
						return;
					}
				}
				else 
				{
					int p = 0;
					for (int i = state.endr + 1; i < state.begr; i++)
					{
						if (map[i][state.begc].id != NONE)
						{
							p++;
						}
					}
					if ((p == 0 && map[state.endr][state.endc].id != NONE)
						|| p == 1 && map[state.endr][state.endc].id == NONE || p > 1)
					{
						return;
					}
				}
			}
			else
			{
				if (state.begc < state.endc)
				{
					int p = 0;
					for (int i = state.begc + 1; i < state.endc; i++)
					{
						if (map[state.begr][i].id != NONE)
						{
							p++;
						}
					}
					if ((p == 0 && map[state.endr][state.endc].id != NONE)
						|| p == 1 && map[state.endr][state.endc].id == NONE || p > 1)
					{
						return;
					}
				}
				else
				{
					int p = 0;
					for (int i = state.endc + 1; i < state.begc; i++)
					{
						if (map[state.begr][i].id != NONE)
						{
							p++;
						}
					}
					if ((p == 0 && map[state.endr][state.endc].id != NONE)
						|| p == 1 && map[state.endr][state.endc].id == NONE || p > 1)
					{
						return;
					}
				}
			}
			setred();
			break;
		case 卒:
			if ((state.begc - state.endc) * (state.begc - state.endc) + (state.begr - state.endr) * (state.begr - state.endr) != 1
				|| state.endr > state.begr
				|| (map[state.begr][state.begc].isRiver == false && state.endc != state.begc))
			{
				return;
			}
			setred();
			break;

			// 黑色棋子
		case 俥:
			if (state.begc == state.endc)
			{
				if (state.begr < state.endr)
				{
					for (int i = state.begr + 1; i < state.endr; i++)
					{
						if (map[i][state.begc].id != NONE)
						{
							return;
						}
					}
				}
				else {
					for (int i = state.endr + 1; i < state.begr; i++)
					{
						if (map[i][state.begc].id != NONE)
						{
							return;
						}
					}
				}
			}
			else if (state.begr == state.endr)
			{
				if (state.begc < state.endc)
				{
					for (int i = state.begc + 1; i < state.endc; i++)
					{
						if (map[state.begr][i].id != NONE)
						{
							return;
						}
					}
				}
				else {
					for (int i = state.endc + 1; i < state.begc; i++)
					{
						if (map[state.begr][i].id != NONE)
						{
							return;
						}
					}
				}
			}
			setblack();
			break;
		case 马:
			if ((state.begc - state.endc) * (state.begc - state.endc) + (state.begr - state.endr) * (state.begr - state.endr) != 5
				|| (abs(state.begr - state.endr) == 2 && map[(state.begr + state.endr) / 2][state.begc].id != NONE)
				|| (abs(state.begc - state.endc) == 2 && map[state.begr][(state.begc + state.endc) / 2].id != NONE))
			{
				return;
			}
			setblack();
			break;
		case 相:
			if (state.endr > 4 || map[(state.begr + state.endr) / 2][(state.begc + state.endc) / 2].id != NONE
				|| (state.begc - state.endc) * (state.begc - state.endc) + (state.begr - state.endr) * (state.begr - state.endr) != 8)
			{
				return;
			}
			setblack();
			break;
		case 仕:
			if (state.endr > 2 || state.endc < 3 || state.endc>5
				|| (state.begc - state.endc) * (state.begc - state.endc) + (state.begr - state.endr) * (state.begr - state.endr) != 2)
			{
				return;
			}
			setblack();
			break;
		case 帥:
			if (state.endr > 2 || state.endc < 3 || state.endc>5
				|| (state.begc - state.endc)* (state.begc - state.endc) + (state.begr - state.endr)* (state.begr - state.endr) != 1)
			{
				return;
			}
			setblack();
			break;
		case 炮:
			if (state.begc != state.endc && state.begr != state.endr)
			{
				return;
			}
			else if (state.begc == state.endc)
			{
				if (state.begr < state.endr)
				{
					int p = 0;
					for (int i = state.begr + 1; i < state.endr; i++)
					{
						if (map[i][state.begc].id != NONE)
						{
							p++;
						}
					}
					if ((p == 0 && map[state.endr][state.endc].id != NONE)
						|| p == 1 && map[state.endr][state.endc].id == NONE || p > 1)
					{
						return;
					}
				}
				else
				{
					int p = 0;
					for (int i = state.endr + 1; i < state.begr; i++)
					{
						if (map[i][state.begc].id != NONE)
						{
							p++;
						}
					}
					if ((p == 0 && map[state.endr][state.endc].id != NONE)
						|| p == 1 && map[state.endr][state.endc].id == NONE || p > 1)
					{
						return;
					}
				}
			}
			else
			{
				if (state.begc < state.endc)
				{
					int p = 0;
					for (int i = state.begc + 1; i < state.endc; i++)
					{
						if (map[state.begr][i].id != NONE)
						{
							p++;
						}
					}
					if ((p == 0 && map[state.endr][state.endc].id != NONE)
						|| p == 1 && map[state.endr][state.endc].id == NONE || p > 1)
					{
						return;
					}
				}
				else
				{
					int p = 0;
					for (int i = state.endc + 1; i < state.begc; i++)
					{
						if (map[state.begr][i].id != NONE)
						{
							p++;
						}
					}
					if ((p == 0 && map[state.endr][state.endc].id != NONE)
						|| p == 1 && map[state.endr][state.endc].id == NONE || p > 1)
					{
						return;
					}
				}
			}
			setblack();
			break;
		case 兵:
			if ((state.begc - state.endc) * (state.begc - state.endc) + (state.begr - state.endr) * (state.begr - state.endr) != 1
				|| state.endr < state.begr
				|| (map[state.begr][state.begc].isRiver == false && state.endc != state.begc))
			{
				return;
			}
			setblack();
			break;
		}
	}
}

//鼠标操作
void mousehit()
{
	ExMessage msg;    //定义消息结构体变量
	if (peekmessage(&msg, EX_MOUSE))
	{
		if (msg.message == WM_LBUTTONDOWN)
		{
			//通过鼠标坐标得出数组的下标 i -> y   j->x
			int row = (msg.y + SIZE / 2) / SIZE - 1;
			int col = (msg.x + SIZE / 2) / SIZE - 1;
			printf("%d %d\n", row, col);
			if (row < 0 || row >= 10 || col < 0 || col >= 9)
			{
				return;
			}
			if (state.state == BEGIN)
			{
				if (map[row][col].id != NONE && map[row][col].type == state.color)
				{
					state.begr = row;
					state.begc = col;
					state.state = END;
					mciSendString("play ./image/5390.mp3", 0, 0, 0);
				}
				else
				{
					return;
				}
				
			}
			else if (state.state == END)
			{
				state.endr = row;
				state.endc = col;
				state.state = BEGIN;
				mciSendString("play ./image/5390.mp3", 0, 0, 0);
			}
			chessmove();
		}
	}
}

int redorblack()
{
	int flag = 0;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 3; j < 6; j++)
		{
			if (map[i][j].id == 帥)
			{
				flag = 1;
			}
		}
	}
	if (flag == 0) //红方胜
	{
		HWND hnd = GetHWnd();
		return MessageBox(hnd, "红棋获胜\n是否要继续游戏？\n 确定：继续   取消：退出", "提示：", MB_OKCANCEL);
	}
	for (int i = 7; i < 10; i++)
	{
		for (int j = 3; j < 6; j++)
		{
			if (map[i][j].id == 将)
			{
				flag = 2;
			}
		}
	}
	if (flag == 1) //黑方胜
	{
		HWND hnd = GetHWnd();
		return MessageBox(hnd, "黑棋获胜!\n是否要继续游戏？\n 确定：继续   取消：退出", "提示：", MB_OKCANCEL);
	}
	return -1;
}

int main()
{
	BKBGM();
	//创建图形窗口
	initgraph(10*SIZE, 11*SIZE );
	//设置背景模式
	setbkmode(TRANSPARENT);

	setfillcolor(WHITE);
	setlinestyle(PS_SOLID, 2);
	settextstyle(SIZE / 2, 0, "楷体");

	IMAGE img_board;
	loadimage(&img_board, "./image/chessboard.jpg", 8 * SIZE, 9 * SIZE);
	gamestart:
	//初始化数据
	init();
	int isok = 0;
	BeginBatchDraw();
	while (1)
	{
		draw(&img_board);
		isok = redorblack();
		if (isok == IDOK)
		{
			goto
				gamestart;
		}
		else if(isok==IDCANCEL){
			break;
		}
		mousehit();
		/*if (map[9][8].id == NONE)
		{
			show();
			break;
		}*/
		FlushBatchDraw();
	}
	EndBatchDraw();

	return 0;
}