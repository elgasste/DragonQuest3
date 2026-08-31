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

#define MIN_GAME_FPS             30
#define MAX_GAME_FPS             120
#define GAME_FPS_STEP            30

#define LOG_FILENAME             "dw3.log"
#define GAME_DATA_FILENAME       "data.dw3d"

#define VK_NOCLIP                49    // 1
#define VK_SHOWHITBOXES          50    // 2
#define VK_MOVEFAST              51    // 3

typedef struct MemArena_t MemArena_t;
typedef struct Game_t Game_t;

typedef struct WinCornerPopup_t
{
   b32 show;
   char msg[STRING_SIZE_DEFAULT];
   u64 untilMs;
}
WinCornerPopup_t;

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
   char exeDir[MAX_PATH];
   char logFilePath[MAX_PATH];

   MemArena_t* memArena;
   Game_t* game;
}
WinGlobalObjects_t;

extern WinGlobalObjects_t g_winGlobals;
extern WinCornerPopup_t g_winCornerPopup;

// win_test_game_data.c
void WriteTestGameDataFile( const char *filePath );

#endif // WIN_COMMON_H
