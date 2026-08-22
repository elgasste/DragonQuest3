#if !defined( MOCK_DISPLAY_H )
#define MOCK_DISPLAY_H

typedef struct PixelBuffer_t PixelBuffer_t;

typedef struct Display_t
{
   PixelBuffer_t* buffer;
}
Display_t;

#endif // MOCK_DISPLAY_H
