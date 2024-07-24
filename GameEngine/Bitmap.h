#pragma once

//------------------------------------------------------------------------------
// Bitmap info struct
//------------------------------------------------------------------------------
typedef struct _BITMAPINFO_256
{
	BITMAPINFOHEADER		bmiHeader;
	RGBQUAD					bmiColors[256];
} BITMAPINFO_256, *LPBITMAPINFO_256;

//------------------------------------------------------------------------------
// Bitmap class - works with 8-bit DIB
//------------------------------------------------------------------------------
class Bitmap
{
protected:
	HBITMAP		m_hBitmap;
	int			m_iWidth, m_iHeight;
	// Helper
	void Free();

public:
	Bitmap();
	Bitmap(HDC hdc, LPTSTR lpszFilename);
	Bitmap(HDC hdc, UINT uiResID, HINSTANCE hInstance);
	Bitmap(HDC hdc, int iWidth, int iHeight, COLORREF crColor = RGB(0, 0, 0));
	virtual ~Bitmap();

	// general methods
	BOOL Create(HDC hdc, LPTSTR lpszFilename);
	BOOL Create(HDC hdc, UINT uiResID, HINSTANCE hInstance);
	BOOL Create(HDC hdc, int iWidth, int iHeight, COLORREF crColor);
	void Draw(HDC hdc, int x, int y);
	int GetWidth() { return m_iWidth; }
	int GetHeight() { return m_iHeight; }
};
