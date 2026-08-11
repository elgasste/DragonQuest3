#if !defined( FILE_H )
#define FILE_H

#include "common.h"

typedef struct File_t
{
   void *stream;
   size_t size;
}
File_t;

#endif // FILE_H
