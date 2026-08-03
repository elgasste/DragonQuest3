#include <stdio.h>

#include "win_common.h"
#include "mem_arena.h"
#include "game.h"
#include "pixel_buffer.h"

internal MemArena_t* CreateMemArena( void );
internal LRESULT CALLBACK MainWindowProc( _In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam );
internal void RenderScreen( void );
internal void HandleKeyboardInput( u32 keyCode, LPARAM flags );

int CALLBACK WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow )
{
   TIMECAPS timeCaps;
   UINT timerResolution;
   WNDCLASSA mainWindowClass = { 0 };
   DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE;
   RECT expectedWindowRect = { 0 };
   LONG clientPaddingRight, clientPaddingTop;
   MemArena_t* memArena;
   MemArenaResult_t result;

   UNUSED_PARAM( hPrevInstance );
   UNUSED_PARAM( lpCmdLine );
   UNUSED_PARAM( nCmdShow );

   memArena = CreateMemArena();
   result = MemArena_Alloc( memArena, (void**)&( g_winGlobals.game ), sizeof( Game_t ) );
   if ( result != MemArenaResult_Success )
   {
      FatalError( "failed to allocate memory for game object." );
      return 1;
   }

   Game_Create( g_winGlobals.game, memArena );

   if ( !QueryPerformanceFrequency( &( g_winGlobals.performanceFrequency ) ) )
   {
      FatalError( "failed to query performance frequency." );
      return 1;
   }

   if ( timeGetDevCaps( &timeCaps, sizeof( TIMECAPS ) ) != TIMERR_NOERROR )
   {
      FatalError( "failed to set timer resolution." );
      return 1;
   }

   timerResolution = min( max( timeCaps.wPeriodMin, 1 ), timeCaps.wPeriodMax );
   timeBeginPeriod( timerResolution );

   mainWindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
   mainWindowClass.lpfnWndProc = MainWindowProc;
   mainWindowClass.hInstance = hInstance;
   mainWindowClass.lpszClassName = "mainWindowClass";

   if ( !RegisterClassA( &mainWindowClass ) )
   {
      FatalError( "failed to register window class." );
      return 1;
   }

   expectedWindowRect.right = SCREEN_WIDTH;
   expectedWindowRect.bottom = SCREEN_HEIGHT;

   if ( !AdjustWindowRect( &expectedWindowRect, windowStyle, 0 ) )
   {
      FatalError( "failed to adjust window rect." );
      return 1;
   }

   clientPaddingRight = ( expectedWindowRect.right - expectedWindowRect.left ) - SCREEN_WIDTH;
   clientPaddingTop = ( expectedWindowRect.bottom - expectedWindowRect.top ) - SCREEN_HEIGHT;

   g_winGlobals.graphicsScale = DEFAULT_GRAPHICS_SCALE;

   // TODO: put the window title somewhere else
   g_winGlobals.hWndMain = CreateWindowExA( 0,
                                            mainWindowClass.lpszClassName,
                                            "Dragon Quest 3",
                                            windowStyle,
                                            CW_USEDEFAULT,
                                            CW_USEDEFAULT,
                                            (int)( SCREEN_WIDTH * g_winGlobals.graphicsScale ) + clientPaddingRight,
                                            (int)( SCREEN_HEIGHT * g_winGlobals.graphicsScale ) + clientPaddingTop,
                                            0,
                                            0,
                                            hInstance,
                                            0 );

   if ( !g_winGlobals.hWndMain )
   {
      FatalError( "failed to create main window." );
      return 1;
   }

   SetCursor( LoadCursor( 0, IDC_ARROW ) );

   g_winGlobals.hFont = CreateFontA( 16, 0, 0, 0,
                                     FW_BOLD, FALSE, FALSE, FALSE,
                                     ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                     DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS,
                                     "Consolas" );

   g_winGlobals.bmpInfo.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
   g_winGlobals.bmpInfo.bmiHeader.biWidth = g_winGlobals.game->pixelBuffer->w;
   g_winGlobals.bmpInfo.bmiHeader.biHeight = -(LONG)( g_winGlobals.game->pixelBuffer->h );
   g_winGlobals.bmpInfo.bmiHeader.biPlanes = 1;
   g_winGlobals.bmpInfo.bmiHeader.biBitCount = 32;
   g_winGlobals.bmpInfo.bmiHeader.biCompression = BI_RGB;

   Game_Run( g_winGlobals.game );

   MemArena_Destroy( &( memArena ) );
   return 0;
}

internal MemArena_t* CreateMemArena( void )
{
   MemArenaResult_t result;
   char msg[STRING_SIZE_DEFAULT];
   MemArena_t* memArena;

   memArena = 0;
   result = MemArena_Create( &( memArena ), GAME_MEMORY_SIZE );
   if ( result != MemArenaResult_Success )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "Failed to create memory arena for game object: %s", MemArena_GetErrorMessage( result ) );
      FatalError( msg );
   }

   return memArena;
}

internal LRESULT CALLBACK MainWindowProc( _In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam )
{
   LRESULT result = 0;

   switch ( uMsg )
   {
      case WM_QUIT:
      case WM_CLOSE:
      case WM_DESTROY:
         Game_Stop( g_winGlobals.game );
         break;
      case WM_KEYDOWN:
      case WM_KEYUP:
      case WM_SYSKEYDOWN:
      case WM_SYSKEYUP:
         HandleKeyboardInput( (u32)wParam, lParam );
         break;
      case WM_PAINT:
         RenderScreen();
         break;
      default:
         result = DefWindowProcA( hWnd, uMsg, wParam, lParam );
   }

   return result;
}

// the double-buffering part of this came from Stack Overflow
internal void RenderScreen( void )
{
   HDC dc, dcMem;
   HBITMAP bmMem;
   HANDLE hOld;
   PAINTSTRUCT ps;
   int winWidth, winHeight;

   winWidth = (int)( SCREEN_WIDTH * g_winGlobals.graphicsScale );
   winHeight = (int)( SCREEN_HEIGHT * g_winGlobals.graphicsScale );

   dc = BeginPaint( g_winGlobals.hWndMain, &ps );

   // create an off-screen DC for double-buffering
   dcMem = CreateCompatibleDC( dc );
   bmMem = CreateCompatibleBitmap( dc, winWidth, winHeight );
   hOld = SelectObject( dcMem, bmMem );

   // actually draw everything
   StretchDIBits( dcMem,
                  0, 0, winWidth, winHeight, // dest
                  0, 0, g_winGlobals.game->pixelBuffer->w, g_winGlobals.game->pixelBuffer->h, // src
                  g_winGlobals.game->pixelBuffer->mem,
                  &( g_winGlobals.bmpInfo ),
                  DIB_RGB_COLORS, SRCCOPY );

   // transfer the off-screen DC to the screen
   BitBlt( dc, 0, 0, winWidth, winHeight, dcMem, 0, 0, SRCCOPY );

   SelectObject( dcMem, hOld );
   DeleteObject( bmMem );
   DeleteDC( dcMem );
   EndPaint( g_winGlobals.hWndMain, &ps );
}

internal void HandleKeyboardInput( u32 keyCode, LPARAM flags )
{
   b32 keyWasDown, keyIsDown;

   keyWasDown = ( flags & ( (LONG_PTR)1 << 30 ) ) != 0 ? True : False;
   keyIsDown = ( flags & ( (LONG_PTR)1 << 31 ) ) == 0 ? True : False;

   // ignore repeat presses
   if ( keyWasDown != keyIsDown )
   {
      if ( keyIsDown )
      {
         // ensure alt+F4 still closes the window
         if ( keyCode == VK_F4 && ( flags & ( (LONG_PTR)1 << 29 ) ) )
         {
            Game_Stop( g_winGlobals.game );
            return;
         }

         // TODO
         // for ( i = 0; i < InputButton_Count; i++ )
         // {
         //    if ( g_winGlobals.buttonMap[i] == keyCode )
         //    {
         //       Input_ButtonPressed( &g_winGlobals.game.input, i );
         //       break;
         //    }
         // }
      }
      else
      {
         // TODO
         // for ( i = 0; i < InputButton_Count; i++ )
         // {
         //    if ( g_winGlobals.buttonMap[i] == keyCode )
         //    {
         //       Input_ButtonReleased( &g_winGlobals.game.input, i );
         //       break;
         //    }
         // }
      }
   }
}

void FatalError( const char* message )
{
   char errorMsg[STRING_SIZE_DEFAULT];
   snprintf( errorMsg, STRING_SIZE_DEFAULT, "Fatal error: %s", message );
   MessageBoxA( 0, message, "Error", MB_OK | MB_ICONERROR );
   exit( 1 );
}
