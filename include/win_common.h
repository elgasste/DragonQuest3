#if !defined( WIN_COMMON_H )
#define WIN_COMMON_H

#define VC_EXTRALEAN
#include <Windows.h>
#include <mmsystem.h>

#include "common.h"

#define MIN_GRAPHICS_SCALE 1.0f
#define MAX_GRAPHICS_SCALE 4.0f
#define DEFAULT_GRAPHICS_SCALE 2.0f

typedef struct Game_t Game_t;

typedef struct WinGlobalObjects_t
{
   HWND hWndMain;
   HFONT hFont;
   LARGE_INTEGER performanceFrequency;
   BITMAPINFO bmpInfo;
   u32* buttonMap;
   r32 graphicsScale;
   b32 showDiagnostics;

   Game_t* game;
}
WinGlobalObjects_t;

WinGlobalObjects_t g_winGlobals;

#endif // !defined( WIN_COMMON_H )
