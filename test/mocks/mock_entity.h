#if !defined( MOCK_ENTITY_H )
#define MOCK_ENTITY_H

#include "../../include/entity.h"

typedef struct Entity_t
{
   Vector4i32_t rect;
   Vector2i32_t velocity;
}
Entity_t;

#endif // MOCK_ENTITY_H
