#ifndef __COMMON_UTILS__
  #define __COMMON_UTILS__

  #include <stdint.h>

  /* Converts a given string to 64 bits integer */
  uint8_t string_to_int64(char*, int64_t*);
  /* A variadic function to concatenate any number of strings */
  char* concat(int count, ...);
#endif
