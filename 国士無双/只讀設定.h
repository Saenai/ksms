
int CIR(int &x, int max)//在max範圍循環x++
{
	int temp = x;
	x++;
	if (x == max) x = 0;
	return temp;
}

struct _GAMEPARAMETER	//游戲難度設定參數
{
	int difficulty;
	int time_fade;
	int num_screennote;
	int time_hantei;
	int kijunritsu;
	bool isShowhud;
};

static _GAMEPARAMETER teki = {//臨時測試用
	0,
	220 * 8,//880
	1,
	220,
	1,
	1
};
/*
static _GAMEPARAMETER teki = {
	0,
	220 * 8*2,//880
	1,
	220,
	0,
	1
};
*/
static _GAMEPARAMETER hyo = {
	1,
	220*8,
	3,
	220,
	1,
	1
};
static _GAMEPARAMETER muzu = {
	2,
	200 * 8,//800
	4,
	150,
	2,
	1
};
static _GAMEPARAMETER kyoku = {
	3,
	200 * 8,
	4,
	150,
	4,
	0
};