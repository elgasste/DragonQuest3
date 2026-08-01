#include <stdio.h>

#include "pixel_buffer.h"
#include "mem_arena.h"

void PixelBuffer_Create( PixelBuffer_t** pBuffer, MemArena_t* memArena, u32 w, u32 h )
{
   MemArenaResult_t result;
   char msg[STRING_SIZE_DEFAULT];

   result = MemArena_Alloc( memArena, (void**)pBuffer, sizeof( PixelBuffer_t ) + ( w * h * sizeof( u32 ) ) );
   if ( result != MemArenaResult_Success )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "Failed to create memory arena for pixel buffer: %s", MemArena_GetErrorMessage( result ) );
      FatalError( msg );
   };
   
   ( *pBuffer )->w = w;
   ( *pBuffer )->h = h;
   ( *pBuffer )->mem = (u32*)( (u8*)(*pBuffer) + sizeof( PixelBuffer_t ) );
}
