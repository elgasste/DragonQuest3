#include "win_common.h"

internal LRESULT CALLBACK DiagnosticsWindowProc( _In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam );

b32 CreateDiagnosticsWindow( HINSTANCE hInstance )
{
   WNDCLASSA windowClass;

   strcpy_s( g_winGlobals.diagnosticsWindowClassName, MAX_PATH, "diagnosticsWindowClass" );
   windowClass.cbClsExtra = 0;
   windowClass.cbWndExtra = 0;
   windowClass.hbrBackground = 0;
   windowClass.hCursor = 0;
   windowClass.hIcon = 0;
   windowClass.lpszMenuName = 0;

   windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
   windowClass.lpfnWndProc = DiagnosticsWindowProc;
   windowClass.hInstance = hInstance;
   windowClass.lpszClassName = g_winGlobals.diagnosticsWindowClassName;

   if ( !RegisterClassA( &windowClass ) )
   {
      return False;
   }

   g_winGlobals.hWndDiagnostics = CreateWindowExA( WS_EX_TOOLWINDOW,
                                                   windowClass.lpszClassName,
                                                   STR_DIAGNOSTICS_WINDOW_TITLE,
                                                   WS_OVERLAPPED | WS_CAPTION | WS_VISIBLE,
                                                   CW_USEDEFAULT,
                                                   CW_USEDEFAULT,
                                                   300,
                                                   300,
                                                   g_winGlobals.hWndMain,
                                                   0,
                                                   hInstance,
                                                   0 );

   if ( !g_winGlobals.hWndDiagnostics )
   {
      UnregisterClassA( windowClass.lpszClassName, hInstance );
      return False;
   }

   return True;
}

internal LRESULT CALLBACK DiagnosticsWindowProc( _In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam )
{
   LRESULT result;

   result = 0;
   switch ( uMsg )
   {
      case WM_CLOSE:
         // this window should stay open for the duration of the app
         result = 0;
         break;
      default:
         result = DefWindowProcA( hWnd, uMsg, wParam, lParam );
   }

   return result;
}