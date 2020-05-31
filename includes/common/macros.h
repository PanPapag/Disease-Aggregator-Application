#ifndef __COMMON_MACROS__
  #define __COMMON_MACROS__

  #include <stdlib.h>

  #define MAX(x, y) ((x) < (y) ? (y) : (x))

  #define __FREE(ptr) \
  do {                \
    free(ptr);        \
    ptr = NULL;       \
  } while(0)

  #define MAX_BUFFER_SIZE 1024

  #define STRING char*

  #define SPACE " "

  #define NO_BUCKETS 20

  #define BUCKET_SIZE 64

  #define NO_RESPONSE "-1"

#endif
