#include "entity.h"
#include "game.h"
#include "input.h"
#include "sprite.h"

#define DIAGONAL_VELOCITY_MULTIPLIER   0.707f

// TODO: this should go in a really specific handler, like an overworld movement handler
void Game_HandleInput( Game_t* game )
{
   b32 leftIsDown, upIsDown, rightIsDown, downIsDown;
   Input_t* input;
   Entity_t* playerEntity;
   ActiveSprite_t* playerSprite;
   Vector2i32_t playerVelocity;
   i32 newVelocity;

   playerEntity = Game_GetPlayerEntity( game );
   playerSprite = Entity_GetSprite( playerEntity );
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

      if ( !( upIsDown && ActiveSprite_GetDirection( playerSprite ) == Direction_Up ) &&
           !( downIsDown && ActiveSprite_GetDirection( playerSprite ) == Direction_Down ) )
      {
         ActiveSprite_SetDirection( playerSprite, Direction_Left );
      }

      Entity_SetVelocity( playerEntity, playerVelocity.x, playerVelocity.y );
   }
   else if ( rightIsDown && !leftIsDown )
   {
      playerVelocity = Entity_GetVelocity( playerEntity );
      playerVelocity.x = newVelocity;

      if ( !( upIsDown && ActiveSprite_GetDirection( playerSprite ) == Direction_Up ) &&
           !( downIsDown && ActiveSprite_GetDirection( playerSprite ) == Direction_Down ) )
      {
         ActiveSprite_SetDirection( playerSprite, Direction_Right );
      }

      Entity_SetVelocity( playerEntity, playerVelocity.x, playerVelocity.y );
   }

   if ( upIsDown && !downIsDown )
   {
      playerVelocity = Entity_GetVelocity( playerEntity );
      playerVelocity.y = -newVelocity;

      if ( !( leftIsDown && ActiveSprite_GetDirection( playerSprite ) == Direction_Left ) &&
           !( rightIsDown && ActiveSprite_GetDirection( playerSprite ) == Direction_Right ) )
      {
         ActiveSprite_SetDirection( playerSprite, Direction_Up );
      }

      Entity_SetVelocity( playerEntity, playerVelocity.x, playerVelocity.y );
   }
   else if ( downIsDown && !upIsDown )
   {
      playerVelocity = Entity_GetVelocity( playerEntity );
      playerVelocity.y = newVelocity;

      if ( !( leftIsDown && ActiveSprite_GetDirection( playerSprite ) == Direction_Left ) &&
           !( rightIsDown && ActiveSprite_GetDirection( playerSprite ) == Direction_Right ) )
      {
         ActiveSprite_SetDirection( playerSprite, Direction_Down );
      }

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
