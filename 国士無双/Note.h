#pragma once
#define RAN(a,b) ((rand() % (b-a+1))+ a);
#define �L	13371	//�L
#define ��	13372	//��
#define ��	13373	//��
#define ɽ	13374	//ɽ
#define ��	1
#define ��	2
#define ��	3
#define ��	4
#include <string.h>
#include <stdio.h>
#include <time.h> //ʹ�û�ȡϵͳʱ��time()������Ҫ������ͷ�ļ�
#include <iostream>
using namespace std;
/*
ÿ����Ⱦ�r������ÿ��not��x,y,z�M��������Ⱦ
�ж��Α򘳄t�r���Õr�g�M���ж�
*/

class Note
{
private:
	bool ext;//��Ⱦ�r�Ĵ����ж�
	int Z;//Zokusei����ȡ�L/��/��/ɽ
	int cr_dire;//��ʼ���򣬿�ȡ�|/��/��/��
	int life;
	int life_nokori;
	int g_t;

public:
	Note();//bool EXT,int X,int Y,int z,int CR);//���캯��
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

	int Ukei(int _In);//����һ�����Թ����r���ж������������锵�ִ���Č���
};

int Zokusei_Hantei(int N, int U)//N���note��U���ܵ��Ĺ���
{
	switch (U - N)
	{
	case 0://����0���o�w��
	case 1://����1���w��
		return 1;
	case -1://����-1�����w��
		return -1;
	case 2:
		return 2;
		break;//����2���o�P����
	case -2:
		return 2;
		break;//����2���o�P����
	case 3:
		return -1;//����3�����w��
	case -3:
		return 1;//����-3���w��
	}
	//return:0�o�w�ơ�1�w�ơ�-1���w�ơ�2�o�P����
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
	Z = RAN(�L, ɽ);
	cr_dire = RAN(��, ��);
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
void Note::SetLifeMinus(int LIFE)//�O�Ú��N����
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