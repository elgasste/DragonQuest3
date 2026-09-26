#include "animation.h"
#include "display.h"
#include "entity.h"
#include "game.h"
#include "npc.h"
#include "platform.h"
#include "player.h"
#include "sprite.h"
#include "sprite_texture_set.h"
#include "tile_map.h"

internal void GameRender_DrawEntities( Game_t* game );
internal void GameRender_DrawEntity( Game_t* game, Entity_t* entity );
internal i32 GameRender_GetEntityDrawOrder( Game_t* game, u32 entityIndex, u32 playerCount );
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
   u32 i, drawOrder, playerCount, npcCount, entityCount;
   i32 selectedY, selectedOrder, lastY, lastOrder, entityDrawOrder;
   b32 hasSelection, hasPrevious;
   Vector4i32_t viewportInPixels, entityRect;
   Vector2i32_t spriteOffset;
   TileMap_t* tileMap;
   Entity_t *entity, *selectedEntity;

   tileMap = Game_GetTileMap( game );
   viewportInPixels = TileMap_GetViewportInPixels( tileMap );
   playerCount = Game_GetPlayerCount( game );
   npcCount = TileMap_GetNpcCount( tileMap );
   entityCount = playerCount + npcCount;
   lastY = 0;
   hasPrevious = False;
   lastOrder = -1;

   for ( drawOrder = 0; drawOrder < entityCount; drawOrder++ )
   {
      hasSelection = False;
      selectedEntity = 0;
      selectedY = 0;
      selectedOrder = 0;

      for ( i = 0; i < entityCount; i++ )
      {
         entityDrawOrder = GameRender_GetEntityDrawOrder( game, i, playerCount );

         if ( i < playerCount )
         {
            entity = Game_GetPlayerEntity( game, i );
         }
         else
         {
            entity = Npc_GetEntity( TileMap_GetNpc( tileMap, i - playerCount ) );
         }

         entityRect = Entity_GetRect( entity );
         spriteOffset = Entity_GetSpriteOffset( entity );

         if ( entityRect.x + entityRect.w + ( spriteOffset.x * WORLD_UNITS_PER_PIXEL ) > viewportInPixels.x * WORLD_UNITS_PER_PIXEL &&
              entityRect.x + ( spriteOffset.x * WORLD_UNITS_PER_PIXEL ) < ( viewportInPixels.x + viewportInPixels.w ) * WORLD_UNITS_PER_PIXEL &&
              entityRect.y + entityRect.h + ( spriteOffset.y * WORLD_UNITS_PER_PIXEL ) > viewportInPixels.y * WORLD_UNITS_PER_PIXEL &&
              entityRect.y + ( spriteOffset.y * WORLD_UNITS_PER_PIXEL ) < ( viewportInPixels.y + viewportInPixels.h ) * WORLD_UNITS_PER_PIXEL &&
                     ( !hasPrevious || entityRect.y > lastY || ( entityRect.y == lastY && entityDrawOrder > lastOrder ) ) &&
                     ( !hasSelection || entityRect.y < selectedY || ( entityRect.y == selectedY && entityDrawOrder < selectedOrder ) ) )
         {
            selectedEntity = entity;
            selectedY = entityRect.y;
                  selectedOrder = entityDrawOrder;
            hasSelection = True;
         }
      }

      if ( !hasSelection )
      {
         break;
      }

      GameRender_DrawEntity( game, selectedEntity );

      lastY = selectedY;
      lastOrder = selectedOrder;
      hasPrevious = True;
   }
}

internal i32 GameRender_GetEntityDrawOrder( Game_t* game, u32 entityIndex, u32 playerCount )
{
   u32 i;
   u32* playerOrder;

   if ( entityIndex >= playerCount )
   {
      // we add one here to make sure the NPC with the index "playerCount" doesn't get skipped
      return (i32)( entityIndex + 1 );
   }

   playerOrder = Game_GetPlayerOrder( game );
   for ( i = 0; i < playerCount; i++ )
   {
      if ( playerOrder[i] == entityIndex )
      {
         return (i32)( playerCount - i );
      }
   }

   return (i32)entityIndex;
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
