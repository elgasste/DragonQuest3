#include <stdio.h>

#include "mem_arena.h"
#include "pixel_buffer.h"
#include "platform_ops.h"

void PixelBuffer_Create( PixelBuffer_t** pBuffer, MemArena_t* memArena, u32 w, u32 h )
{
   MemArenaResult_t memArenaResult;
   char msg[STRING_SIZE_DEFAULT];

   memArenaResult = MemArena_Alloc( memArena, (void**)pBuffer, sizeof( PixelBuffer_t ) + ( w * h * sizeof( u32 ) ) );
   if ( memArenaResult != MemArenaResult_Success )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "Failed to create memory arena for pixel buffer: %s", MemArena_GetErrorMessage( memArenaResult ) );
      PlatformOps_FatalError( msg );
      return;
   };
   
   ( *pBuffer )->w = w;
   ( *pBuffer )->h = h;
   ( *pBuffer )->mem = (u32*)( (u8*)(*pBuffer) + sizeof( PixelBuffer_t ) );
}

void PixelBuffer_ClearColor( PixelBuffer_t* buffer, u32 color )
{
   size_t pixels, i;

   pixels = buffer->w * buffer->h;
   for ( i = 0; i < pixels; i++ )
   {
      buffer->mem[i] = color;
   }
}
