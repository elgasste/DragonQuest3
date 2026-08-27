#include "file.h"
#include "game_data.h"
#include "mem_arena.h"
#include "tile_texture_set.h"

struct TileTextureSet_t
{
   TileTextureSetInfo_t info;
   u32* textures;
};

TileTextureSet_t* TileTextureSet_CreateFromGameData( MemArena_t* memArena, GameData_t* gameData )
{
   i32 fileOffset, textureDataOffset, textureDataSize;
   GameDataFileOffsets_t fileOffsets;
   TileTextureSet_t* tileTextureSet;
   File_t* file;

   file = GameData_GetFile( gameData );
   fileOffsets = GameData_GetFileOffsets( gameData );
   fileOffset = fileOffsets.tileTextureSet;
   if ( (i32)( fileOffset + sizeof( TileTextureSetInfo_t ) ) > file->size )
   {
      Platform_FatalError( "game data file is too small to contain tile texture set info." );
      return 0;
   }

   tileTextureSet = (TileTextureSet_t*)MemArena_AllocMem( memArena, sizeof( TileTextureSet_t ) );
   Platform_FileSeek( file, fileOffset, 0 );
   Platform_ReadFileBytes( file, (u8*)( &tileTextureSet->info ), sizeof( TileTextureSetInfo_t ) );
   tileTextureSet->textures = 0;
   textureDataOffset = fileOffset + sizeof( TileTextureSetInfo_t );

   if ( tileTextureSet->info.count > 0 )
   {
      textureDataSize = tileTextureSet->info.count * tileTextureSet->info.tileSize * tileTextureSet->info.tileSize * sizeof( u32 );
      if ( ( textureDataOffset + textureDataSize ) > file->size )
      {
         Platform_FatalError( "game data file is too small to contain all tile textures." );
         MemArena_FreeMem( memArena, tileTextureSet );
         return 0;
      }

      tileTextureSet->textures = (u32*)MemArena_AllocMem( memArena, textureDataSize );
      Platform_FileSeek( file, textureDataOffset, 0 );
      Platform_ReadFileBytes( file, (u8*)( tileTextureSet->textures ), textureDataSize );
   }

   return tileTextureSet;
}

size_t TileTextureSet_GetStructSize( void )
{
   return sizeof( TileTextureSet_t );
}

void TileTextureSet_Free( TileTextureSet_t* tileTextureSet, MemArena_t* memArena )
{
   MemArena_FreeMem( memArena, tileTextureSet->textures );
   MemArena_FreeMem( memArena, tileTextureSet );
}

u32 TileTextureSet_GetCount( TileTextureSet_t* tileTextureSet )
{
   return tileTextureSet->info.count;
}

u32 TileTextureSet_GetTileSize( TileTextureSet_t* tileTextureSet )
{
   return tileTextureSet->info.tileSize;
}

u32* TileTextureSet_GetTexture( TileTextureSet_t* tileTextureSet, u32 index )
{
   return tileTextureSet->textures + ( index * tileTextureSet->info.tileSize * tileTextureSet->info.tileSize );
}
