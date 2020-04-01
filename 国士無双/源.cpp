/***********头文件**********/
#include <windows.h>
#include "resource.h"
#include "Note.h"
#include "只讀設定.h"
#pragma comment(lib,"winmm.lib")			//调用PlaySound函数所需库文件
#pragma comment(lib,"Msimg32.lib")		//添加使用TransparentBlt函数所需的库文件

/***********调试控制台**********/
/*
char console[64];
sprintf_s(console,"");
MessageBox(hwnd, console, "Console", 0);
*/
/***********WIN32设定**********/
#define WINDOW_WIDTH	1280
#define WINDOW_HEIGHT	720
#define WINDOW_TITLE	"国士無双"
#define WINDOW_FRAMEFREQ	30

/***********结构性全局变量**********/
HDC	sys_hdc = NULL, sys_mdc = NULL, sys_bufdc = NULL;
RECT	sys_rect;				//定义一个RECT结构体，用于储存内部窗口区域的坐标
SIZE windowsize;	//便于操作的窗口尺寸
int	sys_iFrameNum, sys_iTxtNum;  //帧数和文字数量
DWORD	g_tPre = 0, g_tNow = 0;					//声明l两个函数来记录时间,g_tPre记录上一次绘图的时间，g_tNow记录此次准备绘图的时间
char szWinName[] = "Kokushimusou";
HINSTANCE hInst;

#define ID_TIMER1  100			// 定义计时器标识符的值
/***********功能性全局变量**********/
int _GAMESTAGE = 0;
#define STAGE_MAIN 0
#define STAGE_DIFFICULTY 1
#define STAGE_GAMING 2
#define STAGE_HELP 3
int _GAMEDIFFICULTY = -1;
HFONT hFont, hFont_Title, hFont_Note, hFont_Mid;
/*****STAGE_MAIN*****/
HBITMAP hBACKGROUND;
SIZE tLct;//按钮起始点定位
RECT tRect;//按钮区域
/*****STAGE_DIFFICULTY*****/
SIZE tLct1;//按钮起始点定位
RECT tRect1;//按钮区域
/*****STAGE_GAMING*****/
Note note[9];
Note* Proc;
_GAMEPARAMETER _GAMEDIFFPARAM;
int ctrl_direction = -1;
int ctrl_zokusei = 風;
int ga_Pre = 0, ga_Now = 0;
int circulation = 0;
int pixel_hantei = 0;

int score = 0;
int hp = 13;
int combo = 0;
int maxcombo = 0;

/***********函数声明**********/
LRESULT CALLBACK WindowFunc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgFunc_ABOUT(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgFunc_GAMEOVER(HWND, UINT, WPARAM, LPARAM);
BOOL sys_Intialize(HWND hwnd);
int sys_Dispatch(HWND hwnd);
BOOL sys_Destroy(HWND hwnd);
/***********页面函数声明**********/
int sys_Main(HWND hwnd);
int sys_Difficulty(HWND hwnd);
int sys_Gaming(HWND hwnd);
int sys_Help(HWND hwnd);
#define SN_GAME (WM_APP + 0)	//自定义信息Message
/***********游戲邏輯函数声明**********/
int game_Logic(int Dir, HWND hwnd);
int game_DrawNotes(HWND hwnd);
int game_Logic_fail(Note* proc);
int game_Logic_success(Note* proc);
int game_Score();

/***********实装用游戏设定**********/


int FRESH(HWND hwnd)
{
	g_tNow = GetTickCount64();   //获取当前系统时间
	if (g_tNow - g_tPre >= (1000 / WINDOW_FRAMEFREQ))        //当此次循环运行与上次绘图时间相差0.06秒时再进行重绘操作
	{
		sys_Dispatch(hwnd);
	}
	return 0;
}

/***********WinMain**********/
int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst,
	LPSTR lpszArgs, int nWinMode)
{
	HWND hwnd;
	MSG msg = { 0 };
	WNDCLASSEX wcl;
	HACCEL hAccel;

	wcl.cbSize = sizeof(WNDCLASSEX);
	wcl.hInstance = hThisInst;
	wcl.lpszClassName = szWinName;
	wcl.lpfnWndProc = WindowFunc;
	wcl.style = 0;
	wcl.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON2));//	自定义图标
	wcl.hIconSm = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON2));//(HICON)LoadImage(NULL, MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON, 32, 32, 0);//	自定义图标
	wcl.hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));//	自定义光标
	wcl.lpszMenuName = TEXT("MAINMENU");
	wcl.cbClsExtra = 0;
	wcl.cbWndExtra = 0;
	wcl.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	hInst = hThisInst;
	hAccel = LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_GAMEPLAY));//	快捷键
	if (!RegisterClassEx(&wcl)) return 0;
	RECT WINDOW;
	SetRect(&WINDOW, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	AdjustWindowRect(&WINDOW, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 1);//计算以客户区大小为基准的窗口大小
	hwnd = CreateWindow(
		szWinName,
		WINDOW_TITLE,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,//	不可变化的窗口大小
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		(WINDOW.right - WINDOW.left),
		(WINDOW.bottom - WINDOW.top),
		HWND_DESKTOP,
		NULL,
		hThisInst,
		NULL
	);
	ShowWindow(hwnd, nWinMode);
	UpdateWindow(hwnd);
	if (!sys_Intialize(hwnd))
	{
		MessageBox(hwnd, "资源初始化失败", "消息窗口", 0);
		return FALSE;
	}
	while (msg.message != WM_QUIT)		//使用while循环，如果消息不是WM_QUIT消息，就继续循环
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE) && !TranslateAccelerator(hwnd, hAccel, &msg))   //查看应用程序消息队列，有消息时将队列中的消息派发出去。
		{
			TranslateMessage(&msg);		//将虚拟键消息转换为字符消息
			DispatchMessage(&msg);			//分发一个消息给窗口程序。
		}
		FRESH(hwnd);

	}
	return msg.wParam;
}

LRESULT CALLBACK WindowFunc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		break;
	case WM_LBUTTONDOWN: {
		int x = LOWORD(lParam); int y = HIWORD(lParam);
		if (_GAMESTAGE == STAGE_MAIN) {
			if (x > tRect.left && x < tRect.right && y > tRect.top && y < tRect.bottom) {
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_OPERATE_GAMESTART, 0), NULL);
			}
			if (x > tRect.left && x < tRect.right && y > tRect.top + 60 && y < tRect.bottom + 60) {
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_HELP, 0), NULL);
			}
			if (x > tRect.left && x < tRect.right && y > tRect.top + 120 && y < tRect.bottom + 120) {
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_OPERATE_EXIT, 0), NULL);
			}
			break;
		}

		if (_GAMESTAGE == STAGE_DIFFICULTY) {
			if (x > tRect1.left && x < tRect1.right && y > tRect1.top && y < tRect1.bottom) {
				_GAMEDIFFICULTY = 0;
				PostMessage(hwnd, SN_GAME, wParam, lParam);
			}
			if (x > tRect1.left + 140 && x < tRect1.right + 140 && y > tRect1.top && y < tRect1.bottom) {
				_GAMEDIFFICULTY = 1;
				PostMessage(hwnd, SN_GAME, wParam, lParam);
			}
			if (x > tRect1.left && x < tRect1.right && y > tRect1.top + 140 && y < tRect1.bottom + 140) {
				_GAMEDIFFICULTY = 2;
				PostMessage(hwnd, SN_GAME, wParam, lParam);
			}
			if (x > tRect1.left + 140 && x < tRect1.right + 140 && y > tRect1.top + 140 && y < tRect1.bottom + 140) {
				_GAMEDIFFICULTY = 3;
				PostMessage(hwnd, SN_GAME, wParam, lParam);
			}
			break;
		}
	}
	case SN_GAME:
		//在這裏初期化新一局游戲數據
		_GAMESTAGE = STAGE_GAMING;
		Proc = NULL;
		score = 0;
		hp = 13;
		combo = 0;
		maxcombo = 0;
		for (int i = 0; i < 9; i++)
		{
			note[i].Inti();
		}
		switch (_GAMEDIFFICULTY)
		{
		case 0:
			_GAMEDIFFPARAM = teki;
			break;
		case 1:
			_GAMEDIFFPARAM = hyo;
			break;
		case 2:
			_GAMEDIFFPARAM = muzu;
			break;
		case 3:
			_GAMEDIFFPARAM = kyoku;
			break;
		default:
			break;
		}
		break;
	case WM_DESTROY:
		sys_Destroy(hwnd);
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_OPERATE_GAMESTART:
			_GAMESTAGE = STAGE_DIFFICULTY;
			break;
		case ID_OPERATE_EXIT:
			PostMessage(hwnd, WM_QUIT, wParam, lParam);
			break;
		case ID_HELP:
			_GAMESTAGE = STAGE_HELP;
			break;
		case ID_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_ABOUT), hwnd, DlgFunc_ABOUT);
			break;

		case ID_W:
			if (_GAMESTAGE == STAGE_GAMING)
			{
				ctrl_direction = 上;
				game_Logic(上, hwnd);
			}
			break;
		case ID_S:
			if (_GAMESTAGE == STAGE_GAMING)
			{
				ctrl_direction = 下;
				game_Logic(下, hwnd);
			}
			break;
		case ID_A:
			if (_GAMESTAGE == STAGE_GAMING)
			{
				ctrl_direction = 左;
				game_Logic(左, hwnd);
			}
			break;
		case ID_D:
			if (_GAMESTAGE == STAGE_GAMING)
			{
				ctrl_direction = 右;
				game_Logic(右, hwnd);
			}
			break;
		case ID_UP:
			if (_GAMESTAGE == STAGE_GAMING)
			{
				ctrl_zokusei = 風;
			}
			break;
		case ID_DO:
			if (_GAMESTAGE == STAGE_GAMING)
			{
				ctrl_zokusei = 林;
			}
			break;
		case ID_LE:
			if (_GAMESTAGE == STAGE_GAMING)
			{
				ctrl_zokusei = 火;
			}
			break;
		case ID_RI:
			if (_GAMESTAGE == STAGE_GAMING)
			{
				ctrl_zokusei = 山;
			}
			break;
		}
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

BOOL sys_Intialize(HWND hwnd)
{
	srand((unsigned)time(NULL));      //用系统时间初始化随机种子 
	HBITMAP bmp;

	//三缓冲体系的创建
	sys_hdc = GetDC(hwnd);
	sys_mdc = CreateCompatibleDC(sys_hdc);  //创建一个和hdc兼容的内存DC
	sys_bufdc = CreateCompatibleDC(sys_hdc);//再创建一个和hdc兼容的缓冲DC
	bmp = CreateCompatibleBitmap(sys_hdc, WINDOW_WIDTH, WINDOW_HEIGHT); //建一个和窗口兼容的空的位图对象

	//加载资源
	hBACKGROUND = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BACKGROUND));//(HBITMAP)LoadImage(NULL, MAKEINTRESOURCE(IDB_BACKGROUND), IMAGE_BITMAP, WINDOW_WIDTH, WINDOW_HEIGHT,0);

	SelectObject(sys_mdc, bmp);//将空位图对象放到mdc中

	GetClientRect(hwnd, &sys_rect);		//取得内部窗口区域的大小
	windowsize.cx = abs(sys_rect.right - sys_rect.left);//WINDOW_WIDTH
	windowsize.cy = abs(sys_rect.bottom - sys_rect.top);//WINDOW_HEIGHT
	//设置字体
	hFont = CreateFont(24, 0, 0, 0, 400, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, TEXT("微软雅黑"));
	hFont_Note = CreateFont(48, 0, 0, 0, 400, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, TEXT("宋体"));
	hFont_Title = CreateFont(72, 0, 0, 0, 700, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, TEXT("宋体"));
	hFont_Mid = CreateFont(72, 0, 0, 0, 700, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, TEXT("宋体"));
	SelectObject(sys_mdc, hFont);
	SetBkMode(sys_mdc, TRANSPARENT);    //设置文字显示背景透明

	SelectObject(sys_mdc, GetStockObject(NULL_PEN));

	/*****模块初始*****/
	/***MAIN***/
	tLct.cx = .5 * windowsize.cx; tLct.cy = .6 * windowsize.cy;
	SetRect(&tRect, tLct.cx - 100, tLct.cy - 20, tLct.cx + 100, tLct.cy + 20);
	/***DIFFICULTY***/
	tLct1.cx = .5 * windowsize.cx - 70; tLct1.cy = .6 * windowsize.cy - 70;
	SetRect(&tRect1, tLct1.cx - 50, tLct1.cy - 50, tLct1.cx + 50, tLct1.cy + 50);

	sys_Dispatch(hwnd);  //调用一次游戏Main函数
	return TRUE;
}

int sys_Dispatch(HWND hwnd)
{
	switch (_GAMESTAGE) {
	case STAGE_MAIN:
		sys_Main(hwnd);
		break;
	case STAGE_DIFFICULTY:
		sys_Difficulty(hwnd);
		break;
	case STAGE_GAMING:
		sys_Gaming(hwnd);
		break;
	case STAGE_HELP:
		sys_Help(hwnd);
		break;
	}
	g_tPre = GetTickCount64();
	return 0;
}

int sys_Main(HWND hwnd)
{
	static HBRUSH	hBrush;
	char str[64];
	hBrush = CreateSolidBrush(RGB(180, 210, 200));
	SelectObject(sys_mdc, hBrush);
	Rectangle(sys_mdc, -1, -1, windowsize.cx + 1, windowsize.cy + 1);

	SelectObject(sys_bufdc, hBACKGROUND);
	BitBlt(sys_mdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, sys_bufdc, 0, 0, SRCCOPY);

	//hBrush = CreateSolidBrush(RGB(35, 59, 108));
	/***标题***/
	sprintf_s(str, "国　士　無　双");
	SelectObject(sys_mdc, hFont_Title);
	SetTextColor(sys_mdc, RGB(0, 0, 0));
	SetTextAlign(sys_mdc, TA_CENTER | TA_BASELINE);
	TextOut(sys_mdc, 0.5 * windowsize.cx, 0.33 * windowsize.cy, str, strlen(str));
	/***按钮背景***/
	hBrush = CreateSolidBrush(RGB(254, 254, 254));
	SelectObject(sys_mdc, hBrush);
	Rectangle(sys_mdc, tRect.left, tRect.top, tRect.right, tRect.bottom);
	Rectangle(sys_mdc, tRect.left, tRect.top + 60, tRect.right, tRect.bottom + 60);
	Rectangle(sys_mdc, tRect.left, tRect.top + 120, tRect.right, tRect.bottom + 120);

	/***按钮文字***/
	SelectObject(sys_mdc, hFont);
	sprintf_s(str, "开 始 游 戏");
	TextOut(sys_mdc, tLct.cx, tLct.cy + 5, str, strlen(str));
	sprintf_s(str, "帮  　　 助");
	TextOut(sys_mdc, tLct.cx, tLct.cy + 60 + 5, str, strlen(str));
	sprintf_s(str, "结 束 游 戏");
	TextOut(sys_mdc, tLct.cx, tLct.cy + 120 + 5, str, strlen(str));

	//for (int i = 0; i < num_button; i++) ShowWindow(hwndButton[i], SW_SHOW);

	//将mdc中的全部内容贴到hdc中
	BitBlt(sys_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, sys_mdc, 0, 0, SRCCOPY);

	g_tPre = GetTickCount64();

	return 0;
}

int sys_Difficulty(HWND hwnd)
{
	static HBRUSH	hBrush;
	char str[64];
	hBrush = CreateSolidBrush(RGB(0, 130, 114));
	SelectObject(sys_mdc, hBrush);
	Rectangle(sys_mdc, -1, -1, windowsize.cx + 1, windowsize.cy + 1);

	/***标题***/
	sprintf_s(str, "难 度 设 定");
	SetTextColor(sys_mdc, RGB(0, 0, 0));
	SelectObject(sys_mdc, hFont_Title);
	SetTextAlign(sys_mdc, TA_CENTER | TA_BASELINE);
	TextOut(sys_mdc, 0.5 * windowsize.cx, 0.25 * windowsize.cy, str, strlen(str));

	/***按钮背景***/
	hBrush = CreateSolidBrush(RGB(254, 254, 254));
	SelectObject(sys_mdc, hBrush);
	Rectangle(sys_mdc, tRect1.left, tRect1.top, tRect1.right, tRect1.bottom);
	Rectangle(sys_mdc, tRect1.left + 140, tRect1.top, tRect1.right + 140, tRect1.bottom);
	Rectangle(sys_mdc, tRect1.left, tRect1.top + 140, tRect1.right, tRect1.bottom + 140);
	Rectangle(sys_mdc, tRect1.left + 140, tRect1.top + 140, tRect1.right + 140, tRect1.bottom + 140);

	/***按钮文字***/
	SelectObject(sys_mdc, hFont_Title);
	SetTextColor(sys_mdc, RGB(120, 194, 196));
	sprintf_s(str, "適");
	TextOut(sys_mdc, tLct1.cx, tLct1.cy + 25, str, strlen(str));
	SetTextColor(sys_mdc, RGB(217, 205, 144));
	sprintf_s(str, "標");
	TextOut(sys_mdc, tLct1.cx + 140, tLct1.cy + 25, str, strlen(str));
	SetTextColor(sys_mdc, RGB(191, 103, 102));
	sprintf_s(str, "難");
	TextOut(sys_mdc, tLct1.cx, tLct1.cy + 140 + 25, str, strlen(str));
	SetTextColor(sys_mdc, RGB(0, 0, 0));
	sprintf_s(str, "極");
	TextOut(sys_mdc, tLct1.cx + 140, tLct1.cy + 140 + 25, str, strlen(str));

	//将mdc中的全部内容贴到hdc中
	BitBlt(sys_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, sys_mdc, 0, 0, SRCCOPY);
	g_tPre = GetTickCount64();
	return 0;
}

int sys_Gaming(HWND hwnd)
{
	if (hp == 0)
	{
		DialogBox(hInst, MAKEINTRESOURCE(IDD_GAMEOVER), hwnd, DlgFunc_GAMEOVER);
	}

	static HBRUSH	hWhiteBrush;
	hWhiteBrush = CreateSolidBrush(RGB(254, 254, 254));
	SelectObject(sys_mdc, hWhiteBrush);
	Rectangle(sys_mdc, -1, -1, windowsize.cx + 1, windowsize.cy + 1);
	char str[64];

	ga_Now = GetTickCount64();
	if (ga_Now - ga_Pre > (_GAMEDIFFPARAM.time_fade / _GAMEDIFFPARAM.num_screennote))
	{
		int temp = CIR(circulation, _GAMEDIFFPARAM.num_screennote);
		note[temp].Inti();
		note[temp].Set(_GAMEDIFFPARAM.time_fade, ga_Now);
		ga_Pre = ga_Now;
	}

	for (int i = 0; i < _GAMEDIFFPARAM.num_screennote; i++)//更新note壽命
	{
		if (note[i].isExt() == true)
		{
			note[i].SetLifeMinus(note[i].GetLife() - (ga_Now - note[i].GetGT()));
			if (note[i].GetNokori() <= 50) game_Logic_fail(&note[i]);
		}
	}

	for (int i = 0; i < _GAMEDIFFPARAM.num_screennote; i++)//更新待判定note
	{
		if (note[i].isExt() == true)
		{
			if (Proc == NULL) Proc = &note[i];
			else if (Proc != NULL)
			{
				if (Proc->GetNokori() > note[i].GetNokori()) Proc = &note[i];
			}
		}
	}

	hWhiteBrush = CreateSolidBrush(RGB(145, 152, 159));
	SelectObject(sys_mdc, hWhiteBrush);
	//繪製判定區域
	if (_GAMEDIFFICULTY <= 1) {
		Ellipse(sys_mdc, 0.5 * windowsize.cx - 43 - 18, 0.5 * windowsize.cy - 43 - 18, 0.5 * windowsize.cx + 43 + 18, 0.5 * windowsize.cy + 43 + 18);
	}
	if (_GAMEDIFFICULTY == 2) {
		Ellipse(sys_mdc, 0.5 * windowsize.cx - 33 - 18, 0.5 * windowsize.cy - 33 - 18, 0.5 * windowsize.cx + 33 + 18, 0.5 * windowsize.cy + 33 + 18);
	}

	game_DrawNotes(hwnd);



	SelectObject(sys_mdc, hFont_Mid);
	switch (ctrl_zokusei)
	{
	case 風:
		SetTextColor(sys_mdc, RGB(88, 178, 220));
		SetTextAlign(sys_mdc, TA_CENTER | 24);
		sprintf_s(str, "風");
		break;
	case 林:
		SetTextColor(sys_mdc, RGB(34, 125, 81));
		SetTextAlign(sys_mdc, TA_CENTER | 24);
		sprintf_s(str, "林");
		break;
	case 火:
		SetTextColor(sys_mdc, RGB(203, 27, 69));
		SetTextAlign(sys_mdc, TA_CENTER | 24);
		sprintf_s(str, "火");
		break;
	case 山:
		SetTextColor(sys_mdc, RGB(120, 120, 120));
		SetTextAlign(sys_mdc, TA_CENTER | 24);
		sprintf_s(str, "山");
		break;
	default:
		break;
	}
	TextOut(sys_mdc, 0.5 * windowsize.cx, 0.5 * windowsize.cy + 25, str, strlen(str));

	hWhiteBrush = CreateSolidBrush(RGB(0, 0, 0));
	SelectObject(sys_mdc, hWhiteBrush);
	Rectangle(sys_mdc, 0, 0, (WINDOW_WIDTH - WINDOW_HEIGHT) / 2, WINDOW_HEIGHT + 1);
	Rectangle(sys_mdc, (WINDOW_WIDTH + WINDOW_HEIGHT) / 2, 0, WINDOW_WIDTH + 1, WINDOW_HEIGHT + 1);

	//輸出分數等信息
	SelectObject(sys_mdc, hFont);
	SetTextColor(sys_mdc, RGB(255, 255, 255));
	SetTextAlign(sys_mdc, TA_LEFT | TA_BASELINE);
	sprintf_s(str, "[分數] %d", score);
	TextOut(sys_mdc, 32, 32, str, strlen(str));
	sprintf_s(str, "[體力] %d", hp);
	TextOut(sys_mdc, 32, 64, str, strlen(str));
	sprintf_s(str, "[連擊] %d", combo);
	TextOut(sys_mdc, 32, 96, str, strlen(str));
	//sprintf_s(str, "[連擊] %d", combo);
	//TextOut(sys_mdc, 32, 128, str, strlen(str));

	//将mdc中的全部内容贴到hdc中
	BitBlt(sys_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, sys_mdc, 0, 0, SRCCOPY);
	g_tPre = GetTickCount64();
	return 0;
}

int game_Logic(int Dir, HWND hwnd)//此處hwnd僅用作測試用 日後刪除
{
	//Proc-Globalstatus
	//時機
	//不對，直接進入懲罰，消滅note
	if (Proc->GetNokori() > _GAMEDIFFPARAM.time_hantei ||
		Proc->GetDire() != Dir)
	{
		game_Logic_fail(Proc);
	}
	else if (Proc->Ukei(ctrl_zokusei) == 0) game_Logic_fail(Proc);
	else if (Proc->Ukei(ctrl_zokusei) == 1)
	{
		game_Logic_success(Proc);
	}
	return 0;
}
int game_Logic_fail(Note * proc)
{
	combo = 0;
	hp--;
	proc->Dest();
	return 0;
}
int game_Logic_success(Note * proc)
{
	combo++;
	if (maxcombo < combo) maxcombo = combo;
	game_Score();
	proc->Dest();
	return 0;
}
int game_Score()
{
	score += 1 * combo * _GAMEDIFFPARAM.kijunritsu * hp;
	return 0;
}
int game_DrawNotes(HWND hwnd)
{
	char str[64];
	float _x = 0, _y = 0;
	float instance = 1.0;
	HPEN	hPen;
	for (int i = 0; i < _GAMEDIFFPARAM.num_screennote; i++)
	{
		if (note[i].isExt() == true)
		{
			instance = ((float)note[i].GetNokori() / (float)note[i].GetLife()) * WINDOW_HEIGHT * .5;

			/*if (note[i].GetNokori()<=_GAMEDIFFPARAM.time_hantei) {
				char console[64];
				sprintf_s(console, "%f",instance);
				MessageBox(hwnd, console, "Console", 0);
			}*/
			hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
			switch (note[i].GetZ())
			{
			case 風:
				SetTextColor(sys_mdc, RGB(88, 178, 220));
				if (_GAMEDIFFICULTY == 3) SetTextColor(sys_mdc, RGB(0, 0, 0));
				hPen = CreatePen(PS_SOLID, 1, RGB(88, 178, 220));
				SelectObject(sys_mdc, hFont_Note);
				SetTextAlign(sys_mdc, TA_CENTER | TA_BASELINE);
				sprintf_s(str, "風");
				break;
			case 林:
				SetTextColor(sys_mdc, RGB(34, 125, 81));
				if (_GAMEDIFFICULTY == 3) SetTextColor(sys_mdc, RGB(0, 0, 0));
				hPen = CreatePen(PS_SOLID, 1, RGB(34, 125, 81));
				SelectObject(sys_mdc, hFont_Note);
				SetTextAlign(sys_mdc, TA_CENTER | TA_BASELINE);
				sprintf_s(str, "林");
				break;
			case 火:
				SetTextColor(sys_mdc, RGB(203, 27, 69));
				if (_GAMEDIFFICULTY == 3) SetTextColor(sys_mdc, RGB(0, 0, 0));
				hPen = CreatePen(PS_SOLID, 1, RGB(203, 27, 69));
				SelectObject(sys_mdc, hFont_Note);
				SetTextAlign(sys_mdc, TA_CENTER | TA_BASELINE);
				sprintf_s(str, "火");
				break;
			case 山:
				SetTextColor(sys_mdc, RGB(120, 120, 120));
				if (_GAMEDIFFICULTY == 3) SetTextColor(sys_mdc, RGB(0, 0, 0));
				hPen = CreatePen(PS_SOLID, 1, RGB(120, 120, 120));
				SelectObject(sys_mdc, hFont_Note);
				SetTextAlign(sys_mdc, TA_CENTER | TA_BASELINE);
				sprintf_s(str, "山");
				break;
			default:
				break;
			}

			SelectObject(sys_mdc, hPen);
			//辅助判定圈
			//Arc(sys_mdc, .5 * WINDOW_WIDTH - instance, .5 * WINDOW_HEIGHT - instance, .5 * WINDOW_WIDTH + instance, .5 * WINDOW_HEIGHT + instance, .5 * WINDOW_WIDTH, .5 * WINDOW_HEIGHT - instance, .5 * WINDOW_WIDTH, .5 * WINDOW_HEIGHT - instance);
			SelectObject(sys_mdc, GetStockObject(NULL_PEN));

			switch (note[i].GetDire())
			{
			case 上:
				_x = .5 * WINDOW_WIDTH;
				_y = .5 * WINDOW_HEIGHT - instance + 15;
				break;
			case 下:
				_x = .5 * WINDOW_WIDTH;
				_y = .5 * WINDOW_HEIGHT + instance + 15;
				break;
			case 左:
				_x = .5 * WINDOW_WIDTH - instance;
				_y = .5 * WINDOW_HEIGHT + 15;
				break;
			case 右:
				_x = .5 * WINDOW_WIDTH + instance;
				_y = .5 * WINDOW_HEIGHT + 15;
				break;
			default:
				break;
			}
			TextOut(sys_mdc, _x, _y, str, strlen(str));
		}
	}
	return 0;
}

int sys_Help(HWND hwnd) {
	static HBRUSH	hWhiteBrush;
	hWhiteBrush = CreateSolidBrush(RGB(254, 223, 225));
	SelectObject(sys_mdc, hWhiteBrush);
	Rectangle(sys_mdc, -1, -1, windowsize.cx + 1, windowsize.cy + 1);

	char str[128];
	SetTextAlign(sys_mdc, TA_CENTER | TA_BASELINE);
	sprintf_s(str, "帮 　 　 助");
	SelectObject(sys_mdc, hFont_Title);
	SetTextColor(sys_mdc, RGB(0, 0, 0));
	TextOut(sys_mdc, 0.5 * windowsize.cx, 0.25 * windowsize.cy, str, strlen(str));

	SelectObject(sys_mdc, hFont);
	SetTextColor(sys_mdc, RGB(0, 0, 0));
	SetTextAlign(sys_mdc, TA_LEFT | TA_BASELINE);
	sprintf_s(str, "W S A D 分别对应上、下、左、右，按下时触发对最近一个note的判定；");
	TextOut(sys_mdc, 128, 256, str, strlen(str));
	sprintf_s(str, "↑ ↓ ← → 分别对应風、林、火、山四种属性；");
	TextOut(sys_mdc, 128, 256 + 32, str, strlen(str));
	sprintf_s(str, "属性间的克制关系：");
	TextOut(sys_mdc, 128, 256 + 32 * 2, str, strlen(str));
	sprintf_s(str, "　　風 剋 山、");
	TextOut(sys_mdc, 128, 256 + 32 * 3, str, strlen(str));
	sprintf_s(str, "　　山 剋 火、");
	TextOut(sys_mdc, 128, 256 + 32 * 4, str, strlen(str));
	sprintf_s(str, "　　火 剋 林、");
	TextOut(sys_mdc, 128, 256 + 32 * 5, str, strlen(str));
	sprintf_s(str, "　　林 剋 風。");
	TextOut(sys_mdc, 128, 256 + 32 * 6, str, strlen(str));
	sprintf_s(str, "关于难度（目前测试中、均低于设计难度）：");
	TextOut(sys_mdc, 128, 256 + 32 * 7, str, strlen(str));
	sprintf_s(str, "　　適-不计分（目前测试中、计分）；標-标准的游戏方式，适合绝大多数人；");
	TextOut(sys_mdc, 128, 256 + 32 * 8, str, strlen(str));
	sprintf_s(str, "　　難-对大多数人会造成困扰；極-在難的基础上、无颜色标识；");
	TextOut(sys_mdc, 128, 256 + 32 * 9, str, strlen(str));
	sprintf_s(str, "（暂时存在BUG，运行大约2min时会出现信息反馈错误，只能退出）");
	TextOut(sys_mdc, 128, 256 + 32 * 11, str, strlen(str));

	//将mdc中的全部内容贴到hdc中
	BitBlt(sys_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, sys_mdc, 0, 0, SRCCOPY);

	g_tPre = GetTickCount64();

	return 0;

}

BOOL sys_Destroy(HWND hwnd)
{
	//释放资源对象
	DeleteObject(hBACKGROUND);

	DeleteDC(sys_bufdc);
	DeleteDC(sys_mdc);
	ReleaseDC(hwnd, sys_hdc);
	return TRUE;
}

BOOL CALLBACK DlgFunc_ABOUT(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case 	WM_INITDIALOG:
		return TRUE;
	case 	WM_COMMAND:
		switch (LOWORD(wParam))//wParam的低位字段是按钮的ID
		{
		case 	IDOK:
		case 	IDCANCEL:
			EndDialog(hDlg, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

int num = 0;
BOOL CALLBACK DlgFunc_GAMEOVER(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	char str[64];
	switch (message)
	{
	case 	WM_INITDIALOG:
		/*GetClientRect(hDlg, &drect);		//取得窗口区域的大小
		dsize.cx = abs(drect.right - drect.left);
		dsize.cy = abs(drect.bottom - drect.top);*/
		SetTimer(hDlg, ID_TIMER1, 1000, NULL);
		num = 10;
		return TRUE;
	case WM_PAINT:
		hdc = BeginPaint(hDlg, &ps);
		SetBkMode(hdc, TRANSPARENT);    //设置文字显示背景透明
		SelectObject(hdc, hFont);
		SetTextAlign(hdc, TA_CENTER | TA_BASELINE);
		sprintf_s(str, "游戲結束");
		TextOut(hdc, 233, 45, str, strlen(str));
		sprintf_s(str, "分數：%d", score);
		TextOut(hdc, 233, 144 - 10, str, strlen(str));
		sprintf_s(str, "最大連擊：%d", maxcombo);
		TextOut(hdc, 233, 144 + 10, str, strlen(str));

		sprintf_s(str, "(%d)", num);
		TextOut(hdc, 233, 144 + 30, str, strlen(str));
		EndPaint(hDlg, &ps);
		break;
	case WM_TIMER:
		switch (wParam)
		{
		case ID_TIMER1:
			if (num <= 0)
			{
				_GAMESTAGE = STAGE_DIFFICULTY;
				EndDialog(hDlg, 0);
				KillTimer(hDlg, ID_TIMER1);
			}
			else
			{
				num--;
				InvalidateRect(hDlg, NULL, TRUE);
			}
			break;
		}
		break;
	case 	WM_COMMAND:
		switch (LOWORD(wParam))//wParam的低位字段是按钮的ID
		{
		case 	IDOK:
			_GAMESTAGE = STAGE_DIFFICULTY;
			EndDialog(hDlg, 0);
			KillTimer(hDlg, ID_TIMER1);
		}
		break;
	}
	return FALSE;
}