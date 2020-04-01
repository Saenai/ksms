/***********ͷ�ļ�**********/
#include <windows.h>
#include "resource.h"
#include "Note.h"
#include "ֻ�x�O��.h"
#pragma comment(lib,"winmm.lib")			//����PlaySound����������ļ�
#pragma comment(lib,"Msimg32.lib")		//���ʹ��TransparentBlt��������Ŀ��ļ�

/***********���Կ���̨**********/
/*
char console[64];
sprintf_s(console,"");
MessageBox(hwnd, console, "Console", 0);
*/
/***********WIN32�趨**********/
#define WINDOW_WIDTH	1280
#define WINDOW_HEIGHT	720
#define WINDOW_TITLE	"��ʿ�o˫"
#define WINDOW_FRAMEFREQ	30

/***********�ṹ��ȫ�ֱ���**********/
HDC	sys_hdc = NULL, sys_mdc = NULL, sys_bufdc = NULL;
RECT	sys_rect;				//����һ��RECT�ṹ�壬���ڴ����ڲ��������������
SIZE windowsize;	//���ڲ����Ĵ��ڳߴ�
int	sys_iFrameNum, sys_iTxtNum;  //֡������������
DWORD	g_tPre = 0, g_tNow = 0;					//����l������������¼ʱ��,g_tPre��¼��һ�λ�ͼ��ʱ�䣬g_tNow��¼�˴�׼����ͼ��ʱ��
char szWinName[] = "Kokushimusou";
HINSTANCE hInst;

#define ID_TIMER1  100			// �����ʱ����ʶ����ֵ
/***********������ȫ�ֱ���**********/
int _GAMESTAGE = 0;
#define STAGE_MAIN 0
#define STAGE_DIFFICULTY 1
#define STAGE_GAMING 2
#define STAGE_HELP 3
int _GAMEDIFFICULTY = -1;
HFONT hFont, hFont_Title, hFont_Note, hFont_Mid;
/*****STAGE_MAIN*****/
HBITMAP hBACKGROUND;
SIZE tLct;//��ť��ʼ�㶨λ
RECT tRect;//��ť����
/*****STAGE_DIFFICULTY*****/
SIZE tLct1;//��ť��ʼ�㶨λ
RECT tRect1;//��ť����
/*****STAGE_GAMING*****/
Note note[9];
Note* Proc;
_GAMEPARAMETER _GAMEDIFFPARAM;
int ctrl_direction = -1;
int ctrl_zokusei = �L;
int ga_Pre = 0, ga_Now = 0;
int circulation = 0;
int pixel_hantei = 0;

int score = 0;
int hp = 13;
int combo = 0;
int maxcombo = 0;

/***********��������**********/
LRESULT CALLBACK WindowFunc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgFunc_ABOUT(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgFunc_GAMEOVER(HWND, UINT, WPARAM, LPARAM);
BOOL sys_Intialize(HWND hwnd);
int sys_Dispatch(HWND hwnd);
BOOL sys_Destroy(HWND hwnd);
/***********ҳ�溯������**********/
int sys_Main(HWND hwnd);
int sys_Difficulty(HWND hwnd);
int sys_Gaming(HWND hwnd);
int sys_Help(HWND hwnd);
#define SN_GAME (WM_APP + 0)	//�Զ�����ϢMessage
/***********�Α�߉݋��������**********/
int game_Logic(int Dir, HWND hwnd);
int game_DrawNotes(HWND hwnd);
int game_Logic_fail(Note* proc);
int game_Logic_success(Note* proc);
int game_Score();

/***********ʵװ����Ϸ�趨**********/


int FRESH(HWND hwnd)
{
	g_tNow = GetTickCount64();   //��ȡ��ǰϵͳʱ��
	if (g_tNow - g_tPre >= (1000 / WINDOW_FRAMEFREQ))        //���˴�ѭ���������ϴλ�ͼʱ�����0.06��ʱ�ٽ����ػ����
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
	wcl.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON2));//	�Զ���ͼ��
	wcl.hIconSm = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON2));//(HICON)LoadImage(NULL, MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON, 32, 32, 0);//	�Զ���ͼ��
	wcl.hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));//	�Զ�����
	wcl.lpszMenuName = TEXT("MAINMENU");
	wcl.cbClsExtra = 0;
	wcl.cbWndExtra = 0;
	wcl.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	hInst = hThisInst;
	hAccel = LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_GAMEPLAY));//	��ݼ�
	if (!RegisterClassEx(&wcl)) return 0;
	RECT WINDOW;
	SetRect(&WINDOW, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	AdjustWindowRect(&WINDOW, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 1);//�����Կͻ�����СΪ��׼�Ĵ��ڴ�С
	hwnd = CreateWindow(
		szWinName,
		WINDOW_TITLE,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,//	���ɱ仯�Ĵ��ڴ�С
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
		MessageBox(hwnd, "��Դ��ʼ��ʧ��", "��Ϣ����", 0);
		return FALSE;
	}
	while (msg.message != WM_QUIT)		//ʹ��whileѭ���������Ϣ����WM_QUIT��Ϣ���ͼ���ѭ��
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE) && !TranslateAccelerator(hwnd, hAccel, &msg))   //�鿴Ӧ�ó�����Ϣ���У�����Ϣʱ�������е���Ϣ�ɷ���ȥ��
		{
			TranslateMessage(&msg);		//���������Ϣת��Ϊ�ַ���Ϣ
			DispatchMessage(&msg);			//�ַ�һ����Ϣ�����ڳ���
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
		//���@�Y���ڻ���һ���Α򔵓�
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
				ctrl_direction = ��;
				game_Logic(��, hwnd);
			}
			break;
		case ID_S:
			if (_GAMESTAGE == STAGE_GAMING)
			{
				ctrl_direction = ��;
				game_Logic(��, hwnd);
			}
			break;
		case ID_A:
			if (_GAMESTAGE == STAGE_GAMING)
			{
				ctrl_direction = ��;
				game_Logic(��, hwnd);
			}
			break;
		case ID_D:
			if (_GAMESTAGE == STAGE_GAMING)
			{
				ctrl_direction = ��;
				game_Logic(��, hwnd);
			}
			break;
		case ID_UP:
			if (_GAMESTAGE == STAGE_GAMING)
			{
				ctrl_zokusei = �L;
			}
			break;
		case ID_DO:
			if (_GAMESTAGE == STAGE_GAMING)
			{
				ctrl_zokusei = ��;
			}
			break;
		case ID_LE:
			if (_GAMESTAGE == STAGE_GAMING)
			{
				ctrl_zokusei = ��;
			}
			break;
		case ID_RI:
			if (_GAMESTAGE == STAGE_GAMING)
			{
				ctrl_zokusei = ɽ;
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
	srand((unsigned)time(NULL));      //��ϵͳʱ���ʼ��������� 
	HBITMAP bmp;

	//��������ϵ�Ĵ���
	sys_hdc = GetDC(hwnd);
	sys_mdc = CreateCompatibleDC(sys_hdc);  //����һ����hdc���ݵ��ڴ�DC
	sys_bufdc = CreateCompatibleDC(sys_hdc);//�ٴ���һ����hdc���ݵĻ���DC
	bmp = CreateCompatibleBitmap(sys_hdc, WINDOW_WIDTH, WINDOW_HEIGHT); //��һ���ʹ��ڼ��ݵĿյ�λͼ����

	//������Դ
	hBACKGROUND = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BACKGROUND));//(HBITMAP)LoadImage(NULL, MAKEINTRESOURCE(IDB_BACKGROUND), IMAGE_BITMAP, WINDOW_WIDTH, WINDOW_HEIGHT,0);

	SelectObject(sys_mdc, bmp);//����λͼ����ŵ�mdc��

	GetClientRect(hwnd, &sys_rect);		//ȡ���ڲ���������Ĵ�С
	windowsize.cx = abs(sys_rect.right - sys_rect.left);//WINDOW_WIDTH
	windowsize.cy = abs(sys_rect.bottom - sys_rect.top);//WINDOW_HEIGHT
	//��������
	hFont = CreateFont(24, 0, 0, 0, 400, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, TEXT("΢���ź�"));
	hFont_Note = CreateFont(48, 0, 0, 0, 400, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, TEXT("����"));
	hFont_Title = CreateFont(72, 0, 0, 0, 700, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, TEXT("����"));
	hFont_Mid = CreateFont(72, 0, 0, 0, 700, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, TEXT("����"));
	SelectObject(sys_mdc, hFont);
	SetBkMode(sys_mdc, TRANSPARENT);    //����������ʾ����͸��

	SelectObject(sys_mdc, GetStockObject(NULL_PEN));

	/*****ģ���ʼ*****/
	/***MAIN***/
	tLct.cx = .5 * windowsize.cx; tLct.cy = .6 * windowsize.cy;
	SetRect(&tRect, tLct.cx - 100, tLct.cy - 20, tLct.cx + 100, tLct.cy + 20);
	/***DIFFICULTY***/
	tLct1.cx = .5 * windowsize.cx - 70; tLct1.cy = .6 * windowsize.cy - 70;
	SetRect(&tRect1, tLct1.cx - 50, tLct1.cy - 50, tLct1.cx + 50, tLct1.cy + 50);

	sys_Dispatch(hwnd);  //����һ����ϷMain����
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
	/***����***/
	sprintf_s(str, "����ʿ���o��˫");
	SelectObject(sys_mdc, hFont_Title);
	SetTextColor(sys_mdc, RGB(0, 0, 0));
	SetTextAlign(sys_mdc, TA_CENTER | TA_BASELINE);
	TextOut(sys_mdc, 0.5 * windowsize.cx, 0.33 * windowsize.cy, str, strlen(str));
	/***��ť����***/
	hBrush = CreateSolidBrush(RGB(254, 254, 254));
	SelectObject(sys_mdc, hBrush);
	Rectangle(sys_mdc, tRect.left, tRect.top, tRect.right, tRect.bottom);
	Rectangle(sys_mdc, tRect.left, tRect.top + 60, tRect.right, tRect.bottom + 60);
	Rectangle(sys_mdc, tRect.left, tRect.top + 120, tRect.right, tRect.bottom + 120);

	/***��ť����***/
	SelectObject(sys_mdc, hFont);
	sprintf_s(str, "�� ʼ �� Ϸ");
	TextOut(sys_mdc, tLct.cx, tLct.cy + 5, str, strlen(str));
	sprintf_s(str, "��  ���� ��");
	TextOut(sys_mdc, tLct.cx, tLct.cy + 60 + 5, str, strlen(str));
	sprintf_s(str, "�� �� �� Ϸ");
	TextOut(sys_mdc, tLct.cx, tLct.cy + 120 + 5, str, strlen(str));

	//for (int i = 0; i < num_button; i++) ShowWindow(hwndButton[i], SW_SHOW);

	//��mdc�е�ȫ����������hdc��
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

	/***����***/
	sprintf_s(str, "�� �� �� ��");
	SetTextColor(sys_mdc, RGB(0, 0, 0));
	SelectObject(sys_mdc, hFont_Title);
	SetTextAlign(sys_mdc, TA_CENTER | TA_BASELINE);
	TextOut(sys_mdc, 0.5 * windowsize.cx, 0.25 * windowsize.cy, str, strlen(str));

	/***��ť����***/
	hBrush = CreateSolidBrush(RGB(254, 254, 254));
	SelectObject(sys_mdc, hBrush);
	Rectangle(sys_mdc, tRect1.left, tRect1.top, tRect1.right, tRect1.bottom);
	Rectangle(sys_mdc, tRect1.left + 140, tRect1.top, tRect1.right + 140, tRect1.bottom);
	Rectangle(sys_mdc, tRect1.left, tRect1.top + 140, tRect1.right, tRect1.bottom + 140);
	Rectangle(sys_mdc, tRect1.left + 140, tRect1.top + 140, tRect1.right + 140, tRect1.bottom + 140);

	/***��ť����***/
	SelectObject(sys_mdc, hFont_Title);
	SetTextColor(sys_mdc, RGB(120, 194, 196));
	sprintf_s(str, "�m");
	TextOut(sys_mdc, tLct1.cx, tLct1.cy + 25, str, strlen(str));
	SetTextColor(sys_mdc, RGB(217, 205, 144));
	sprintf_s(str, "��");
	TextOut(sys_mdc, tLct1.cx + 140, tLct1.cy + 25, str, strlen(str));
	SetTextColor(sys_mdc, RGB(191, 103, 102));
	sprintf_s(str, "�y");
	TextOut(sys_mdc, tLct1.cx, tLct1.cy + 140 + 25, str, strlen(str));
	SetTextColor(sys_mdc, RGB(0, 0, 0));
	sprintf_s(str, "�O");
	TextOut(sys_mdc, tLct1.cx + 140, tLct1.cy + 140 + 25, str, strlen(str));

	//��mdc�е�ȫ����������hdc��
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

	for (int i = 0; i < _GAMEDIFFPARAM.num_screennote; i++)//����note����
	{
		if (note[i].isExt() == true)
		{
			note[i].SetLifeMinus(note[i].GetLife() - (ga_Now - note[i].GetGT()));
			if (note[i].GetNokori() <= 50) game_Logic_fail(&note[i]);
		}
	}

	for (int i = 0; i < _GAMEDIFFPARAM.num_screennote; i++)//���´��ж�note
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
	//�L�u�ж��^��
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
	case �L:
		SetTextColor(sys_mdc, RGB(88, 178, 220));
		SetTextAlign(sys_mdc, TA_CENTER | 24);
		sprintf_s(str, "�L");
		break;
	case ��:
		SetTextColor(sys_mdc, RGB(34, 125, 81));
		SetTextAlign(sys_mdc, TA_CENTER | 24);
		sprintf_s(str, "��");
		break;
	case ��:
		SetTextColor(sys_mdc, RGB(203, 27, 69));
		SetTextAlign(sys_mdc, TA_CENTER | 24);
		sprintf_s(str, "��");
		break;
	case ɽ:
		SetTextColor(sys_mdc, RGB(120, 120, 120));
		SetTextAlign(sys_mdc, TA_CENTER | 24);
		sprintf_s(str, "ɽ");
		break;
	default:
		break;
	}
	TextOut(sys_mdc, 0.5 * windowsize.cx, 0.5 * windowsize.cy + 25, str, strlen(str));

	hWhiteBrush = CreateSolidBrush(RGB(0, 0, 0));
	SelectObject(sys_mdc, hWhiteBrush);
	Rectangle(sys_mdc, 0, 0, (WINDOW_WIDTH - WINDOW_HEIGHT) / 2, WINDOW_HEIGHT + 1);
	Rectangle(sys_mdc, (WINDOW_WIDTH + WINDOW_HEIGHT) / 2, 0, WINDOW_WIDTH + 1, WINDOW_HEIGHT + 1);

	//ݔ���֔�����Ϣ
	SelectObject(sys_mdc, hFont);
	SetTextColor(sys_mdc, RGB(255, 255, 255));
	SetTextAlign(sys_mdc, TA_LEFT | TA_BASELINE);
	sprintf_s(str, "[�֔�] %d", score);
	TextOut(sys_mdc, 32, 32, str, strlen(str));
	sprintf_s(str, "[�w��] %d", hp);
	TextOut(sys_mdc, 32, 64, str, strlen(str));
	sprintf_s(str, "[�B��] %d", combo);
	TextOut(sys_mdc, 32, 96, str, strlen(str));
	//sprintf_s(str, "[�B��] %d", combo);
	//TextOut(sys_mdc, 32, 128, str, strlen(str));

	//��mdc�е�ȫ����������hdc��
	BitBlt(sys_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, sys_mdc, 0, 0, SRCCOPY);
	g_tPre = GetTickCount64();
	return 0;
}

int game_Logic(int Dir, HWND hwnd)//��̎hwnd�H�����yԇ�� ����h��
{
	//Proc-Globalstatus
	//�r�C
	//������ֱ���M����P������note
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
			case �L:
				SetTextColor(sys_mdc, RGB(88, 178, 220));
				if (_GAMEDIFFICULTY == 3) SetTextColor(sys_mdc, RGB(0, 0, 0));
				hPen = CreatePen(PS_SOLID, 1, RGB(88, 178, 220));
				SelectObject(sys_mdc, hFont_Note);
				SetTextAlign(sys_mdc, TA_CENTER | TA_BASELINE);
				sprintf_s(str, "�L");
				break;
			case ��:
				SetTextColor(sys_mdc, RGB(34, 125, 81));
				if (_GAMEDIFFICULTY == 3) SetTextColor(sys_mdc, RGB(0, 0, 0));
				hPen = CreatePen(PS_SOLID, 1, RGB(34, 125, 81));
				SelectObject(sys_mdc, hFont_Note);
				SetTextAlign(sys_mdc, TA_CENTER | TA_BASELINE);
				sprintf_s(str, "��");
				break;
			case ��:
				SetTextColor(sys_mdc, RGB(203, 27, 69));
				if (_GAMEDIFFICULTY == 3) SetTextColor(sys_mdc, RGB(0, 0, 0));
				hPen = CreatePen(PS_SOLID, 1, RGB(203, 27, 69));
				SelectObject(sys_mdc, hFont_Note);
				SetTextAlign(sys_mdc, TA_CENTER | TA_BASELINE);
				sprintf_s(str, "��");
				break;
			case ɽ:
				SetTextColor(sys_mdc, RGB(120, 120, 120));
				if (_GAMEDIFFICULTY == 3) SetTextColor(sys_mdc, RGB(0, 0, 0));
				hPen = CreatePen(PS_SOLID, 1, RGB(120, 120, 120));
				SelectObject(sys_mdc, hFont_Note);
				SetTextAlign(sys_mdc, TA_CENTER | TA_BASELINE);
				sprintf_s(str, "ɽ");
				break;
			default:
				break;
			}

			SelectObject(sys_mdc, hPen);
			//�����ж�Ȧ
			//Arc(sys_mdc, .5 * WINDOW_WIDTH - instance, .5 * WINDOW_HEIGHT - instance, .5 * WINDOW_WIDTH + instance, .5 * WINDOW_HEIGHT + instance, .5 * WINDOW_WIDTH, .5 * WINDOW_HEIGHT - instance, .5 * WINDOW_WIDTH, .5 * WINDOW_HEIGHT - instance);
			SelectObject(sys_mdc, GetStockObject(NULL_PEN));

			switch (note[i].GetDire())
			{
			case ��:
				_x = .5 * WINDOW_WIDTH;
				_y = .5 * WINDOW_HEIGHT - instance + 15;
				break;
			case ��:
				_x = .5 * WINDOW_WIDTH;
				_y = .5 * WINDOW_HEIGHT + instance + 15;
				break;
			case ��:
				_x = .5 * WINDOW_WIDTH - instance;
				_y = .5 * WINDOW_HEIGHT + 15;
				break;
			case ��:
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
	sprintf_s(str, "�� �� �� ��");
	SelectObject(sys_mdc, hFont_Title);
	SetTextColor(sys_mdc, RGB(0, 0, 0));
	TextOut(sys_mdc, 0.5 * windowsize.cx, 0.25 * windowsize.cy, str, strlen(str));

	SelectObject(sys_mdc, hFont);
	SetTextColor(sys_mdc, RGB(0, 0, 0));
	SetTextAlign(sys_mdc, TA_LEFT | TA_BASELINE);
	sprintf_s(str, "W S A D �ֱ��Ӧ�ϡ��¡����ң�����ʱ���������һ��note���ж���");
	TextOut(sys_mdc, 128, 256, str, strlen(str));
	sprintf_s(str, "�� �� �� �� �ֱ��Ӧ�L���֡���ɽ�������ԣ�");
	TextOut(sys_mdc, 128, 256 + 32, str, strlen(str));
	sprintf_s(str, "���Լ�Ŀ��ƹ�ϵ��");
	TextOut(sys_mdc, 128, 256 + 32 * 2, str, strlen(str));
	sprintf_s(str, "�����L �w ɽ��");
	TextOut(sys_mdc, 128, 256 + 32 * 3, str, strlen(str));
	sprintf_s(str, "����ɽ �w ��");
	TextOut(sys_mdc, 128, 256 + 32 * 4, str, strlen(str));
	sprintf_s(str, "������ �w �֡�");
	TextOut(sys_mdc, 128, 256 + 32 * 5, str, strlen(str));
	sprintf_s(str, "������ �w �L��");
	TextOut(sys_mdc, 128, 256 + 32 * 6, str, strlen(str));
	sprintf_s(str, "�����Ѷȣ�Ŀǰ�����С�����������Ѷȣ���");
	TextOut(sys_mdc, 128, 256 + 32 * 7, str, strlen(str));
	sprintf_s(str, "�����m-���Ʒ֣�Ŀǰ�����С��Ʒ֣�����-��׼����Ϸ��ʽ���ʺϾ�������ˣ�");
	TextOut(sys_mdc, 128, 256 + 32 * 8, str, strlen(str));
	sprintf_s(str, "�����y-�Դ�����˻�������ţ��O-���y�Ļ����ϡ�����ɫ��ʶ��");
	TextOut(sys_mdc, 128, 256 + 32 * 9, str, strlen(str));
	sprintf_s(str, "����ʱ����BUG�����д�Լ2minʱ�������Ϣ��������ֻ���˳���");
	TextOut(sys_mdc, 128, 256 + 32 * 11, str, strlen(str));

	//��mdc�е�ȫ����������hdc��
	BitBlt(sys_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, sys_mdc, 0, 0, SRCCOPY);

	g_tPre = GetTickCount64();

	return 0;

}

BOOL sys_Destroy(HWND hwnd)
{
	//�ͷ���Դ����
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
		switch (LOWORD(wParam))//wParam�ĵ�λ�ֶ��ǰ�ť��ID
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
		/*GetClientRect(hDlg, &drect);		//ȡ�ô�������Ĵ�С
		dsize.cx = abs(drect.right - drect.left);
		dsize.cy = abs(drect.bottom - drect.top);*/
		SetTimer(hDlg, ID_TIMER1, 1000, NULL);
		num = 10;
		return TRUE;
	case WM_PAINT:
		hdc = BeginPaint(hDlg, &ps);
		SetBkMode(hdc, TRANSPARENT);    //����������ʾ����͸��
		SelectObject(hdc, hFont);
		SetTextAlign(hdc, TA_CENTER | TA_BASELINE);
		sprintf_s(str, "�Α�Y��");
		TextOut(hdc, 233, 45, str, strlen(str));
		sprintf_s(str, "�֔���%d", score);
		TextOut(hdc, 233, 144 - 10, str, strlen(str));
		sprintf_s(str, "����B����%d", maxcombo);
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
		switch (LOWORD(wParam))//wParam�ĵ�λ�ֶ��ǰ�ť��ID
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