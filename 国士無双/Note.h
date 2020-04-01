#pragma once
#define RAN(a,b) ((rand() % (b-a+1))+ a);
#define 風	13371	//風
#define 林	13372	//林
#define 火	13373	//火
#define 山	13374	//山
#define 上	1
#define 下	2
#define 左	3
#define 右	4
#include <string.h>
#include <stdio.h>
#include <time.h> //使用获取系统时间time()函数需要包含的头文件
#include <iostream>
using namespace std;
/*
每次渲染時，按照每個not的x,y,z進行逐幀渲染
判定游戲槼則時利用時間進行判定
*/

class Note
{
private:
	bool ext;//渲染時的存在判定
	int Z;//Zokusei，可取風/林/火/山
	int cr_dire;//起始方向，可取東/南/西/北
	int life;
	int life_nokori;
	int g_t;

public:
	Note();//bool EXT,int X,int Y,int z,int CR);//構造函數
	virtual ~Note();
	void Inti();
	bool isExt();

	void Set(int LIFE, int g_Now);
	void Dest();
	void SetLifeMinus(int LIFE);
	void SetGT(int T);
	int GetZ();
	int GetLife();
	int GetDire();
	int GetGT();
	int GetNokori();

	int Ukei(int _In);//接受一個屬性攻擊時的判定函數，參數為數字代表的屬性
};

int Zokusei_Hantei(int N, int U)//N為此note，U為受到的攻擊
{
	switch (U - N)
	{
	case 0://相差為0，無剋制
	case 1://相差為1，剋制
		return 1;
	case -1://相差為-1，被剋制
		return -1;
	case 2:
		return 2;
		break;//相差為2，無關屬性
	case -2:
		return 2;
		break;//相差為2，無關屬性
	case 3:
		return -1;//相差為3，被剋制
	case -3:
		return 1;//相差為-3，剋制
	}
	//return:0無剋制、1剋制、-1被剋制、2無關屬性
}

Note::Note()//bool EXT, int X, int Y, int z, int CR)
{
	//ext = EXT; x = X; y = Y; Z = z; cr_dire = CR;
}

Note::~Note()
{

}

void Note::Inti()
{
	ext = false;
	Z = -1; 
	cr_dire = -1;
	life = -1;
	life_nokori = -1;
	g_t = 0;
}

int Note::Ukei(int _In)
{
	if (Zokusei_Hantei(Z, _In) == 1) {
		return 1;
	}
	return 0;
}

void Note::Set(int LIFE,int g_Now)
{
	ext = true;
	Z = RAN(風, 山);
	cr_dire = RAN(上, 右);
	life = LIFE;
	life_nokori = LIFE;
	g_t = g_Now;
}
inline void Note::Dest()
{
	ext = 0;
}
bool Note::isExt()
{
	return ext;
}
void Note::SetLifeMinus(int LIFE)//設置殘餘生命
{
	if (LIFE>=1) life_nokori = LIFE;
	else 
	{
		ext = false;
	}
}
int Note::GetZ()
{
	return Z;
}
int Note::GetLife()
{
	return life;
}
int Note::GetNokori()
{
	return life_nokori;
}
int Note::GetDire()
{
	return cr_dire;
}
int Note::GetGT()
{
	return g_t;
}