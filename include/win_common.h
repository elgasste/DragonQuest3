#if !defined( WIN_COMMON_H )
#define WIN_COMMON_H

#define VC_EXTRALEAN
#include <Windows.h>
#include <mmsystem.h>

#include "common.h"

#define DEFAULT_GRAPHICS_SCALE      2.0f
#define MIN_GRAPHICS_SCALE          1.0f
#define MAX_GRAPHICS_SCALE          4.0f
#define GRAPHICS_SCALE_STEP         0.5f
#define CLOCK_FPS_STEP              30

#define LOG_FILENAME                "dw3.log"
#define GAME_DATA_FILENAME          "data.dw3d"
#define WIN_DEBUG_CONFIG_FILENAME   "win_debug.cfg"

typedef struct MemArena_t MemArena_t;
typedef struct Game_t Game_t;

typedef struct WinGlobalObjects_t
{
   HWND hWndMain;
   char mainWindowClassName[MAX_PATH];
   HWND hWndDiagnostics;
   char diagnosticsWindowClassName[MAX_PATH];
   b32 anchorDiagnosticsWindow;
   b32 movingDiagnosticsWindow;
   LONG clientPaddingRight;
   LONG clientPaddingTop;
   HFONT hFont;
   LARGE_INTEGER performanceFrequency;
   BITMAPINFO bmpInfo;
   u32* buttonMap;
   r32 graphicsScale;
   char exeDir[MAX_PATH];
   char logFilePath[MAX_PATH];
   char debugConfigPath[MAX_PATH];

   MemArena_t* memArena;
   Game_t* game;
}
WinGlobalObjects_t;

extern WinGlobalObjects_t g_winGlobals;

// win_diagnostics.c
b32 CreateDiagnosticsWindow( HINSTANCE hInstance );
void SetDiagnosticsStatus( const char* msg );

// win_main.c
void MemArena_DumpStats( MemArena_t* memArena );
void SaveWinDebugConfig();
void ToggleDiagnosticsWindow( void );

// win_test_game_data.c
void WriteTestGameDataFile( const char *filePath );

#endif // WIN_COMMON_H
