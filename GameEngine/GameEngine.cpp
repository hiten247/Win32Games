//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
#include "Framework.h"
#include "GameEngine.h"

GameEngine* GameEngine::m_pGameEngine = NULL;

//------------------------------------------------------------------------------
// Global functions
//------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);

LPCTSTR fnGameEngineVersion()
{
	return TEXT("Win32 GameEngine v1.0.0");
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpszCmdLine, int iCmdShow)
{
	MSG			msg;
	static int  iTickTrigger = 0;
	int			iTickCount;

	if (fnGameInitialize(hInstance))
	{
		// Init the game engine
		if (!GameEngine::GetEngine()->Initialize(iCmdShow))
			return FALSE;

		// Enter the msg loop
		while (TRUE)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				// process the msg
				if (msg.message == WM_QUIT)
					break;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				// check if game is not asleep
				if (!GameEngine::GetEngine()->GetSleep())
				{
					// check tick count to see if game cycle has elapsed
					iTickCount = GetTickCount();
					if (iTickCount > iTickTrigger)
					{
						iTickTrigger = iTickCount + GameEngine::GetEngine()->GetFrameDelay();
						fnGameCycle();
					}
				}
			}
		}
		return (int)msg.wParam;
	}

	fnGameEnd();

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return GameEngine::GetEngine()->HandleEvent(hWindow, msg, wParam, lParam);
}

//------------------------------------------------------------------------------
// Member function definitions
//------------------------------------------------------------------------------
GameEngine::GameEngine(HINSTANCE hInstance, LPCTSTR szWindowClass, LPCTSTR szTitle,
	WORD wIcon, WORD wSmallIcon, int iWidth, int iHeight)
{
	// set the game engine variables
	m_pGameEngine = this;
	m_hInstance = hInstance;
	m_hWindow = NULL;
	if (lstrlen(szWindowClass) > 0)
		lstrcpy(m_szWindowClass, szWindowClass);
	if (lstrlen(szTitle) > 0)
		lstrcpy(m_szTitle, szTitle);
	m_wIcon = wIcon;
	m_wSmallIcon = wSmallIcon;
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_iFrameDelay = 50; // 20 FPS default
	m_bSleep = TRUE;
}

GameEngine::~GameEngine()
{
}

BOOL GameEngine::Initialize(int iCmdShow)
{
	WNDCLASSEX wc;

	// Create window class for main window
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(GetIcon()));
	wc.hIconSm = LoadIcon(m_hInstance, MAKEINTRESOURCE(GetSmallIcon()));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_szWindowClass;

	if (!RegisterClassEx(&wc))
		return FALSE;

	// calculate window size and pos based on game size
	int iWindowWidth = m_iWidth + GetSystemMetrics(SM_CXFIXEDFRAME) * 2,
		iWindowHeight = m_iHeight + GetSystemMetrics(SM_CYFIXEDFRAME) * 2 +
		GetSystemMetrics(SM_CYCAPTION);

	if (wc.lpszMenuName != NULL)
		iWindowHeight += GetSystemMetrics(SM_CYMENU);

	int iXWindowPos = (GetSystemMetrics(SM_CXSCREEN) - iWindowWidth) / 2,
		iYWindowPos = (GetSystemMetrics(SM_CYSCREEN) - iWindowHeight) / 2;

	// create the window
	m_hWindow = CreateWindow(m_szWindowClass, m_szTitle, WS_POPUPWINDOW |
		WS_CAPTION | WS_MINIMIZEBOX, iXWindowPos, iYWindowPos, iWindowWidth,
		iWindowHeight, NULL, NULL, m_hInstance, NULL);
	if (!m_hWindow)
		return FALSE;

	// show and update window
	ShowWindow(m_hWindow, iCmdShow);
	UpdateWindow(m_hWindow);

	return TRUE;
}

LRESULT GameEngine::HandleEvent(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// route win msgs for game
	switch (msg)
	{
	case WM_CREATE:
		// set game window and start
		SetWindow(hWindow);
		fnGameStart(hWindow);
		return 0;

	case WM_ACTIVATE:
		// activate / deactivate the game, update sleep status
		if (wParam != WA_INACTIVE)
		{
			fnGameActivate(hWindow);
			SetSleep(FALSE);
		}
		else
		{
			fnGameDeactivate(hWindow);
			SetSleep(TRUE);
		}
		return 0;

	case WM_PAINT:
		HDC			hdc;
		PAINTSTRUCT ps;
		hdc = BeginPaint(hWindow, &ps);

		// Paint the game
		fnGamePaint(hdc);

		EndPaint(hWindow, &ps);
		return 0;

	case WM_DESTROY:
		// end game and exit
		fnGameEnd();
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWindow, msg, wParam, lParam);
}

