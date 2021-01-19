#include "stdafx.h"
#include "minesweeper.h"
#include <ctime>
#include <string>
#include <iomanip>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];            
WCHAR szKidWindowClass[MAX_LOADSTRING];            

int windows_width;
int windows_height;
int mines_count;
int flague_count;
int **matrix;
HWND** kidshWnd;
HWND parenthWnd;
HWND dialoghWnd;
bool mine;
bool firstclick;
int time_clock;
int dialogncmdshow;
bool debugging;
int mines_found;



ATOM                MyRegisterClass(HINSTANCE hInstance);
ATOM                KidRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    KidWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void Draw_Number(HWND);
void Step_On_Mine(HWND);
void Paint_Background(HWND,int);
void Draw_Zero(HWND);
void Counting_Flagues();
BOOL Make_New_Game(HINSTANCE, int);
void Draw_Flague(HWND);
void Kill_Children();


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PWSGLAB1, szWindowClass, MAX_LOADSTRING);
	LoadStringW(hInstance, 109, szKidWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	KidRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PWSGLAB1));
	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PWSGLAB1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PWSGLAB1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

ATOM KidRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = KidWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(109));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(109);
	wcex.lpszClassName = szKidWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	dialogncmdshow = nCmdShow;
	windows_width = 10;
	windows_height = 10;
	mines_count = 20;
	dialoghWnd = NULL;

	RECT rc = { 0,0,26 * windows_width - 1,26 * windows_height - 1 + 25 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX, TRUE);

	parenthWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX,
		0, 0, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);

	if (!parenthWnd)
	{
		return FALSE;
	}

	ShowWindow(parenthWnd, nCmdShow);
	UpdateWindow(parenthWnd);

	if (!Make_New_Game(hInstance, nCmdShow))
		return FALSE;

	return TRUE;
}


void Choose_Color(int c, int* r, int* g, int* b)
{
	if (c == 1)
	{
		*r = 255;
		*g = 0;
		*b = 0;
	}
	if (c == 2)
	{
		*r = 0;
		*g = 255;
		*b = 0;
	}
	if (c == 3)
	{
		*r = 0;
		*g = 0;
		*b = 255;
	}
	if (c == 4)
	{
		*r = 255;
		*g = 255;
		*b = 0;
	}
	if (c == 5)
	{
		*r = 0;
		*g = 255;
		*b = 255;
	}
	if (c == 6)
	{
		*r = 255;
		*g = 0;
		*b = 255;
	}
	if (c == 7)
	{
		*r = 128;
		*g = 128;
		*b = 0;
	}
	if (c == 8)
	{
		*r = 128;
		*g = 0;
		*b = 255;
	}

}

void Step_On_Mine(HWND hWnd)
{
	HDC hdc = GetDC(hWnd);
	HBITMAP bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(131));
	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, bitmap);
	BITMAP bmInfo;
	GetObject(bitmap, sizeof(bmInfo), &bmInfo);
	BitBlt(hdc, 0, 0, 20, 20, memDC, 0, 0, SRCCOPY);
	StretchBlt(hdc, 0, 0, 25, 25, memDC, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight, SRCCOPY);
	SelectObject(memDC, oldBitmap);
	DeleteObject(bitmap);
	DeleteDC(memDC);
	if (!debugging)
	{
		KillTimer(parenthWnd, 7);
		SendMessage(parenthWnd, WM_COMMAND, 32771, 0);
		MessageBox(parenthWnd, _T("BOOM!"), _T("Mine"), MB_ICONERROR | MB_APPLMODAL);
		mine = true;
		
	}
}

void Paint_Background(HWND hWnd, int c)
{
	HDC hdc = GetDC(hWnd);
	RECT rc;
	GetClientRect(hWnd, &rc);
	HBRUSH brush = CreateSolidBrush(RGB(c, c, c));
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
	Rectangle(hdc, -1, -1, rc.right -rc.left + 1, rc.bottom - rc.top + 1);
	SelectObject(hdc, oldBrush);
	DeleteObject(brush);
	ReleaseDC(hWnd, hdc);
}

void Draw_Zero(HWND hWnd)
{
	if (!debugging)
	{
		int i = (int)GetProp(hWnd, _T("index_i")), j = (int)GetProp(hWnd, _T("index_j"));
		if (i != 0 && j != 0 && GetProp(kidshWnd[i - 1][j - 1], _T("full")) == false)
			Draw_Number(kidshWnd[i - 1][j - 1]);
		if (i != 0 && GetProp(kidshWnd[i - 1][j], _T("full")) == false)
			Draw_Number(kidshWnd[i - 1][j]);
		if (j != 0 && GetProp(kidshWnd[i][j - 1], _T("full")) == false)
			Draw_Number(kidshWnd[i][j - 1]);
		if (i != windows_height - 1 && j != windows_width - 1 && GetProp(kidshWnd[i + 1][j + 1], _T("full")) == false)
			Draw_Number(kidshWnd[i + 1][j + 1]);
		if (i != windows_height - 1 && GetProp(kidshWnd[i + 1][j], _T("full")) == false)
			Draw_Number(kidshWnd[i + 1][j]);
		if (j != windows_width - 1 && GetProp(kidshWnd[i][j + 1], _T("full")) == false)
			Draw_Number(kidshWnd[i][j + 1]);
		if (i != windows_height - 1 && j != 0 && GetProp(kidshWnd[i + 1][j - 1], _T("full")) == false)
			Draw_Number(kidshWnd[i + 1][j - 1]);
		if (i != 0 && j != windows_width - 1 && GetProp(kidshWnd[i - 1][j + 1], _T("full")) == false)
			Draw_Number(kidshWnd[i - 1][j + 1]);
	}
}

bool Checkflags(HWND hWnd)
{
	int countflags=0;
	int i = (int)GetProp(hWnd, _T("index_i")), j = (int)GetProp(hWnd, _T("index_j"));
	if (i != 0 && j != 0 && GetProp(kidshWnd[i - 1][j - 1], _T("flague")) == (HANDLE)true)
		countflags++;
	if (i != 0 && GetProp(kidshWnd[i - 1][j], _T("flague")) == (HANDLE)true)
		countflags++;
	if (j != 0 && GetProp(kidshWnd[i][j - 1], _T("flague")) == (HANDLE)true)
		countflags++;
	if (i != windows_height - 1 && j != windows_width - 1 && GetProp(kidshWnd[i + 1][j + 1], _T("flague")) == (HANDLE)true)
		countflags++;
	if (i != windows_height - 1 && GetProp(kidshWnd[i + 1][j], _T("flague")) == (HANDLE)true)
		countflags++;
	if (j != windows_width - 1 && GetProp(kidshWnd[i][j + 1], _T("flague")) == (HANDLE)true)
		countflags++;
	if (i != windows_height - 1 && j != 0 && GetProp(kidshWnd[i + 1][j - 1], _T("flague")) == (HANDLE)true)
		countflags++;
	if (i != 0 && j != windows_width - 1 && GetProp(kidshWnd[i - 1][j + 1], _T("flague")) == (HANDLE)true)
		countflags++;
	if (countflags==matrix[i][j]) {
		return true;
	}
	return false;
}

void Draw_Number(HWND hWnd)
{
	int i = (int)GetProp(hWnd, _T("index_i")), j = (int)GetProp(hWnd, _T("index_j"));
	if (!debugging)
		SetProp(hWnd, _T("full"), (HANDLE)true);
	if (matrix[i][j] == -1)
		Step_On_Mine(hWnd);
	else
	{
		if (!debugging)
			Paint_Background(hWnd, 255);

		if (matrix[i][j] == 0 || Checkflags(hWnd))
			Draw_Zero(hWnd);
		if (matrix[i][j]>0)
		{
			HDC hdc = GetDC(hWnd);
			RECT rc;
			GetClientRect(hWnd, &rc);
			TCHAR s[2];
			_itow_s(matrix[i][j], s, 16);
			int r, g, b;
			Choose_Color(matrix[i][j], &r, &g, &b);
			SetTextColor(hdc, RGB(r, g, b));
			if (debugging)
				SetBkMode(hdc, TRANSPARENT);
			DrawText(hdc, s, (int)_tcslen(s), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			ReleaseDC(hWnd, hdc);
		}
	}
}

void Draw_Flague(HWND hWnd)
{
	HDC hdc = GetDC(hWnd);
	HBITMAP bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(130));
	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, bitmap);
	BITMAP bmInfo;
	GetObject(bitmap, sizeof(bmInfo), &bmInfo);
	BitBlt(hdc, 0, 0, 20, 20, memDC, 0, 0, SRCCOPY);
	StretchBlt(hdc, 0, 0, 25, 25, memDC, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight, SRCCOPY);
	SetProp(hWnd, _T("full"), (HANDLE)true);
	SetProp(hWnd, _T("flague"), (HANDLE)true);
	SelectObject(memDC, oldBitmap);
	DeleteObject(bitmap);
	DeleteDC(memDC);
}

void Counting_Flagues()
{
	HDC hdc = GetDC(parenthWnd);
	RECT rc;
	GetClientRect(parenthWnd, &rc);
	TCHAR s[5];
	_stprintf_s(s, 5, TEXT("%04d"),flague_count);
	TextOut(hdc, 3 * (rc.right - rc.left) / 4, 5, s, (int)_tcslen(s));
	ReleaseDC(parenthWnd, hdc);
}

void Kill_Children()
{
	for (int i = 0; i < windows_height; i++)
	{
		for (int j = 0; j < windows_width; j++)
		{
			DestroyWindow(kidshWnd[i][j]);

		}
		delete[] matrix[i];
	}
}

void Clock_Show()
{
	RECT rc;
	HDC hdc = GetDC(parenthWnd);
	GetClientRect(parenthWnd, &rc);
	TCHAR s[5];
	_stprintf_s(s, 5, TEXT("%04d"), time_clock);
	TextOut(hdc, (rc.right - rc.left) / 4, 5, s, (int)_tcslen(s));
	ReleaseDC(parenthWnd, hdc);
}

void UpdateNumbers() 
{
	for (int i = 0; i < windows_height; i++)
		for (int j = 0; j < windows_width; j++)
		{
			if (matrix[i][j] != -1)
			{
				int count = 0;
				if (i != 0 && j != 0 && matrix[i - 1][j - 1] == -1)
					count++;
				if (i != 0 && matrix[i - 1][j] == -1)
					count++;
				if (j != 0 && matrix[i][j - 1] == -1)
					count++;
				if (i != windows_height - 1 && j != windows_width - 1 && matrix[i + 1][j + 1] == -1)
					count++;
				if (i != windows_height - 1 && matrix[i + 1][j] == -1)
					count++;
				if (j != windows_width - 1 && matrix[i][j + 1] == -1)
					count++;
				if (i != windows_height - 1 && j != 0 && matrix[i + 1][j - 1] == -1)
					count++;
				if (i != 0 && j != windows_width - 1 && matrix[i - 1][j + 1] == -1)
					count++;
				matrix[i][j] = count;
			}
		}
}

BOOL Make_New_Game(HINSTANCE hInstance, int nCmdShow)
{
	mines_found = 0;
	flague_count = mines_count;
	mine = false;
	firstclick = true;
	time_clock = 0;

	HMENU hmenu = GetMenu(parenthWnd);
	debugging = false;
	CheckMenuItem(hmenu, 32771, MFS_UNCHECKED);

	kidshWnd = (HWND**)malloc(windows_height*sizeof(HWND*));
	for (int i = 0; i < windows_height; i++)
	{
		kidshWnd[i] = (HWND*)malloc(windows_width*sizeof(HWND));
	}

	for (int i = 0; i < windows_height; i++)
		for (int j = 0; j < windows_width; j++)
		{
			kidshWnd[i][j] = CreateWindowW(szKidWindowClass, szTitle, WS_CHILD,
				j * 26, 25 + i * 26, 25, 25, parenthWnd, nullptr, hInstance, nullptr);
			SetWindowLong(kidshWnd[i][j], GWL_WNDPROC, (long)& KidWndProc);
		}
	for (int i = 0; i < windows_height; i++)
		for (int j = 0; j < windows_width; j++)
			if (!kidshWnd[i][j])
			{
				return FALSE;
			}

	Counting_Flagues();
	Clock_Show();

	srand(time(NULL));
	int pi, pj;
	matrix = (int**)malloc(windows_height*sizeof(int*));

	for (int i = 0; i < windows_height; i++) 
	{
		matrix[i] = (int*)malloc(windows_width * sizeof(int));
		for (int j = 0; j < windows_width; j++)
		{
			matrix[i][j] = 0;
		}
	}
	for (int i = 0; i < mines_count; i++)
	{
		do {
			pi = rand() % windows_height;
			pj = rand() % windows_width;
		} while (matrix[pi][pj] == -1);
		matrix[pi][pj] = -1;
	}
	UpdateNumbers();
	for (int i = 0; i < windows_height; i++)
		for (int j = 0; j < windows_width; j++)
		{
			Paint_Background(kidshWnd[i][j], 223);
			SetProp(kidshWnd[i][j], _T("index_i"), (HANDLE)i);
			SetProp(kidshWnd[i][j], _T("index_j"), (HANDLE)j);
			SetProp(kidshWnd[i][j], _T("full"), (HANDLE)false);
			SetProp(kidshWnd[i][j], _T("flague"), (HANDLE)false);
			ShowWindow(kidshWnd[i][j], nCmdShow);
			UpdateWindow(kidshWnd[i][j]);
		}

	return TRUE;
}

LRESULT CALLBACK KidWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_RBUTTONDOWN:
	{
		int i = (int)GetProp(hWnd, _T("index_i")), j = (int)GetProp(hWnd, _T("index_j"));
		if (firstclick)
		{
			SetTimer(parenthWnd, 7, 1000, NULL);
			firstclick = false;
		}
		if (!mine)
		{
			if (GetProp(hWnd, _T("flague")))
			{
				if (matrix[i][j] == -1)
					mines_found--;
				SetProp(hWnd, _T("flague"), (HANDLE)false);
				SetProp(hWnd, _T("full"), (HANDLE)false);
				flague_count++;
				Counting_Flagues();
				HDC hdc = GetDC(hWnd);
				RECT rc;
				GetClientRect(hWnd, &rc);
				HBRUSH brush = CreateSolidBrush(RGB(223, 223, 223));
				HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
				Rectangle(hdc, -1, -1, rc.right + 1, rc.bottom + 1);
				SelectObject(hdc, oldBrush);
				DeleteObject(brush);
				ReleaseDC(hWnd, hdc);
				if (mines_found == mines_count && flague_count == 0) {
					KillTimer(parenthWnd, 7);
					MessageBox(parenthWnd, _T("You win!"), _T("Brawo!"), MB_APPLMODAL);					
				}
			}
			else if (!GetProp(hWnd, _T("full")))
			{
				if (flague_count > 0) {
					if (matrix[i][j] == -1)
						mines_found++;
					flague_count--;
					Counting_Flagues();
					Draw_Flague(hWnd);
					if (mines_found == mines_count && flague_count == 0) {
						KillTimer(parenthWnd, 7);
						MessageBox(parenthWnd, _T("You win!"), _T("Brawo!"), MB_APPLMODAL);					
					}
				}
			}
		}
	}
	break;
	case WM_LBUTTONDOWN:
	{
		if (firstclick)
		{
			SetTimer(parenthWnd, 7, 1000, NULL);
			firstclick = false;
			int i = (int)GetProp(hWnd, _T("index_i")), j = (int)GetProp(hWnd, _T("index_j"));
			if ((!debugging) && (matrix[i][j] == -1))
			{
				matrix[i][j] = 0;
				int pi, pj;
				do {
					pi = rand() % windows_height;
					pj = rand() % windows_width;
				} while ((pi==i && pj==j) || matrix[pi][pj] == -1);
				matrix[pi][pj] = -1;
				UpdateNumbers();
			}
		}
		if (!mine)
			Draw_Number(hWnd);
	}
	break;
	case WM_PAINT:
	{
		int i = (int)GetProp(hWnd, _T("index_i")), j = (int)GetProp(hWnd, _T("index_j"));
		if (debugging && (!GetProp(kidshWnd[i][j], _T("full")) || (GetProp(kidshWnd[i][j], _T("full")) && GetProp(kidshWnd[i][j], _T("flague")))))
			Draw_Number(hWnd);
		if (!debugging && (GetProp(hWnd, _T("full"))) && matrix[i][j]>0)
		{			
			PAINTSTRUCT ps;
			HDC m_hDC = BeginPaint(hWnd, &ps);
			HDC memDC = CreateCompatibleDC(m_hDC);
			HBITMAP hBM = CreateCompatibleBitmap(m_hDC, 25, 25);
			SelectObject(memDC, hBM);
			RECT rc;
			GetClientRect(hWnd, &rc);
			//Draw background
			HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
			FillRect(memDC, &rc, brush);
			// Draw text
			TCHAR s[2];
			_itow_s(matrix[i][j], s, 16);
			int r, g, b;
			Choose_Color(matrix[i][j], &r, &g, &b);
			SetTextColor(memDC, RGB(r, g, b));
			DrawText(memDC, s, (int)_tcslen(s), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			// Paint on window
			BitBlt(m_hDC, 0, 0, 25, 25, memDC, 0, 0, SRCCOPY);
			DeleteObject(hBM);
			DeleteDC(memDC);
			EndPaint(hWnd, &ps);
		}
		if (!debugging && GetProp(hWnd, _T("full")) && GetProp(hWnd, _T("flague")))
			Draw_Flague(hWnd);
		if (!debugging && !GetProp(hWnd, _T("full")))
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			HBRUSH brush = CreateSolidBrush(RGB(223, 223, 223));
			HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
			Rectangle(hdc, -1, -1, rc.right - rc.left + 1, rc.bottom - rc.top + 1);
			SelectObject(hdc, oldBrush);
			DeleteObject(brush);
			EndPaint(hWnd, &ps);
		}
	}
	break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

BOOL MyDialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
UNREFERENCED_PARAMETER(lParam);

switch (message)
{
case WM_INITDIALOG:
	return (INT_PTR)TRUE;

case WM_COMMAND:
	if (LOWORD(wParam) == 2)			//CANCEL
	{
		EndDialog(hWnd, LOWORD(wParam));
		dialoghWnd = NULL;
		return (INT_PTR)TRUE;
	}
	if (LOWORD(wParam) == IDOK)
	{
		int bufwindows_width = GetDlgItemInt(hWnd, 1005, NULL, false);
		int bufwindows_height = GetDlgItemInt(hWnd, 1006, NULL, false);
		int bufmines_count = GetDlgItemInt(hWnd, 1007, NULL, false);
		RECT rcworkarea;
		SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcworkarea, NULL);
		int maxwidth = rcworkarea.right / 30;
		int maxheight = rcworkarea.bottom / 30;
		if (maxwidth<bufwindows_width || maxheight<bufwindows_height || (bufwindows_width * bufwindows_height - bufmines_count) > 1900 ||
			(bufwindows_width * bufwindows_height - bufmines_count) < 5 || bufwindows_width<=0 || bufwindows_height<=0 || bufmines_count<=0)
		{
			std::string s;
			maxwidth < bufwindows_width ? s += "Превышено максимальное количество столбцов для вашего экрана. "
				: maxheight < bufwindows_height ? s += "Превышено максимальное количество строк для вашего экрана. "
				: (bufwindows_width * bufwindows_height - bufmines_count) > 1900 ? s += "Превышена площадь для данного количества мин. " 
				: (bufwindows_width * bufwindows_height - bufmines_count) < 5 ? s += "Сводных клеток должно быть больше 5. "
				:s+="Количество строк, столбцов и мин должно быть больше 0.";
			int len;
			int slength = (int)s.length() + 1;
			len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
			wchar_t* buf = new wchar_t[len];
			MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
			std::wstring r(buf);
			delete[] buf;
			LPCWSTR sw = r.c_str();
			MessageBox(parenthWnd, sw, _T("Неправильный ввод"), MB_ICONERROR | MB_APPLMODAL);
		}
		else {
			Kill_Children();
			windows_width = bufwindows_width;
			windows_height = bufwindows_height;
			mines_count = bufmines_count;
			if (windows_width < 5)
				windows_width = 5;
			if (mines_count > 4 * windows_height * windows_width / 5)
				mines_count = 4 * windows_height * windows_width / 5;
			RECT rc = { 0,0,26 * windows_width - 1,26 * windows_height - 1 + 25 };
			AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX, TRUE);
			SetWindowPos(parenthWnd, HWND_BOTTOM, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_SHOWWINDOW);
			Make_New_Game(hInst, dialogncmdshow);
			EndDialog(hWnd, LOWORD(wParam));
			dialoghWnd = NULL;
		}
		return (INT_PTR)TRUE;
	}
	break;

default:
	return (INT_PTR)FALSE;
}
return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case 32775:
		{
			KillTimer(parenthWnd, 7);
			Kill_Children();
			Make_New_Game(hInst, dialogncmdshow);
		}
		break;
		case 32776:
		{
			if (!IsWindow(dialoghWnd))
			{
				dialoghWnd = CreateDialog(hInst,
				MAKEINTRESOURCE(135),
				hWnd,
				(DLGPROC)MyDialogProc);
				SetDlgItemText(dialoghWnd, 1004, _T("Width:"));
				SetDlgItemText(dialoghWnd, 1008, _T("Height:"));
				SetDlgItemText(dialoghWnd, 1009, _T("Mines:"));
				SetDlgItemInt(dialoghWnd, 1005, windows_width, false);
				SetDlgItemInt(dialoghWnd, 1006, windows_height, false);
				SetDlgItemInt(dialoghWnd, 1007, mines_count, false);
				ShowWindow(dialoghWnd, SW_SHOW);
			}
		}
		break;
		case 32771:
		{
			HMENU hmenu = GetMenu(parenthWnd);
			if (!debugging)
			{
				debugging = true;
				CheckMenuItem(hmenu, 32771, MFS_CHECKED);
			}
			else
			{
				debugging = false;
				CheckMenuItem(hmenu, 32771, MFS_UNCHECKED);
			}
			for (int i = 0; i < windows_height; i++)
				for (int j = 0; j < windows_width; j++)
				{
					if (debugging && (!GetProp(kidshWnd[i][j], _T("full")) || (GetProp(kidshWnd[i][j], _T("full")) && GetProp(kidshWnd[i][j], _T("flague")))))
						Draw_Number(kidshWnd[i][j]);
					if (!debugging && GetProp(kidshWnd[i][j], _T("full")) && GetProp(kidshWnd[i][j], _T("flague")))
						Draw_Flague(kidshWnd[i][j]);
					if (!debugging && !GetProp(kidshWnd[i][j], _T("full")))
						Paint_Background(kidshWnd[i][j], 223);
				}
		}
		break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		RECT rc;
		GetClientRect(hWnd, &rc);
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		HBRUSH brush = CreateSolidBrush(RGB(255,255,255));
		HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
		Rectangle(hdc, -1, -1, rc.right-rc.left+1, rc.bottom-rc.top+1);
		SelectObject(hdc, oldBrush);
		DeleteObject(brush);
		EndPaint(hWnd, &ps);
		Clock_Show();
		Counting_Flagues();
	}
	break;
	case WM_TIMER:
	{
		Clock_Show();
		time_clock++;
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
