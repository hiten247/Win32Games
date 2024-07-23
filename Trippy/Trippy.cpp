//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
#include "Trippy.h"

GameEngine* _pGame = NULL;

//------------------------------------------------------------------------------
// Game functions
//------------------------------------------------------------------------------
BOOL fnGameInitialize(HINSTANCE hInstance)
{
	// create the game engine
	_pGame = new GameEngine(hInstance, TEXT("Trippy"), TEXT("Trippy"), IDI_APP,
		IDI_APP_SM);
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

	// set initial size and pos of game rect
	_rcGameRect.left = _pGame->GetWidth() * 2 / 5;
	_rcGameRect.top = _pGame->GetHeight() * 2 / 5;
	_rcGameRect.right = _rcGameRect.left + _pGame->GetWidth() / 5;
	_rcGameRect.bottom = _rcGameRect.top + _pGame->GetHeight() / 5;
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
}

void fnGameDeactivate(HWND hWindow)
{
}

void fnGamePaint(HDC hdc)
{
	// draw grid lines
	int iGridLines = 50;
	for (int i = 0; i < iGridLines; i++)
	{
		// draw horizontal line
		MoveToEx(hdc, _pGame->GetWidth() * i / iGridLines, 0, NULL);
		LineTo(hdc, _pGame->GetWidth() * i / iGridLines, _pGame->GetHeight());

		// draw vertical line
		MoveToEx(hdc, 0, _pGame->GetHeight() * i / iGridLines, NULL);
		LineTo(hdc, _pGame->GetWidth(), _pGame->GetHeight() * i / iGridLines);
	}
}

void fnGameCycle()
{
	HDC		hdc;
	HWND	hWindow = _pGame->GetWindow();
	HBRUSH	hBrush;
	
	// randomly alter the rect
	int iInflation = (rand() % 21) - 10;
	InflateRect(&_rcGameRect, iInflation, iInflation);
	OffsetRect(&_rcGameRect, (rand() % 19) - 9, (rand() % 19) - 9);

	// draw new rect with random color
	hBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
	hdc = GetDC(hWindow);
	FillRect(hdc, &_rcGameRect, hBrush);
	ReleaseDC(hWindow, hdc);
	DeleteObject(hBrush);
}