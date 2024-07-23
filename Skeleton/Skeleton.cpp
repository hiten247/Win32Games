//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
#include "Skeleton.h"

GameEngine* _pGame = NULL;

//------------------------------------------------------------------------------
// Game functions
//------------------------------------------------------------------------------
BOOL fnGameInitialize(HINSTANCE hInstance)
{
	// create the game engine
	_pGame = new GameEngine(hInstance, TEXT("Game Skeleton"),
		TEXT("Game Skeleton"), IDI_SKELETON, IDI_SKELETON_SM);
	if (_pGame == NULL)
		return FALSE;

	// set frame rate
	_pGame->SetFrameRate(15);

	return TRUE;
}

void fnGameStart(HWND hWindow)
{
	// seed the random number generator
	srand(GetTickCount());
}

void fnGameEnd()
{
	// clean up game engine
	if (_pGame != NULL)
		delete _pGame;

	_pGame = NULL;
}

void fnGameActivate(HWND hWindow)
{
	HDC		hDC;
	RECT	rc;

	// draw activation text on game screen
	GetClientRect(hWindow, &rc);
	hDC = GetDC(hWindow);
	DrawText(hDC, TEXT("Activated!"), -1, &rc,
		DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	ReleaseDC(hWindow, hDC);
}

void fnGameDeactivate(HWND hWindow)
{
	HDC		hDC;
	RECT	rc;

	// draw activation text on game screen
	GetClientRect(hWindow, &rc);
	hDC = GetDC(hWindow);
	DrawText(hDC, TEXT("Deactivated!"), -1, &rc,
		DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	ReleaseDC(hWindow, hDC);
}

void fnGamePaint(HDC hdc)
{
}

void fnGameCycle()
{
	HDC		hDC;
	HWND	hWindow = _pGame->GetWindow();

	// draw the icon at random positions on game screen
	hDC = GetDC(hWindow);
	DrawIcon(hDC, rand() % _pGame->GetWidth(), rand() % _pGame->GetHeight(),
		(HICON)(DWORD)GetClassLongPtr(hWindow, GCLP_HICON));
	ReleaseDC(hWindow, hDC);
}