#ifndef __COMMON_MACROS__
  #define __COMMON_MACROS__

  #include <stdlib.h>

  #define MAX(x, y) ((x) < (y) ? (y) : (x))

  #define __FREE(ptr) \
  do {                \
    free(ptr);        \
    ptr = NULL;       \
  } while(0)

  #define BUFFER_SIZE 1025

  #define STRING char*

#endif
