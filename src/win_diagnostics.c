#include <stdio.h>

#include "clock.h"
#include "display.h"
#include "entity.h"
#include "game.h"
#include "input.h"
#include "tile_map.h"
#include "win_common.h"
#include <shellapi.h>

#define IDC_DIAGNOSTICS_RESETFLAGS_BTN 1000
#define IDC_DIAGNOSTICS_NOCLIP_BTN 1001
#define IDC_DIAGNOSTICS_HITBOXES_BTN 1002
#define IDC_DIAGNOSTICS_FASTMOVE_BTN 1003
#define IDC_DIAGNOSTICS_INCFPS_BTN 1004
#define IDC_DIAGNOSTICS_DECFPS_BTN 1005
#define IDC_DIAGNOSTICS_INCSCALE_BTN 1006
#define IDC_DIAGNOSTICS_DECSCALE_BTN 1007
#define IDC_DIAGNOSTICS_DUMPSTATS_BTN 1008
#define IDC_DIAGNOSTICS_CLEARLOG_BTN 1009
#define IDC_DIAGNOSTICS_OPENLOG_BTN 1010

internal HWND g_hWndResetFlagsBtn = NULL;
internal HWND g_hWndNoClipBtn = NULL;
internal HWND g_hWndHitBoxesBtn = NULL;
internal HWND g_hWndFastMoveBtn = NULL;
internal HWND g_hWndIncFpsBtn = NULL;
internal HWND g_hWndDecFpsBtn = NULL;
internal HWND g_hWndIncScaleBtn = NULL;
internal HWND g_hWndDecScaleBtn = NULL;
internal HWND g_hWndDumpStatsBtn = NULL;
internal HWND g_hWndClearLogBtn = NULL;
internal HWND g_hWndOpenLogBtn = NULL;

typedef struct WinDiagnosticsStatus_t
{
   char msg[STRING_SIZE_DEFAULT];
   u64 untilMicro;
}
WinDiagnosticsStatus_t;

internal WinDiagnosticsStatus_t g_diagStatus = { 0 };

internal LRESULT CALLBACK DiagnosticsWindowProc( _In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam );
internal void UpdateDiagnosticsText( HWND hWnd );
internal void ChangeGameFps( b32 increase );
internal void ResizeScreen( b32 increase );

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

   g_winGlobals.movingDiagnosticsWindow = True;
   g_winGlobals.hWndDiagnostics = CreateWindowExA( WS_EX_TOOLWINDOW,
                                                   g_winGlobals.diagnosticsWindowClassName,
                                                   STR_WIN_DIAGNOSTICS_WINDOW_TITLE,
                                                   WS_OVERLAPPED | WS_CAPTION | WS_CLIPCHILDREN,
                                                   CW_USEDEFAULT,
                                                   CW_USEDEFAULT,
                                                   336,
                                                   540,
                                                   g_winGlobals.hWndMain,
                                                   0,
                                                   hInstance,
                                                   0 );
   g_winGlobals.movingDiagnosticsWindow = False;

   if ( !g_winGlobals.hWndDiagnostics )
   {
      UnregisterClassA( windowClass.lpszClassName, hInstance );
      return False;
   }

   g_hWndIncFpsBtn = CreateWindowExA( 0,
                                      "BUTTON",
                                      "+",
                                      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                      200,
                                      7,
                                      24,
                                      20,
                                      g_winGlobals.hWndDiagnostics,
                                      (HMENU)(UINT_PTR)IDC_DIAGNOSTICS_INCFPS_BTN,
                                      hInstance,
                                      0 );

   g_hWndDecFpsBtn = CreateWindowExA( 0,
                                      "BUTTON",
                                      "-",
                                      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                      228,
                                      7,
                                      24,
                                      20,
                                      g_winGlobals.hWndDiagnostics,
                                      (HMENU)(UINT_PTR)IDC_DIAGNOSTICS_DECFPS_BTN,
                                      hInstance,
                                      0 );

   g_hWndIncScaleBtn = CreateWindowExA( 0,
                                        "BUTTON",
                                        "+",
                                        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                        200,
                                        71,
                                        24,
                                        20,
                                        g_winGlobals.hWndDiagnostics,
                                        (HMENU)(UINT_PTR)IDC_DIAGNOSTICS_INCSCALE_BTN,
                                        hInstance,
                                        0 );

   g_hWndDecScaleBtn = CreateWindowExA( 0,
                                        "BUTTON",
                                        "-",
                                        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                        228,
                                        71,
                                        24,
                                        20,
                                        g_winGlobals.hWndDiagnostics,
                                        (HMENU)(UINT_PTR)IDC_DIAGNOSTICS_DECSCALE_BTN,
                                        hInstance,
                                        0 );

   g_hWndResetFlagsBtn = CreateWindowExA( 0,
                                          "BUTTON",
                                          "Reset Debug Flags",
                                          WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                          10,
                                          234,
                                          180,
                                          26,
                                          g_winGlobals.hWndDiagnostics,
                                          (HMENU)(UINT_PTR)IDC_DIAGNOSTICS_RESETFLAGS_BTN,
                                          hInstance,
                                          0 );

   g_hWndNoClipBtn = CreateWindowExA( 0,
                                      "BUTTON",
                                      g_winDebugFlags.noClip ? "Disable No-Clip" : "Enable No-Clip",
                                      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                      10,
                                      266,
                                      180,
                                      26,
                                      g_winGlobals.hWndDiagnostics,
                                      (HMENU)(UINT_PTR)IDC_DIAGNOSTICS_NOCLIP_BTN,
                                      hInstance,
                                      0 );

   g_hWndHitBoxesBtn = CreateWindowExA( 0,
                                        "BUTTON",
                                        g_winDebugFlags.showHitBoxes ? "Hide Hit Boxes" : "Show Hit Boxes",
                                        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                        10,
                                        298,
                                        180,
                                        26,
                                        g_winGlobals.hWndDiagnostics,
                                        (HMENU)(UINT_PTR)IDC_DIAGNOSTICS_HITBOXES_BTN,
                                        hInstance,
                                        0 );

   g_hWndFastMoveBtn = CreateWindowExA( 0,
                                        "BUTTON",
                                        g_winDebugFlags.moveFast ? "Disable Fast Movement" : "Enable Fast Movement",
                                        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                        10,
                                        330,
                                        180,
                                        26,
                                        g_winGlobals.hWndDiagnostics,
                                        (HMENU)(UINT_PTR)IDC_DIAGNOSTICS_FASTMOVE_BTN,
                                        hInstance,
                                        0 );

   g_hWndDumpStatsBtn = CreateWindowExA( 0,
                                         "BUTTON",
                                         "Dump Memory Stats",
                                         WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                         10,
                                         378,
                                         180,
                                         26,
                                         g_winGlobals.hWndDiagnostics,
                                         (HMENU)(UINT_PTR)IDC_DIAGNOSTICS_DUMPSTATS_BTN,
                                         hInstance,
                                         0 );

   g_hWndClearLogBtn = CreateWindowExA( 0,
                                        "BUTTON",
                                        "Clear Log File",
                                        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                        224,
                                        448,
                                        90,
                                        26,
                                        g_winGlobals.hWndDiagnostics,
                                        (HMENU)(UINT_PTR)IDC_DIAGNOSTICS_CLEARLOG_BTN,
                                        hInstance,
                                        0 );

   g_hWndOpenLogBtn = CreateWindowExA( 0,
                                       "BUTTON",
                                       "Open Log File",
                                       WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                       224,
                                       416,
                                       90,
                                       26,
                                       g_winGlobals.hWndDiagnostics,
                                       (HMENU)(UINT_PTR)IDC_DIAGNOSTICS_OPENLOG_BTN,
                                       hInstance,
                                       0 );

   return True;
}

void SetDiagnosticsStatus( const char* msg )
{
   strcpy_s( g_diagStatus.msg, STRING_SIZE_DEFAULT, msg );
   g_diagStatus.untilMicro = Platform_GetMicros() + ( 3 * 1000000 );
}

internal LRESULT CALLBACK DiagnosticsWindowProc( _In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam )
{
   switch ( uMsg )
   {
      case WM_CLOSE:
         // this window should stay open for the duration of the app
         return 0;
      case WM_COMMAND:
         if ( HIWORD( wParam ) == BN_CLICKED )
         {
            switch ( LOWORD( wParam ) )
            {
               case IDC_DIAGNOSTICS_INCFPS_BTN:
                  ChangeGameFps( True );
                  SetDiagnosticsStatus( "FPS increased" );
                  SetFocus( g_winGlobals.hWndMain );
                  return 0;

               case IDC_DIAGNOSTICS_DECFPS_BTN:
                  ChangeGameFps( False );
                  SetDiagnosticsStatus( "FPS decreased" );
                  SetFocus( g_winGlobals.hWndMain );
                  return 0;

               case IDC_DIAGNOSTICS_INCSCALE_BTN:
                  ResizeScreen( True );
                  SetDiagnosticsStatus( "Graphics scaled up" );
                  SetFocus( g_winGlobals.hWndMain );
                  return 0;

               case IDC_DIAGNOSTICS_DECSCALE_BTN:
                  ResizeScreen( False );
                  SetDiagnosticsStatus( "Graphics scaled down" );
                  SetFocus( g_winGlobals.hWndMain );
                  return 0;

               case IDC_DIAGNOSTICS_RESETFLAGS_BTN:
                  g_winDebugFlags.noClip = False;
                  g_winDebugFlags.showHitBoxes = False;
                  g_winDebugFlags.moveFast = False;
                  SetDiagnosticsStatus( "Debug flags reset" );
                  SetFocus( g_winGlobals.hWndMain );
                  return 0;

               case IDC_DIAGNOSTICS_NOCLIP_BTN:
                  TOGGLE_BOOL( g_winDebugFlags.noClip );
                  SetDiagnosticsStatus( g_winDebugFlags.noClip ? STR_WIN_DIAGNOSTICS_NOCLIP_ENABLED : STR_WIN_DIAGNOSTICS_NOCLIP_DISABLED );
                  SetFocus( g_winGlobals.hWndMain );
                  return 0;

               case IDC_DIAGNOSTICS_HITBOXES_BTN:
                  TOGGLE_BOOL( g_winDebugFlags.showHitBoxes );
                  SetDiagnosticsStatus( g_winDebugFlags.showHitBoxes ? STR_WIN_DIAGNOSTICS_HITBOXES_ENABLED : STR_WIN_DIAGNOSTICS_HITBOXES_DISABLED );
                  SetFocus( g_winGlobals.hWndMain );
                  return 0;

               case IDC_DIAGNOSTICS_FASTMOVE_BTN:
                  TOGGLE_BOOL( g_winDebugFlags.moveFast );
                  SetDiagnosticsStatus( g_winDebugFlags.moveFast ? STR_WIN_DIAGNOSTICS_FASTMOVE_ENABLED : STR_WIN_DIAGNOSTICS_FASTMOVE_DISABLED );
                  SetFocus( g_winGlobals.hWndMain );
                  return 0;

               case IDC_DIAGNOSTICS_DUMPSTATS_BTN:
                  MemArena_DumpStats( g_winGlobals.memArena );
                  SetDiagnosticsStatus( "Memory stats dumped to log file" );
                  SetFocus( g_winGlobals.hWndMain );
                  return 0;

               case IDC_DIAGNOSTICS_CLEARLOG_BTN:
               {
                  HANDLE logFile = CreateFileA( g_winGlobals.logFilePath,
                                                GENERIC_WRITE,
                                                FILE_SHARE_READ,
                                                NULL,
                                                CREATE_ALWAYS,
                                                FILE_ATTRIBUTE_NORMAL,
                                                NULL );
                  if ( logFile == INVALID_HANDLE_VALUE )
                  {
                     SetDiagnosticsStatus( "Failed to clear log file" );
                  }
                  else
                  {
                     CloseHandle( logFile );
                     SetDiagnosticsStatus( "Log file cleared" );
                  }
                  SetFocus( g_winGlobals.hWndMain );
                  return 0;
               }

               case IDC_DIAGNOSTICS_OPENLOG_BTN:
                  ShellExecuteA( hWnd, "open", g_winGlobals.logFilePath, NULL, NULL, SW_SHOWNORMAL );
                  SetDiagnosticsStatus( "Opening log file" );
                  SetFocus( g_winGlobals.hWndMain );
                  return 0;
            }
         }
         break;
      case WM_DRAWITEM:
      {
         LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;
         if ( dis->CtlType == ODT_BUTTON )
         {
            b32 isEnabled = False;
            char text[STRING_SIZE_DEFAULT];
            RECT btnRect = dis->rcItem;
            UINT state = DFCS_BUTTONPUSH;
            HFONT hFont, hOldFont;

            switch ( dis->CtlID )
            {
               case IDC_DIAGNOSTICS_RESETFLAGS_BTN:
               case IDC_DIAGNOSTICS_INCFPS_BTN:
               case IDC_DIAGNOSTICS_DECFPS_BTN:
               case IDC_DIAGNOSTICS_INCSCALE_BTN:
               case IDC_DIAGNOSTICS_DECSCALE_BTN:
               case IDC_DIAGNOSTICS_DUMPSTATS_BTN:
               case IDC_DIAGNOSTICS_CLEARLOG_BTN:
               case IDC_DIAGNOSTICS_OPENLOG_BTN:
                  isEnabled = False;
                  break;
               case IDC_DIAGNOSTICS_NOCLIP_BTN:
                  isEnabled = g_winDebugFlags.noClip;
                  break;
               case IDC_DIAGNOSTICS_HITBOXES_BTN:
                  isEnabled = g_winDebugFlags.showHitBoxes;
                  break;
               case IDC_DIAGNOSTICS_FASTMOVE_BTN:
                  isEnabled = g_winDebugFlags.moveFast;
                  break;
            }

            if ( dis->itemState & ODS_SELECTED )
            {
               state |= DFCS_PUSHED;
            }

            DrawFrameControl( dis->hDC, &btnRect, DFC_BUTTON, state );

            SetTextColor( dis->hDC, isEnabled ? RGB( 0, 140, 0 ) : RGB( 40, 40, 40 ) );
            SetBkMode( dis->hDC, TRANSPARENT );

            GetWindowTextA( dis->hwndItem, text, sizeof( text ) );

            if ( dis->itemState & ODS_SELECTED )
            {
               OffsetRect( &btnRect, 1, 1 );
            }

            hFont = (HFONT)GetStockObject( DEFAULT_GUI_FONT );
            hOldFont = (HFONT)SelectObject( dis->hDC, hFont );
            DrawTextA( dis->hDC, text, -1, &btnRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
            if ( hOldFont )
            {
               SelectObject( dis->hDC, hOldFont );
            }

            if ( dis->itemState & ODS_FOCUS )
            {
               RECT focusRect = dis->rcItem;
               InflateRect( &focusRect, -3, -3 );
               DrawFocusRect( dis->hDC, &focusRect );
            }

            return TRUE;
         }
         break;
      }
      case WM_ERASEBKGND:
         return 1;
      case WM_PAINT:
         UpdateDiagnosticsText( hWnd );
         return 0;
      case WM_MOVE:
         if ( !g_winGlobals.movingDiagnosticsWindow )
         {
            g_winGlobals.anchorDiagnosticsWindow = False;
         }
         return DefWindowProcA( hWnd, uMsg, wParam, lParam );

      default:
         return DefWindowProcA( hWnd, uMsg, wParam, lParam );
   }

   return 0;
}

internal void UpdateDiagnosticsText( HWND hWnd )
{
   PAINTSTRUCT ps;
   HDC dc, dcMem;
   HBITMAP bmMem;
   HANDLE hOld;
   RECT r, clientRect;
   HFONT oldFont;
   Game_t* game;
   Clock_t* clock;
   Input_t* input;
   Entity_t* playerEntity;
   Vector4i32_t playerRect;
   u32 gameSeconds, realSeconds, playerTileIndex, playerTileX, playerTileY;
   char str[STRING_SIZE_DEFAULT];

   dc = BeginPaint( hWnd, &ps );
   game = g_winGlobals.game;
   clock = Game_GetClock( game );
   input = Game_GetInput( game );
   playerEntity = Game_GetPlayerEntity( game );
   playerRect = Entity_GetRect( playerEntity );

   GetClientRect( hWnd, &clientRect );

   dcMem = CreateCompatibleDC( dc );
   bmMem = CreateCompatibleBitmap( dc, clientRect.right, clientRect.bottom );
   hOld = SelectObject( dcMem, bmMem );

   FillRect( dcMem, &clientRect, (HBRUSH)GetStockObject( BLACK_BRUSH ) );

   r.left = 10;
   r.top = 10;
   r.right = 0;
   r.bottom = 0;

   oldFont = (HFONT)SelectObject( dcMem, g_winGlobals.hFont );

   SetTextColor( dcMem, RGB( 255, 255, 255 ) );
   SetBkColor( dcMem, RGB( 0, 0, 0 ) );
   SetBkMode( dcMem, TRANSPARENT );

   sprintf_s( str, STRING_SIZE_DEFAULT, "Target Frame Rate: %u", Clock_GetFps( clock ) );
   DrawTextA( dcMem, str, -1, &r, DT_SINGLELINE | DT_NOCLIP );
   r.top += 16;

   sprintf_s( str, STRING_SIZE_DEFAULT, "    Last Frame MS: %u", (u32)( Clock_GetLastFrameMicro( clock ) / 1000 ) );
   DrawTextA( dcMem, str, -1, &r, DT_SINGLELINE | DT_NOCLIP );
   r.top += 16;

   sprintf_s( str, STRING_SIZE_DEFAULT, "     Total Frames: %u", Clock_GetFrameCount( clock ) );
   DrawTextA( dcMem, str, -1, &r, DT_SINGLELINE | DT_NOCLIP );
   r.top += 16;

   sprintf_s( str, STRING_SIZE_DEFAULT, "       Lag Frames: %u", Clock_GetLagFrameCount( clock ) );
   DrawTextA( dcMem, str, -1, &r, DT_SINGLELINE | DT_NOCLIP );
   r.top += 16;

   sprintf_s( str, STRING_SIZE_DEFAULT, "   Graphics Scale: %.1f", g_winGlobals.graphicsScale );
   DrawTextA( dcMem, str, -1, &r, DT_SINGLELINE | DT_NOCLIP );
   r.top += 16;

   gameSeconds = Clock_GetFrameCount( clock ) / Clock_GetFps( clock );
   sprintf_s( str, STRING_SIZE_DEFAULT, "    In-Game Timer: %u:%02u:%02u", gameSeconds / 3600, gameSeconds / 60, gameSeconds );
   DrawTextA( dcMem, str, -1, &r, DT_SINGLELINE | DT_NOCLIP );
   r.top += 16;

   realSeconds = (u32)( Clock_GetAbsoluteEndMicro( clock ) - Clock_GetAbsoluteStartMicro( clock ) ) / 1000000;
   sprintf_s( str, STRING_SIZE_DEFAULT, " Real World Timer: %u:%02u:%02u", realSeconds / 3600, realSeconds / 60, realSeconds );
   DrawTextA( dcMem, str, -1, &r, DT_SINGLELINE | DT_NOCLIP );
   r.top += 16;

   sprintf_s( str, STRING_SIZE_DEFAULT, "  Player Position: (%d, %d)", playerRect.x, playerRect.y );
   DrawTextA( dcMem, str, -1, &r, DT_SINGLELINE | DT_NOCLIP );
   r.top += 16;

   playerTileIndex = Entity_GetTileIndex( Game_GetPlayerEntity( game ) );
   playerTileX = playerTileIndex % TileMap_GetTilesX( Game_GetTileMap( game ) );
   playerTileY = playerTileIndex / TileMap_GetTilesX( Game_GetTileMap( game ) );
   sprintf_s( str, STRING_SIZE_DEFAULT, "Player Tile Index: %u (%u, %u)", playerTileIndex, playerTileX, playerTileY );
   DrawTextA( dcMem, str, -1, &r, DT_SINGLELINE | DT_NOCLIP );
   r.top += 16;

   r.top += 16;
   r.left = ( clientRect.right - 150 ) / 2;

   sprintf_s( str, STRING_SIZE_DEFAULT, "  |" );
   SetTextColor( dcMem, Input_GetButtonState( input, InputButton_Up )->down ? 0x00FFFFFF : 0x00777777 );
   DrawTextA( dcMem, str, -1, &r, DT_SINGLELINE | DT_NOCLIP );
   r.top += 16;

   sprintf_s( str, STRING_SIZE_DEFAULT, "--" );
   SetTextColor( dcMem, Input_GetButtonState( input, InputButton_Left )->down ? 0x00FFFFFF : 0x00777777 );
   DrawTextA( dcMem, str, -1, &r, DT_SINGLELINE | DT_NOCLIP );

   sprintf_s( str, STRING_SIZE_DEFAULT, "   --" );
   SetTextColor( dcMem, Input_GetButtonState( input, InputButton_Right )->down ? 0x00FFFFFF : 0x00777777 );
   DrawTextA( dcMem, str, -1, &r, DT_SINGLELINE | DT_NOCLIP );

   sprintf_s( str, STRING_SIZE_DEFAULT, "      SEL" );
   SetTextColor( dcMem, Input_GetButtonState( input, InputButton_Select )->down ? 0x00FFFFFF : 0x00777777 );
   DrawTextA( dcMem, str, -1, &r, DT_SINGLELINE | DT_NOCLIP );

   sprintf_s( str, STRING_SIZE_DEFAULT, "          STA" );
   SetTextColor( dcMem, Input_GetButtonState( input, InputButton_Start )->down ? 0x00FFFFFF : 0x00777777 );
   DrawTextA( dcMem, str, -1, &r, DT_SINGLELINE | DT_NOCLIP );

   sprintf_s( str, STRING_SIZE_DEFAULT, "              B" );
   SetTextColor( dcMem, Input_GetButtonState( input, InputButton_B )->down ? 0x00FFFFFF : 0x00777777 );
   DrawTextA( dcMem, str, -1, &r, DT_SINGLELINE | DT_NOCLIP );

   sprintf_s( str, STRING_SIZE_DEFAULT, "                A" );
   SetTextColor( dcMem, Input_GetButtonState( input, InputButton_A )->down ? 0x00FFFFFF : 0x00777777 );
   DrawTextA( dcMem, str, -1, &r, DT_SINGLELINE | DT_NOCLIP );
   r.top += 16;

   sprintf_s( str, STRING_SIZE_DEFAULT, "  |" );
   SetTextColor( dcMem, Input_GetButtonState( input, InputButton_Down )->down ? 0x00FFFFFF : 0x00777777 );
   DrawTextA( dcMem, str, -1, &r, DT_SINGLELINE | DT_NOCLIP );
   r.top += 16;

   {
      static b32 s_lastNoClip = (b32)-1;
      if ( g_hWndNoClipBtn && s_lastNoClip != g_winDebugFlags.noClip )
      {
         s_lastNoClip = g_winDebugFlags.noClip;
         SetWindowTextA( g_hWndNoClipBtn, g_winDebugFlags.noClip ? "Disable No-Clip" : "Enable No-Clip" );
      }

      static b32 s_lastShowHitBoxes = (b32)-1;
      if ( g_hWndHitBoxesBtn && s_lastShowHitBoxes != g_winDebugFlags.showHitBoxes )
      {
         s_lastShowHitBoxes = g_winDebugFlags.showHitBoxes;
         SetWindowTextA( g_hWndHitBoxesBtn, g_winDebugFlags.showHitBoxes ? "Hide Hit Boxes" : "Show Hit Boxes" );
      }

      static b32 s_lastMoveFast = (b32)-1;
      if ( g_hWndFastMoveBtn && s_lastMoveFast != g_winDebugFlags.moveFast )
      {
         s_lastMoveFast = g_winDebugFlags.moveFast;
         SetWindowTextA( g_hWndFastMoveBtn, g_winDebugFlags.moveFast ? "Disable Fast Movement" : "Enable Fast Movement" );
      }
   }

   {
      RECT statusRect;
      int statusBarHeight = 22;
      statusRect.left = 0;
      statusRect.top = clientRect.bottom - statusBarHeight;
      statusRect.right = clientRect.right;
      statusRect.bottom = clientRect.bottom;

      FillRect( dcMem, &statusRect, (HBRUSH)GetStockObject( DKGRAY_BRUSH ) );
      
      HPEN pen = CreatePen( PS_SOLID, 1, RGB( 80, 80, 80 ) );
      HPEN oldPen = (HPEN)SelectObject( dcMem, pen );
      MoveToEx( dcMem, statusRect.left, statusRect.top, NULL );
      LineTo( dcMem, statusRect.right, statusRect.top );
      SelectObject( dcMem, oldPen );
      DeleteObject( pen );

      if ( g_diagStatus.untilMicro > 0 )
      {
         if ( Platform_GetMicros() < g_diagStatus.untilMicro )
         {
            RECT textRect = statusRect;
            textRect.left += 6;
            textRect.top += 3;
            SetTextColor( dcMem, RGB( 255, 255, 255 ) );
            SetBkMode( dcMem, TRANSPARENT );
            DrawTextA( dcMem, g_diagStatus.msg, -1, &textRect, DT_SINGLELINE | DT_VCENTER );
         }
         else
         {
            g_diagStatus.untilMicro = 0;
            g_diagStatus.msg[0] = '\0';
         }
      }
   }

   BitBlt( dc, 0, 0, clientRect.right, clientRect.bottom, dcMem, 0, 0, SRCCOPY );

   SelectObject( dcMem, oldFont );
   SelectObject( dcMem, hOld );
   DeleteObject( bmMem );
   DeleteDC( dcMem );
   EndPaint( hWnd, &ps );
}

internal void ChangeGameFps( b32 increase )
{
   u32 fps;
   Clock_t* clock;

   clock = Game_GetClock( g_winGlobals.game );
   fps = Clock_GetFps( clock );

   if ( increase && fps < MAX_GAME_FPS )
   {
      Clock_SetFps( clock, fps + GAME_FPS_STEP );
      SaveWinDebugConfig( fps + GAME_FPS_STEP );
   }
   else if ( !increase && fps > MIN_GAME_FPS )
   {
      Clock_SetFps( clock, fps - GAME_FPS_STEP );
      SaveWinDebugConfig( fps - GAME_FPS_STEP );
   }
}

internal void ResizeScreen( b32 increase )
{
   b32 changed;
   RECT mainWindowRect;
   int newWidth, newHeight;

   changed = False;
   if ( increase && g_winGlobals.graphicsScale < MAX_GRAPHICS_SCALE )
   {
      g_winGlobals.graphicsScale += GRAPHICS_SCALE_STEP;
      changed = True;
   }
   else if ( !increase && g_winGlobals.graphicsScale > MIN_GRAPHICS_SCALE )
   {
      g_winGlobals.graphicsScale -= GRAPHICS_SCALE_STEP;
      changed = True;
   }

   if ( changed )
   {
      g_winGlobals.anchorDiagnosticsWindow = True;
      newWidth = (int)( DISPLAY_WIDTH * g_winGlobals.graphicsScale ) + g_winGlobals.clientPaddingRight;
      newHeight = (int)( DISPLAY_HEIGHT * g_winGlobals.graphicsScale ) + g_winGlobals.clientPaddingTop;

      SetWindowPos( g_winGlobals.hWndMain,
                    NULL, // No change in Z-order
                    0, 0, // No change in position
                    newWidth, 
                    newHeight,
                    SWP_NOMOVE | SWP_NOZORDER );

      if ( GetWindowRect( g_winGlobals.hWndMain, &mainWindowRect ) )
      {
         g_winGlobals.movingDiagnosticsWindow = True;
         SetWindowPos( g_winGlobals.hWndDiagnostics,
                       HWND_TOP,
                       mainWindowRect.left + newWidth + 16,
                       mainWindowRect.top,
                       0,
                       0,
                       SWP_NOSIZE | SWP_NOACTIVATE );
         g_winGlobals.movingDiagnosticsWindow = False;
      }

      SaveWinDebugConfig( Clock_GetFps( Game_GetClock( g_winGlobals.game ) ) );
   }
}
