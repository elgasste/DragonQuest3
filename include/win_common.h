#if !defined( WIN_COMMON_H )
#define WIN_COMMON_H

#define VC_EXTRALEAN
#include <Windows.h>
#include <mmsystem.h>

#include "common.h"

#define DEFAULT_GRAPHICS_SCALE   2.0f
#define MIN_GRAPHICS_SCALE       0.5f
#define MAX_GRAPHICS_SCALE       4.0f
#define GRAPHICS_SCALE_STEP      0.5f

#define MIN_GAME_FPS             15
#define MAX_GAME_FPS             120
#define GAME_FPS_STEP            15

#define LOG_FILE_NAME            "log.txt"
#define GAME_DATA_FILENAME       "data.dw3d"

typedef struct Game_t Game_t;

typedef struct WinGlobalObjects_t
{
   HWND hWndMain;
   LONG clientPaddingRight;
   LONG clientPaddingTop;
   HFONT hFont;
   LARGE_INTEGER performanceFrequency;
   BITMAPINFO bmpInfo;
   u32* buttonMap;
   r32 graphicsScale;
   b32 showDiagnostics;
   char exeDir[MAX_PATH];
   char logFilePath[MAX_PATH];

   Game_t* game;
}
WinGlobalObjects_t;

WinGlobalObjects_t g_winGlobals;

#endif // !defined( WIN_COMMON_H )
