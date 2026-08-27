#if !defined( SPRITE_TEXTURE_SET )
#define SPRITE_TEXTURE_SET

#include "common.h"
#include "platform.h"

typedef struct GameData_t GameData_t;
typedef struct MemArena_t MemArena_t;

PACKED_STRUCT
typedef struct ActiveSpriteTextureSetInfo_t
{
   u32 count;
   u32 frameSize;
   u32 frameCount;
}
ActiveSpriteTextureSetInfo_t;
END_PACKED_STRUCT

typedef struct ActiveSpriteTextureSet_t ActiveSpriteTextureSet_t;
size_t ActiveSpriteTextureSet_GetStructSize( void );
ActiveSpriteTextureSet_t* ActiveSpriteTextureSet_CreateFromGameData( MemArena_t* memArena, GameData_t* gameData );
void ActiveSpriteTextureSet_Free( ActiveSpriteTextureSet_t* textureSet, MemArena_t* memArena );

u32 ActiveSpriteTextureSet_GetCount( ActiveSpriteTextureSet_t* textureSet );
u32 ActiveSpriteTextureSet_GetFrameSize( ActiveSpriteTextureSet_t* textureSet );
u32 ActiveSpriteTextureSet_GetFrameCount( ActiveSpriteTextureSet_t* textureSet );
u32* ActiveSpriteTextureSet_GetTexture( ActiveSpriteTextureSet_t* textureSet, u32 index );

#endif // SPRITE_TEXTURE_SET
