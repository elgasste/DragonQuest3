#include "entity.h"
#include "game.h"
#include "tile_map.h"
#include "tile_texture_set.h"

void Game_TicPhysics( Game_t* game )
{
   i32 mapWidth, mapHeight, maxX, maxY;
   Vector4i32_t playerRect;
   Vector2i32_t playerVelocity;
   TileMap_t* tileMap;
   TileTextureSet_t* tileTextureSet;
   Entity_t* playerEntity;

   playerEntity = Game_GetPlayerEntity( game );
   tileMap = Game_GetTileMap( game );
   tileTextureSet = Game_GetTileTextureSet( game );
   playerRect = Entity_GetRect( playerEntity );
   playerVelocity = Entity_GetVelocity( playerEntity );

   playerRect.x += playerVelocity.x;
   playerRect.y += playerVelocity.y;

   if ( !TileMap_GetWraps( tileMap ) )
	{
		mapWidth = (i32)( TileMap_GetTilesX( tileMap ) * TileTextureSet_GetTileSize( tileTextureSet ) ) * WORLD_UNITS_PER_PIXEL;
		mapHeight = (i32)( TileMap_GetTilesY( tileMap ) * TileTextureSet_GetTileSize( tileTextureSet ) ) * WORLD_UNITS_PER_PIXEL;
		maxX = mapWidth - playerRect.w;
		maxY = mapHeight - playerRect.h;

		if ( maxX < 0 )
      {
			maxX = 0;
      }
		if ( maxY < 0 )
      {
			maxY = 0;
      }

		if ( playerRect.x < 0 )
      {
			playerRect.x = 0;
      }
		else if ( playerRect.x > maxX )
      {
			playerRect.x = maxX;
      }

		if ( playerRect.y < 0 )
      {
			playerRect.y = 0;
      }
		else if ( playerRect.y > maxY )
      {
			playerRect.y = maxY;
      }
	}

	Entity_SetPosition( playerEntity, playerRect.x, playerRect.y );
    Entity_SetVelocity( playerEntity, 0, 0 );
}
