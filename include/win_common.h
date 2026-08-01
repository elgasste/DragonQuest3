#if !defined( WIN_COMMON_H )
#define WIN_COMMON_H

#define VC_EXTRALEAN
#include <Windows.h>
#include <mmsystem.h>

#include "common.h"
#include "win_pixel_buffer.h"

#define STRING_SIZE_DEFAULT               1024

typedef struct WinGlobalObjects_t
{
   HWND hWndMain;
   HFONT hFont;
   LARGE_INTEGER performanceFrequency;
   BITMAPINFO bmpInfo;
   WinPixelBuffer_t screenBuffer;
   b32 shutdown;
}
WinGlobalObjects_t;

WinGlobalObjects_t g_winGlobals;

#endif // !defined( WIN_COMMON_H )
