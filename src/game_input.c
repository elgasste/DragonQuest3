#include "entity.h"
#include "game.h"
#include "input.h"

#define DIAGONAL_VELOCITY_MULTIPLIER   0.707f

void Game_HandleInput( Game_t* game )
{
   b32 leftIsDown, upIsDown, rightIsDown, downIsDown;
   Input_t* input;
   Entity_t* playerEntity;
   Vector2i32_t playerVelocity;
   i32 newVelocity;

   playerEntity = Game_GetPlayerEntity( game );
   // TODO: this should come from the specific tile the player is standing on
   newVelocity = 60 * WORLD_UNITS_PER_PIXEL;

   input = Game_GetInput( game );
   leftIsDown = Input_IsButtonDown( input, InputButton_Left );
   upIsDown = Input_IsButtonDown( input, InputButton_Up );
   rightIsDown = Input_IsButtonDown( input, InputButton_Right );
   downIsDown = Input_IsButtonDown( input, InputButton_Down );

   if ( leftIsDown && !rightIsDown )
   {
      playerVelocity = Entity_GetVelocity( playerEntity );
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

   if ( ( leftIsDown || rightIsDown ) && ( upIsDown || downIsDown ) )
   {
      playerVelocity = Entity_GetVelocity( playerEntity );
      playerVelocity.x = (i32)( playerVelocity.x * DIAGONAL_VELOCITY_MULTIPLIER );
      playerVelocity.y = (i32)( playerVelocity.y * DIAGONAL_VELOCITY_MULTIPLIER );
      Entity_SetVelocity( playerEntity, playerVelocity.x, playerVelocity.y );
   }
}
