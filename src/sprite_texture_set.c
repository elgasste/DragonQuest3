#include "direction.h"
#include "file.h"
#include "game_data.h"
#include "mem_arena.h"
#include "sprite_texture_set.h"

struct ActiveSpriteTextureSet_t
{
   ActiveSpriteTextureSetInfo_t info;
   u32* textures;
};

size_t ActiveSpriteTextureSet_GetStructSize( void )
{
   return sizeof( ActiveSpriteTextureSet_t );
}

ActiveSpriteTextureSet_t* ActiveSpriteTextureSet_CreateFromGameData( MemArena_t* memArena, GameData_t* gameData )
{
   i32 fileOffset, textureDataOffset, textureDataSize;
   GameDataFileOffsets_t fileOffsets;
   ActiveSpriteTextureSet_t* textureSet;
   File_t* file;

   file = GameData_GetFile( gameData );
   fileOffsets = GameData_GetFileOffsets( gameData );
   fileOffset = fileOffsets.activeSpriteTextureSet;
   if ( (i32)( fileOffset + sizeof( ActiveSpriteTextureSetInfo_t ) ) > file->size )
   {
      Platform_FatalError( "game data file is too small to contain sprite texture set info." );
      return 0;
   }

   textureSet = (ActiveSpriteTextureSet_t*)MemArena_AllocMem( memArena, sizeof( ActiveSpriteTextureSet_t ) );
   Platform_FileSeek( file, fileOffset, 0 );
   Platform_ReadFileBytes( file, (u8*)&textureSet->info, sizeof( ActiveSpriteTextureSetInfo_t ) );
   textureSet->textures = 0;
   textureDataOffset = fileOffset + sizeof( ActiveSpriteTextureSetInfo_t );

   if ( textureSet->info.count > 0 )
   {
      textureDataSize = textureSet->info.count * textureSet->info.frameSize * textureSet->info.frameSize * textureSet->info.frameCount * Direction_Count * sizeof( u32 );
      if ( ( textureDataOffset + textureDataSize ) > file->size )
      {
         Platform_FatalError( "game data file is too small to contain all active sprite textures." );
         MemArena_FreeMem( memArena, textureSet );
         return 0;
      }

      textureSet->textures = (u32*)MemArena_AllocMem( memArena, textureDataSize );
      Platform_FileSeek( file, textureDataOffset, 0 );
      Platform_ReadFileBytes( file, (u8*)( textureSet->textures ), textureDataSize );
   }

   return textureSet;
}

void ActiveSpriteTextureSet_Free( ActiveSpriteTextureSet_t* textureSet, MemArena_t* memArena )
{
   MemArena_FreeMem( memArena, textureSet->textures );
   MemArena_FreeMem( memArena, textureSet );
}

u32 ActiveSpriteTextureSet_GetCount( ActiveSpriteTextureSet_t* textureSet )
{
   return textureSet->info.count;
}

u32 ActiveSpriteTextureSet_GetFrameSize( ActiveSpriteTextureSet_t* textureSet )
{
   return textureSet->info.frameSize;
}

u32 ActiveSpriteTextureSet_GetFrameCount( ActiveSpriteTextureSet_t* textureSet )
{
   return textureSet->info.frameCount;
}

u32* ActiveSpriteTextureSet_GetTexture( ActiveSpriteTextureSet_t* textureSet, u32 index )
{
   return textureSet->textures + ( index * textureSet->info.frameSize * textureSet->info.frameSize );
}
