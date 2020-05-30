#ifndef __COMMON_UTILS__
  #define __COMMON_UTILS__

  #include <stdint.h>

  #define __USE_XOPEN
  #include <time.h>

  /* Converts a given string to 64 bits integer */
  uint8_t string_to_int64(char*, int64_t*);

  /* A variadic function to concatenate any number of strings */
  char* concat(int count, ...);

  /* Hash Function for unsigned integers */
  size_t uint_create(void*);
  /* Returns an integer in a new memory address with the given value */
  int* int_create(int);
  /* Prints a given int* */
  void int_print(void*, FILE* out);
  /* Compares two given integers */
  int int_compare(void*, void*);
  /* Free a pointer to int */
  void int_destroy(void*);

  /* Hash Function for strings */
  size_t string_hash(void*);
  /* Prints the given string */
  void string_print(void*, FILE* out);
  /* Compares two given strings */
  int string_compare(void*, void*);
  /* Free a pointer to char */
  void string_destroy(void*);
  /* Prints the value of a given string pointer */
  void ptr_to_string_print(void*, FILE* out);
  /* Compares two given string pointers */
  int ptr_to_string_compare(void*, void*);
  /* Free a pointer to char* */
  void ptr_to_string_destroy(void*);

  /* Checks if a string contains only letters and number */
  int is_alpharithmetic(char*);
  /* Checks if a string contains only letters */
  int is_alphabetical(char*);
  /* Checks if a string is number */
  int is_number(char*);

  /* Checks if a date string format is valid */
  int is_valid_date_string(char*);
  /* Checks if a date string format is unspecified aka '-' */
  int is_unspecified_date_string(char*);
  /* Checks if a date in struct tm format is unspecified aka 00-01-1900 */
  int is_unspecified_date_tm(struct tm);
  /*
  Compares two given dates formated strings.
  Returns:
    - 0 if dates are equal.
    - > 0 if date1 is after of date2
    - < 0 if date1 is previous of date2
  */
  int64_t date_string_compare(char*, const char*);
  /* Compares two given dates in struct tm format */
  int64_t date_tm_compare(struct tm, struct tm);

  /* Given an array of strings it returns the same array without the first string */
  char** prune_command_name(char**, size_t);

  /* Returns the last token given a string and a delimiter */
  char* get_last_token(const char*, char*);

#endif
