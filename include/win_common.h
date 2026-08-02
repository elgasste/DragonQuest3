#if !defined( WIN_COMMON_H )
#define WIN_COMMON_H

#define VC_EXTRALEAN
#include <Windows.h>
#include <mmsystem.h>

#include "common.h"
#include "game.h"

typedef struct WinGlobalObjects_t
{
   HWND hWndMain;
   HFONT hFont;
   LARGE_INTEGER performanceFrequency;
   BITMAPINFO bmpInfo;
   Game_t game;
   b32 shutdown;
}
WinGlobalObjects_t;

WinGlobalObjects_t g_winGlobals;

#endif // !defined( WIN_COMMON_H )
