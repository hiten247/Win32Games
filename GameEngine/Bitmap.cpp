//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
#include "Framework.h"
#include "Bitmap.h"

Bitmap::Bitmap() : m_hBitmap(NULL), m_iWidth(0), m_iHeight(0)
{

}

Bitmap::Bitmap(HDC hdc, LPTSTR lpszFilename)
{
	Create(hdc, lpszFilename);
}

Bitmap::Bitmap(HDC hdc, UINT uiResID, HINSTANCE hInstance)
{
	Create(hdc, uiResID, hInstance);
}

Bitmap::Bitmap(HDC hdc, int iWidth, int iHeight, COLORREF crColor)
{
	Create(hdc, iWidth, iHeight, crColor);
}

Bitmap::~Bitmap()
{
	Free();
}

void Bitmap::Free()
{
	if (m_hBitmap != NULL)
	{
		DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}
}

BOOL Bitmap::Create(HDC hdc, LPTSTR lpszFilename)
{
	// clear existing
	Free();

	// open bmp file
	HANDLE hFile = CreateFile(lpszFilename, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	// read bitmap header
	BITMAPFILEHEADER bmfHeader;
	DWORD			 dwBytesRead;
	BOOL			 bOK = ReadFile(hFile, (LPVOID)&bmfHeader, sizeof(BITMAPFILEHEADER),
		&dwBytesRead, NULL);

	if ((!bOK) || (dwBytesRead != sizeof(BITMAPFILEHEADER)) || (bmfHeader.bfType != 0x4D42))
	{
		CloseHandle(hFile);
		return FALSE;
	}

	BITMAPINFO* pBitmapInfo = (BITMAPINFO*)(new BITMAPINFO_256);
	if (pBitmapInfo != NULL)
	{
		// read the bitmap info header
		bOK = ReadFile(hFile, (LPVOID)pBitmapInfo, sizeof(BITMAPINFOHEADER), &dwBytesRead, NULL);
		if ((!bOK) || (dwBytesRead != sizeof(BITMAPINFOHEADER)))
		{
			CloseHandle(hFile);
			Free();
			return FALSE;
		}

		m_iWidth = pBitmapInfo->bmiHeader.biWidth;
		m_iHeight = pBitmapInfo->bmiHeader.biHeight;

		// adjust file ptr
		if (pBitmapInfo->bmiHeader.biSize != sizeof(BITMAPINFOHEADER))
			SetFilePointer(hFile, pBitmapInfo->bmiHeader.biSize - sizeof(BITMAPINFOHEADER),
				NULL, FILE_CURRENT);

		// read color info
		bOK = ReadFile(hFile, pBitmapInfo->bmiColors,
			pBitmapInfo->bmiHeader.biClrUsed * sizeof(RGBQUAD), &dwBytesRead, NULL);
		if ((!bOK) || (dwBytesRead != (pBitmapInfo->bmiHeader.biClrUsed * sizeof(RGBQUAD))))
		{
			CloseHandle(hFile);
			Free();
			return FALSE;
		}

		//Get handle to the bitmap and copy the image bits
		PBYTE pBitmapBits;
		m_hBitmap = CreateDIBSection(hdc, pBitmapInfo, DIB_RGB_COLORS,
			(PVOID*)&pBitmapBits, NULL, 0);
		if ((m_hBitmap != NULL) && (pBitmapBits != NULL))
		{
			SetFilePointer(hFile, bmfHeader.bfOffBits, NULL, FILE_BEGIN);
			bOK = ReadFile(hFile, pBitmapBits, pBitmapInfo->bmiHeader.biSizeImage,
				&dwBytesRead, NULL);
			if (bOK)
				return TRUE;
		}
	}
	
	Free();
	return FALSE;
}

BOOL Bitmap::Create(HDC hdc, UINT uiResID, HINSTANCE hInstance)
{
	Free();

	HRSRC hResInfo = FindResource(hInstance, MAKEINTRESOURCE(uiResID), RT_BITMAP);
	if (hResInfo == NULL)
		return FALSE;

	// load the bitmap resource
	HGLOBAL hMemBitmap = LoadResource(hInstance, hResInfo);
	if (hMemBitmap == NULL)
		return FALSE;

	// lock resource to access the image
	PBYTE pBitmapImage = (BYTE*)LockResource(hMemBitmap);
	if (pBitmapImage == NULL)
	{
		FreeResource(hMemBitmap);
		return FALSE;
	}

	BITMAPINFO* pBitmapInfo = (BITMAPINFO*)pBitmapImage;
	m_iWidth = pBitmapInfo->bmiHeader.biWidth;
	m_iHeight = pBitmapInfo->bmiHeader.biHeight;

	//Get handle to the bitmap and copy the image bits
	PBYTE pBitmapBits;
	m_hBitmap = CreateDIBSection(hdc, pBitmapInfo, DIB_RGB_COLORS,
		(PVOID*)&pBitmapBits, NULL, 0);
	if ((m_hBitmap != NULL) && (pBitmapBits != NULL))
	{
		const PBYTE pTempBits = pBitmapImage + pBitmapInfo->bmiHeader.biSize +
			pBitmapInfo->bmiHeader.biClrUsed * sizeof(RGBQUAD);
		CopyMemory(pBitmapBits, pTempBits, pBitmapInfo->bmiHeader.biSizeImage);

		//Unlcok and free the bitmap graphics object
		UnlockResource(hMemBitmap);
		FreeResource(hMemBitmap);
		return TRUE;
	}

	//Something went wrong, so cleanup everything
	UnlockResource(hMemBitmap);
	FreeResource(hMemBitmap);
	Free();
	return FALSE;
}

BOOL Bitmap::Create(HDC hdc, int iWidth, int iHeight, COLORREF crColor)
{
	m_hBitmap = CreateCompatibleBitmap(hdc, iWidth, iHeight);
	if (m_hBitmap == NULL)
		return FALSE;

	m_iWidth = iWidth;
	m_iHeight = iHeight;

	// create mem dc for drawing
	HDC hMemDC = CreateCompatibleDC(hdc);

	// create brush
	HBRUSH hBrush = CreateSolidBrush(crColor);

	// select bitmap
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);

	// fill bitmap with color
	RECT rcBitmap = { 0, 0, m_iWidth, m_iHeight };
	FillRect(hMemDC, &rcBitmap, hBrush);

	// clean up
	SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);
	DeleteObject(hBrush);

	return TRUE;
}

void Bitmap::Draw(HDC hdc, int x, int y)
{
	if (m_hBitmap != NULL)
	{
		HDC hMemDC = CreateCompatibleDC(hdc);

		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);

		BitBlt(hdc, x, y, GetWidth(), GetHeight(), hMemDC, 0, 0, SRCCOPY);

		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);
	}
}