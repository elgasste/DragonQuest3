#include "entity.h"
#include "game.h"
#include "input.h"

// TODO : figure out the frickin' jitterbug issue
void Game_HandleInput( Game_t* game )
{
   b32 leftIsDown, upIsDown, rightIsDown, downIsDown;
   Input_t* input;
   Entity_t* playerEntity;
   Vector2i32_t playerVelocity;
   i32 newVelocity;

   playerEntity = Game_GetPlayerEntity( game );
   newVelocity = 60 * WORLD_UNITS_PER_PIXEL;

   input = Game_GetInput( game );
   leftIsDown = Input_IsButtonDown( input, InputButton_Left );
   upIsDown = Input_IsButtonDown( input, InputButton_Up );
   rightIsDown = Input_IsButtonDown( input, InputButton_Right );
   downIsDown = Input_IsButtonDown( input, InputButton_Down );

   if ( leftIsDown && !rightIsDown )
   {
      playerVelocity = Entity_GetVelocity( playerEntity );
      // this should be units per second
      playerVelocity.x = -newVelocity;

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
      playerVelocity.x = newVelocity;

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
      playerVelocity.y = -newVelocity;

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
      playerVelocity.y = newVelocity;

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
