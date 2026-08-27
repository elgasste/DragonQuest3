#if !defined( ENTITY_H )
#define ENTITY_H

#include "common.h"
#include "vector.h"

typedef struct MemArena_t MemArena_t;
typedef struct ActiveSprite_t ActiveSprite_t;

typedef struct Entity_t Entity_t;
size_t Entity_GetStructSize( void );
Entity_t* Entity_Create( MemArena_t* memArena );
void Entity_Free( Entity_t* entity, MemArena_t* memArena );

Vector4i32_t Entity_GetRect( Entity_t* entity );
Vector2i32_t Entity_GetVelocity( Entity_t* entity );
u32 Entity_GetTileIndex( Entity_t* entity );
ActiveSprite_t* Entity_GetSprite( Entity_t* entity );
Vector2i32_t Entity_GetSpriteOffset( Entity_t* entity );

void Entity_SetOnTileIndexChanged( Entity_t* entity, void (*onTileIndexChanged)( Entity_t* entity, u32 oldTileIndex, u32 newTileIndex ) );
void Entity_SetPosition( Entity_t* entity, i32 x, i32 y );
void Entity_SetSize( Entity_t* entity, i32 w, i32 h );
void Entity_SetVelocity( Entity_t* entity, i32 vx, i32 vy );
void Entity_SetTileIndex( Entity_t* entity, u32 tileIndex );
void Entity_SetSprite( Entity_t* entity, ActiveSprite_t* sprite );
void Entity_SetSpriteOffset( Entity_t* entity, i32 offsetX, i32 offsetY );

#endif // ENTITY_H
