#if !defined( GAME_DATA_H )
#define GAME_DATA_H

#include "common.h"

#define GAME_DATA_MAGIC "DW3D"

typedef struct GameDataVersion_t
{
   u8 major;
   u8 minor;
   u8 maint;
}
GameDataVersion_t;

typedef struct GameDataHeader_t
{
   char magic[4];
   GameDataVersion_t version;
   u32 tileTexturesHeaderOffset;
}
GameDataHeader_t;

typedef struct GameDataTileTexturesHeader_t
{
   u32 count;
   u32 tileSize;
   u32 texturesOffset;
}
GameDataTileTexturesHeader_t;

#endif // GAME_DATA_H
