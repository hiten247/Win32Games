//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
#include "Slideshow.h"

GameEngine* _pGame = NULL;

BOOL fnGameInitialize(HINSTANCE hInstance)
{
	_pGame = new GameEngine(hInstance, TEXT("Slideshow"), TEXT("Slideshow"),
		IDI_APP, IDI_APP_SM);
	if (_pGame == NULL)
		return FALSE;

	_pGame->SetFrameRate(1);

	_hInstance = hInstance;

	return TRUE;
}

void fnGameStart(HWND hWindow)
{
	HDC hdc = GetWindowDC(hWindow);
	_pSlides[0] = new Bitmap(hdc, (LPTSTR)TEXT("Image1.bmp"));
	_pSlides[1] = new Bitmap(hdc, (LPTSTR)TEXT("Image2.bmp"));
	_pSlides[2] = new Bitmap(hdc, (LPTSTR)TEXT("Image3.bmp"));
	_pSlides[3] = new Bitmap(hdc, IDB_BITMAP4, _hInstance);
	_pSlides[4] = new Bitmap(hdc, IDB_BITMAP5, _hInstance);
	_pSlides[5] = new Bitmap(hdc, IDB_BITMAP6, _hInstance);
	_pSlides[6] = new Bitmap(hdc, 640, 480, RGB(128, 128, 64));

	// set first slide
	_iCurSlide = 0;
}

void fnGameEnd()
{
	// clean up bitmaps
	for (int i = 0; i < _iNUMSLIDES; i++)
		delete _pSlides[i];

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
	// draw the current slide bitmap
	_pSlides[_iCurSlide]->Draw(hdc, 0, 0);
}

void fnGameCycle()
{
	static int iDelay = 0;
	// delay for 3sec
	if (++iDelay > 3)
	{
		iDelay = 0;

		// move to next slide
		if (++_iCurSlide == _iNUMSLIDES)
			_iCurSlide = 0;

		// force a repaint to draw the next slide
		InvalidateRect(_pGame->GetWindow(), NULL, FALSE);
	}
}