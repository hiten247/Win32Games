#pragma once
//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
#include <Windows.h>
#include "Resource.h"
#include "GameEngine.h"
#include "Bitmap.h"

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------
HINSTANCE				_hInstance;
extern GameEngine*		_pGameEngine;
const int				_iNUMSLIDES = 7;
Bitmap*					_pSlides[_iNUMSLIDES];
int						_iCurSlide;
