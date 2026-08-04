#if !defined( WIN_COMMON_H )
#define WIN_COMMON_H

#define VC_EXTRALEAN
#include <Windows.h>
#include <mmsystem.h>

#include "common.h"

#define DEFAULT_GRAPHICS_SCALE 2.0f

typedef struct Game_t Game_t;

typedef struct WinGlobalObjects_t
{
   HWND hWndMain;
   HFONT hFont;
   LARGE_INTEGER performanceFrequency;
   BITMAPINFO bmpInfo;
   u32* buttonMap;
   // TODO: move this into screen.h?
   r32 graphicsScale;
   Game_t* game;
}
WinGlobalObjects_t;

WinGlobalObjects_t g_winGlobals;

#endif // !defined( WIN_COMMON_H )
