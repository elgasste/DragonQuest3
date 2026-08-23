#if !defined( ENTITY_H )
#define ENTITY_H

#include "common.h"
#include "vector.h"

typedef struct MemArena_t MemArena_t;

typedef struct Entity_t Entity_t;
size_t Entity_GetStructSize( void );
Entity_t* Entity_Create( MemArena_t* memArena, i32 x, i32 y, i32 w, i32 h );
void Entity_Free( Entity_t* entity, MemArena_t* memArena );

Vector4i32_t Entity_GetRect( Entity_t* entity );
Vector2i32_t Entity_GetVelocity( Entity_t* entity );

void Entity_SetPosition( Entity_t* entity, i32 x, i32 y );
void Entity_SetSize( Entity_t* entity, i32 w, i32 h );
void Entity_SetVelocity( Entity_t* entity, i32 vx, i32 vy );

#endif // ENTITY_H
