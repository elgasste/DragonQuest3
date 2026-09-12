#include "animation.h"
#include "display.h"
#include "entity.h"
#include "game.h"
#include "npc.h"
#include "platform.h"
#include "sprite.h"
#include "sprite_texture_set.h"
#include "tile_map.h"

internal void GameRender_DrawEntities( Game_t* game );
internal void GameRender_DrawEntity( Game_t* game, Entity_t* entity );
internal void GameRender_ApplyFadeOut( Game_t* game );
internal void GameRender_ApplyFadeIn( Game_t* game );
internal void GameRender_ApplyBlackout( Game_t* game );

void Game_Render( Game_t* game )
{
   Display_t* display;
   TileTextureSet_t* tileTextureSet;
   TileMap_t* tileMap;

   display = Game_GetDisplay( game );
   tileTextureSet = Game_GetTileTextureSet( game );
   tileMap = Game_GetTileMap( game );
   
   Display_Fill( display, 0 );

   // TODO: draw this in the correct place based on the game state
   Display_DrawTileMapViewport( display, tileMap, tileTextureSet, 0, 0 );

   GameRender_DrawEntities( game );

   if ( AnimationChain_GetIsRunning( Game_GetAnimationChain( game ) ) )
   {
      switch( AnimationChain_GetCurAnimationType( Game_GetAnimationChain( game ) ) )
      {
         case AnimationType_FadeOut:
            GameRender_ApplyFadeOut( game );
            break;
         case AnimationType_FadeIn:
            GameRender_ApplyFadeIn( game );
            break;
         case AnimationType_Blackout:
            GameRender_ApplyBlackout( game );
            break;
      }
   }

   Platform_RenderDisplayBuffer( display );
}

internal void GameRender_DrawEntities( Game_t* game )
{
   u32 i, drawOrder, npcCount;
   i32 selectedY, selectedOrder, lastY, lastOrder;
   b32 hasSelection, hasPrevious, playerDrawn;
   Vector4i32_t viewportInPixels, entityRect;
   TileMap_t* tileMap;
   Entity_t *playerEntity, *npcEntity, *selectedEntity;

   tileMap = Game_GetTileMap( game );
   viewportInPixels = TileMap_GetViewportInPixels( tileMap );
   playerEntity = Game_GetPlayerEntity( game );
   npcCount = TileMap_GetNpcCount( tileMap );
   playerDrawn = False;
   lastY = 0;
   selectedY = 0;
   hasPrevious = False;
   lastOrder = -1;

   for ( drawOrder = 0; drawOrder <= npcCount; drawOrder++ )
   {
      hasSelection = False;
      selectedEntity = 0;
      selectedOrder = 0;

      if ( !playerDrawn )
      {
         entityRect = Entity_GetRect( playerEntity );
         if ( !playerDrawn &&
              ( !hasPrevious || entityRect.y > lastY || ( entityRect.y == lastY && 0 > lastOrder ) ) &&
              ( !hasSelection || entityRect.y < selectedY || ( entityRect.y == selectedY && 0 < selectedOrder ) ) )
         {
            selectedEntity = playerEntity;
            selectedY = entityRect.y;
            selectedOrder = 0;
            hasSelection = True;
         }
      }

      for ( i = 0; i < npcCount; i++ )
      {
         npcEntity = Npc_GetEntity( TileMap_GetNpc( tileMap, i ) );
         entityRect = Entity_GetRect( npcEntity );
         
         if ( entityRect.x + entityRect.w > viewportInPixels.x * WORLD_UNITS_PER_PIXEL &&
              entityRect.x < ( viewportInPixels.x + viewportInPixels.w ) * WORLD_UNITS_PER_PIXEL &&
              entityRect.y + entityRect.h > viewportInPixels.y * WORLD_UNITS_PER_PIXEL &&
              entityRect.y < ( viewportInPixels.y + viewportInPixels.h ) * WORLD_UNITS_PER_PIXEL &&
              ( !hasPrevious || entityRect.y > lastY || ( entityRect.y == lastY && (i32)( i + 1 ) > lastOrder ) ) &&
              ( !hasSelection || entityRect.y < selectedY || ( entityRect.y == selectedY && (i32)( i + 1 ) < selectedOrder ) ) )
         {
            selectedEntity = npcEntity;
            selectedY = entityRect.y;
            selectedOrder = (i32)( i + 1 );
            hasSelection = True;
         }
      }

      if ( !hasSelection )
      {
         break;
      }

      GameRender_DrawEntity( game, selectedEntity );

      if ( selectedEntity == playerEntity )
      {
         playerDrawn = True;
      }

      lastY = selectedY;
      lastOrder = selectedOrder;
      hasPrevious = True;
   }
}

internal void GameRender_DrawEntity( Game_t* game, Entity_t* entity )
{
   i32 displayX, displayY;
   u32 frameCount, frameSize, textureIndex;
   u32* texture;
   ActiveSpriteTextureSet_t* textureSet;
   ActiveSprite_t* sprite;
   Vector4i32_t viewportInPixels;
   Vector4i32_t entityRect;
   Vector2i32_t spriteOffset;

   viewportInPixels = TileMap_GetViewportInPixels( Game_GetTileMap( game ) );
   entityRect = Entity_GetRect( entity );
   sprite = Entity_GetSprite( entity );
   textureSet = Game_GetActiveSpriteTextureSet( game );
   spriteOffset = Entity_GetSpriteOffset( entity );
   frameCount = ActiveSpriteTextureSet_GetFrameCount( textureSet );
   frameSize = ActiveSpriteTextureSet_GetFrameSize( textureSet );
   textureIndex = ( ActiveSprite_GetTextureIndex( sprite ) * Direction_Count * frameCount )
      + ( ActiveSprite_GetDirection( sprite ) * frameCount )
      + ActiveSprite_GetFrameIndex( sprite );
   texture = ActiveSpriteTextureSet_GetTexture( textureSet, textureIndex );

   displayX = ( entityRect.x / WORLD_UNITS_PER_PIXEL ) + spriteOffset.x - viewportInPixels.x;
   displayY = ( entityRect.y / WORLD_UNITS_PER_PIXEL ) + spriteOffset.y - viewportInPixels.y;
   Display_DrawBuffer( Game_GetDisplay( game ), texture, frameSize, frameSize, displayX, displayY );

#if defined( _WIN32 )
   if ( g_winDebugFlags.showHitBoxes )
   {
      displayX = ( entityRect.x / WORLD_UNITS_PER_PIXEL ) - viewportInPixels.x;
      displayY = ( entityRect.y / WORLD_UNITS_PER_PIXEL ) - viewportInPixels.y;
      Display_DrawRect( Game_GetDisplay( game ), displayX, displayY, entityRect.w / WORLD_UNITS_PER_PIXEL, entityRect.h / WORLD_UNITS_PER_PIXEL, 0x99FF0000 );
   }
#endif
}

internal void GameRender_ApplyFadeOut( Game_t* game )
{
   r32 progress;
   Animation_t* anim;

   anim = AnimationChain_GetCurAnimation( Game_GetAnimationChain( game ) );
   progress = Animation_GetElapsed( anim ) / Animation_GetDuration( anim );
   Display_ApplyFade( Game_GetDisplay( game ), progress );
}

internal void GameRender_ApplyFadeIn( Game_t* game )
{
   r32 progress;
   Animation_t* anim;

   anim = AnimationChain_GetCurAnimation( Game_GetAnimationChain( game ) );
   progress = Animation_GetElapsed( anim ) / Animation_GetDuration( anim );
   Display_ApplyFade( Game_GetDisplay( game ), 1.0f - progress );
}

internal void GameRender_ApplyBlackout( Game_t* game )
{
   Display_DrawRect( Game_GetDisplay( game ), 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0xFF000000 );
}
