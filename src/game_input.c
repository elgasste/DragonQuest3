#include "clock.h"
#include "entity.h"
#include "game.h"
#include "input.h"
#include "tile_map.h"
#include "tile_texture_set.h"

void Game_HandleInput( Game_t* game )
{
   b32 leftIsDown, upIsDown, rightIsDown, downIsDown;
   r32 frameSeconds;
   Clock_t* clock;
   Input_t* input;
   Entity_t* playerEntity;
   Vector2i32_t playerVelocity;

   playerEntity = Game_GetPlayerEntity( game );
   clock = Game_GetClock( game );
   frameSeconds = Clock_GetFrameSec( clock );
   // TODO: this is 4 tiles per second, but we're getting that familiar jitter that comes from
   // when the world units don't exactly match up with the clock values. let's mess with those
   // a little bit and see if we can get them to land on the exact same boundaries.
   i32 velocity = (i32)( ( WORLD_UNITS_PER_PIXEL * 16 * 4 ) * frameSeconds );

   input = Game_GetInput( game );
   leftIsDown = Input_IsButtonDown( input, InputButton_Left );
   upIsDown = Input_IsButtonDown( input, InputButton_Up );
   rightIsDown = Input_IsButtonDown( input, InputButton_Right );
   downIsDown = Input_IsButtonDown( input, InputButton_Down );

   if ( leftIsDown && !rightIsDown )
   {
      playerVelocity = Entity_GetVelocity( playerEntity );
      playerVelocity.x = -velocity;

      // TODO
      // if ( !( upIsDown && sprite->direction == Direction_Up ) &&
      //      !( downIsDown && sprite->direction == Direction_Down ) )
      // {
      //    ActiveSprite_SetDirection( sprite, Direction_Left );
      // }

      // if ( upIsDown || downIsDown )
      // {
      //    entity->velocity.x = ( entity->velocity.x < 0 ) ? -( TileMap_GetTileDiagonalVelocity( velocity ) ) : TileMap_GetTileDiagonalVelocity( velocity );
      // }

      Entity_SetVelocity( playerEntity, playerVelocity.x, playerVelocity.y );
   }
   else if ( rightIsDown && !leftIsDown )
   {
      playerVelocity = Entity_GetVelocity( playerEntity );
      playerVelocity.x = velocity;

      // TODO
      // if ( !( upIsDown && sprite->direction == Direction_Up ) &&
      //      !( downIsDown && sprite->direction == Direction_Down ) )
      // {
      //    ActiveSprite_SetDirection( sprite, Direction_Right );
      // }

      // if ( upIsDown || downIsDown )
      // {
      //    entity->velocity.x = ( entity->velocity.x < 0 ) ? -( TileMap_GetTileDiagonalVelocity( velocity ) ) : TileMap_GetTileDiagonalVelocity( velocity );
      // }

      Entity_SetVelocity( playerEntity, playerVelocity.x, playerVelocity.y );
   }

   if ( upIsDown && !downIsDown )
   {
      playerVelocity = Entity_GetVelocity( playerEntity );
      playerVelocity.y = -velocity;

      // TODO
      // if ( !( leftIsDown && sprite->direction == Direction_Left ) &&
      //      !( rightIsDown && sprite->direction == Direction_Right ) )
      // {
      //    ActiveSprite_SetDirection( sprite, Direction_Up );
      // }

      // if ( leftIsDown || rightIsDown )
      // {
      //    entity->velocity.y = ( entity->velocity.y < 0 ) ? -( TileMap_GetTileDiagonalVelocity( velocity ) ) : TileMap_GetTileDiagonalVelocity( velocity );
      // }

      Entity_SetVelocity( playerEntity, playerVelocity.x, playerVelocity.y );
   }
   else if ( downIsDown && !upIsDown )
   {
      playerVelocity = Entity_GetVelocity( playerEntity );
      playerVelocity.y = velocity;

      // TODO
      // if ( !( leftIsDown && sprite->direction == Direction_Left ) &&
      //      !( rightIsDown && sprite->direction == Direction_Right ) )
      // {
      //    ActiveSprite_SetDirection( sprite, Direction_Down );
      // }

      // if ( leftIsDown || rightIsDown )
      // {
      //    entity->velocity.y = ( entity->velocity.y < 0 ) ? -( TileMap_GetTileDiagonalVelocity( velocity ) ) : TileMap_GetTileDiagonalVelocity( velocity );
      // }

      Entity_SetVelocity( playerEntity, playerVelocity.x, playerVelocity.y );
   }
}
