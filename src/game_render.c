#include "display.h"
#include "entity.h"
#include "game.h"
#include "platform.h"
#include "sprite.h"
#include "sprite_texture_set.h"
#include "tile_map.h"

internal void GameRender_DrawPlayer( Game_t* game );

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

   GameRender_DrawPlayer( game );

   Platform_RenderDisplayBuffer( display );
}

internal void GameRender_DrawPlayer( Game_t* game )
{
   Display_t* display;
   TileMap_t* tileMap;
   ActiveSpriteTextureSet_t* textureSet;
   ActiveSprite_t* sprite;
   Vector4i32_t viewportInPixels;
   Vector4i32_t playerRect;
   Vector2i32_t spriteOffset;
   Entity_t* playerEntity;
   u32 frameCount;
   u32 frameSize;
   u32 textureIndex;
   u32* texture;

   display = Game_GetDisplay( game );
   tileMap = Game_GetTileMap( game );
   viewportInPixels = TileMap_GetViewportInPixels( tileMap );
   playerEntity = Game_GetPlayerEntity( game );
   playerRect = Entity_GetRect( playerEntity );
   sprite = Entity_GetSprite( playerEntity );
   textureSet = Game_GetActiveSpriteTextureSet( game );
   spriteOffset = Entity_GetSpriteOffset( playerEntity );
   frameCount = ActiveSpriteTextureSet_GetFrameCount( textureSet );
   frameSize = ActiveSpriteTextureSet_GetFrameSize( textureSet );
   textureIndex = ( ActiveSprite_GetTextureIndex( sprite ) * Direction_Count * frameCount )
      + ( ActiveSprite_GetDirection( sprite ) * frameCount )
      + ActiveSprite_GetFrameIndex( sprite );
   texture = ActiveSpriteTextureSet_GetTexture( textureSet, textureIndex );

   playerRect.x = ( playerRect.x / WORLD_UNITS_PER_PIXEL ) + spriteOffset.x - viewportInPixels.x;
   playerRect.y = ( playerRect.y / WORLD_UNITS_PER_PIXEL ) + spriteOffset.y - viewportInPixels.y;

   Display_DrawBuffer( display, texture, frameSize, frameSize, playerRect.x, playerRect.y );
}
