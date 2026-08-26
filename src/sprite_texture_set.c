#include "direction.h"
#include "mem_arena.h"
#include "sprite_texture_set.h"

struct ActiveSpriteTextureSet_t
{
   u32 count;
   u32 frameSize;
   u32 frameCount;
   u32* textures;
};

size_t ActiveSpriteTextureSet_GetStructSize( void )
{
   return sizeof( ActiveSpriteTextureSet_t );
}

ActiveSpriteTextureSet_t* ActiveSpriteTextureSet_CreateFromGameData( MemArena_t* memArena, GameData_t* gameData )
{
   ActiveSpriteTextureSet_t* textureSet;

   UNUSED_PARAM( gameData );

   textureSet = (ActiveSpriteTextureSet_t*)MemArena_AllocMem( memArena, sizeof( ActiveSpriteTextureSet_t ) );

   // TODO: actually load the textures from the game data

   return textureSet;
}

void ActiveSpriteTextureSet_Free( ActiveSpriteTextureSet_t* textureSet, MemArena_t* memArena )
{
   MemArena_FreeMem( memArena, textureSet );
}

u32 ActiveSpriteTextureSet_GetCount( ActiveSpriteTextureSet_t* textureSet )
{
   return textureSet->count;
}

u32 ActiveSpriteTextureSet_GetFrameSize( ActiveSpriteTextureSet_t* textureSet )
{
   return textureSet->frameSize;
}

u32 ActiveSpriteTextureSet_GetFrameCount( ActiveSpriteTextureSet_t* textureSet )
{
   return textureSet->frameCount;
}

u32 ActiveSpriteTextureSet_GetTexture( ActiveSpriteTextureSet_t* textureSet, u32 index )
{
   return textureSet->textures[index];
}
