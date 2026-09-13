#include <stdio.h>

#include "clock.h"
#include "entity.h"
#include "game.h"
#include "input.h"
#include "tile_map.h"
#include "win_common.h"

#define IDC_DIAGNOSTICS_NOCLIP_BTN 1001
#define IDC_DIAGNOSTICS_HITBOXES_BTN 1002
#define IDC_DIAGNOSTICS_FASTMOVE_BTN 1003

internal HWND g_hWndNoClipBtn = NULL;
internal HWND g_hWndHitBoxesBtn = NULL;
internal HWND g_hWndFastMoveBtn = NULL;

internal LRESULT CALLBACK DiagnosticsWindowProc( _In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam );
internal void UpdateDiagnosticsText( HWND hWnd );

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
                                                   g_winGlobals.diagnosticsWindowClassName,
                                                   STR_DIAGNOSTICS_WINDOW_TITLE,
                                                   WS_OVERLAPPED | WS_CAPTION | WS_VISIBLE | WS_CLIPCHILDREN,
                                                   CW_USEDEFAULT,
                                                   CW_USEDEFAULT,
                                                   340,
                                                   372,
                                                   g_winGlobals.hWndMain,
                                                   0,
                                                   hInstance,
                                                   0 );

   if ( !g_winGlobals.hWndDiagnostics )
   {
      UnregisterClassA( windowClass.lpszClassName, hInstance );
      return False;
   }

   g_hWndNoClipBtn = CreateWindowExA( 0,
                                      "BUTTON",
                                      g_winDebugFlags.noClip ? "Disable No-Clip" : "Enable No-Clip",
                                      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                      10,
                                      234,
                                      180,
                                      26,
                                      g_winGlobals.hWndDiagnostics,
                                      (HMENU)(UINT_PTR)IDC_DIAGNOSTICS_NOCLIP_BTN,
                                      hInstance,
                                      0 );

   g_hWndHitBoxesBtn = CreateWindowExA( 0,
                                        "BUTTON",
                                        g_winDebugFlags.showHitBoxes ? "Hide Hit Boxes" : "Show Hit Boxes",
                                        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                        10,
                                        266,
                                        180,
                                        26,
                                        g_winGlobals.hWndDiagnostics,
                                        (HMENU)(UINT_PTR)IDC_DIAGNOSTICS_HITBOXES_BTN,
                                        hInstance,
                                        0 );

   g_hWndFastMoveBtn = CreateWindowExA( 0,
                                        "BUTTON",
                                        g_winDebugFlags.moveFast ? "Disable Fast Movement" : "Enable Fast Movement",
                                        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                        10,
                                        298,
                                        180,
                                        26,
                                        g_winGlobals.hWndDiagnostics,
                                        (HMENU)(UINT_PTR)IDC_DIAGNOSTICS_FASTMOVE_BTN,
                                        hInstance,
                                        0 );

   return True;
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
               case IDC_DIAGNOSTICS_NOCLIP_BTN:
                  TOGGLE_BOOL( g_winDebugFlags.noClip );
                  SetFocus( g_winGlobals.hWndMain );
                  return 0;

               case IDC_DIAGNOSTICS_HITBOXES_BTN:
                  TOGGLE_BOOL( g_winDebugFlags.showHitBoxes );
                  SetFocus( g_winGlobals.hWndMain );
                  return 0;

               case IDC_DIAGNOSTICS_FASTMOVE_BTN:
                  TOGGLE_BOOL( g_winDebugFlags.moveFast );
                  SetFocus( g_winGlobals.hWndMain );
                  return 0;
            }
         }
         break;
      case WM_ERASEBKGND:
         return 1;
      case WM_PAINT:
         UpdateDiagnosticsText( hWnd );
         return 0;

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

   BitBlt( dc, 0, 0, clientRect.right, clientRect.bottom, dcMem, 0, 0, SRCCOPY );

   SelectObject( dcMem, oldFont );
   SelectObject( dcMem, hOld );
   DeleteObject( bmMem );
   DeleteDC( dcMem );
   EndPaint( hWnd, &ps );
}
