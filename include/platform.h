#if !defined( PLATFORM_H )
#define PLATFORM_H

#include "common.h"

typedef struct Game_t Game_t;
typedef struct MemArena_t MemArena_t;
typedef struct Screen_t Screen_t;

void Platform_Log( const char* msg );
void Platform_FatalError( const char* msg );
u64 Platform_GetMicros( void );
void Platform_HandleMessages( Game_t* game );
void Platform_RenderScreenBuffer( Screen_t* screen );
void Platform_SleepMs( u32 ms );
u8* Platform_LoadFileToMemory( const char* filePath, MemArena_t* memArena, u32* bytesRead );

#endif // PLATFORM_H
