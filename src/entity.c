#include "entity.h"
#include "mem_arena.h"
#include "platform.h"

struct Entity_t
{
   Vector4i32_t rect;
   Vector2i32_t velocity;
   u32 tileIndex;
   ActiveSprite_t* sprite;
   Vector2i32_t spriteOffset;

   void (*onTileIndexChanged)( void* receiver, u32 oldTileIndex, u32 newTileIndex );
   void* onTileIndexChangedReceiver;
};

size_t Entity_GetStructSize( void )
{
   return sizeof( Entity_t );
}

Entity_t* Entity_Create( MemArena_t* memArena )
{
   Entity_t* entity;

   entity = (Entity_t*)MemArena_AllocMem( memArena, sizeof( Entity_t ) );
   entity->tileIndex = 0;
   entity->sprite = 0;
   entity->onTileIndexChanged = 0;
   entity->onTileIndexChangedReceiver = 0;

   return entity;
}

void Entity_Free( Entity_t* entity, MemArena_t* memArena )
{
   MemArena_FreeMem( memArena, entity );
}

Vector4i32_t Entity_GetRect( Entity_t* entity )
{
   return entity->rect;
}

Vector2i32_t Entity_GetVelocity( Entity_t* entity )
{
#if defined( _WIN32 )
   if ( g_winDebugFlags.moveFast )
   {
      if ( entity->velocity.x != 0 )
      {
         entity->velocity.x = ( entity->velocity.x < 0 ) ? -( 180 * WORLD_UNITS_PER_PIXEL ) : ( 180 * WORLD_UNITS_PER_PIXEL );
      }
      if ( entity->velocity.y != 0 )
      {
         entity->velocity.y = ( entity->velocity.y < 0 ) ? -( 180 * WORLD_UNITS_PER_PIXEL ) : ( 180 * WORLD_UNITS_PER_PIXEL );
      }
   }
#endif

   return entity->velocity;
}

u32 Entity_GetTileIndex( Entity_t* entity )
{
   return entity->tileIndex;
}

ActiveSprite_t* Entity_GetSprite( Entity_t* entity )
{
   return entity->sprite;
}

Vector2i32_t Entity_GetSpriteOffset( Entity_t* entity )
{
   return entity->spriteOffset;
}

void Entity_SetOnTileIndexChanged( Entity_t* entity, void* receiver, void (*onTileIndexChanged)( void* receiver, u32 oldTileIndex, u32 newTileIndex ) )
{
   entity->onTileIndexChanged = onTileIndexChanged;
   entity->onTileIndexChangedReceiver = receiver;
}

void Entity_SetPosition( Entity_t* entity, i32 x, i32 y )
{
   entity->rect.x = x;
   entity->rect.y = y;
}

void Entity_SetSize( Entity_t* entity, i32 w, i32 h )
{
   entity->rect.w = w;
   entity->rect.h = h;
}

void Entity_SetVelocity( Entity_t* entity, i32 vx, i32 vy )
{
   entity->velocity.x = vx;
   entity->velocity.y = vy;
}

void Entity_SetTileIndex( Entity_t* entity, u32 tileIndex )
{
   if( entity->tileIndex != tileIndex && entity->onTileIndexChanged )
   {
       entity->onTileIndexChanged( entity->onTileIndexChangedReceiver, entity->tileIndex, tileIndex );
   }

   entity->tileIndex = tileIndex;
}

void Entity_SetSprite( Entity_t* entity, ActiveSprite_t* sprite )
{
   entity->sprite = sprite;
}

void Entity_SetSpriteOffset( Entity_t* entity, i32 offsetX, i32 offsetY )
{
   entity->spriteOffset.x = offsetX;
   entity->spriteOffset.y = offsetY;
}
