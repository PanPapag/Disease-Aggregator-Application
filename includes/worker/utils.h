#ifndef __UTILS__
  #define __UTILS__

  #include <stdint.h>

  #define _XOPEN_SOURCE
  #include <time.h>

  /* Hash Function for unsigned integers */
  size_t hash_uint(void*);
  /* Returns an integer in a new memory address with the given value */
  int* create_int(int);
  /* Prints a given int* */
  void print_int(void*, FILE* out);
  /* Compares two given integers */
  int compare_int(void*, void*);
  /* Free a pointer to int */
  void destroy_int(void*);

  /* Hash Function for strings */
  size_t hash_string(void*);
  /* Prints the value of a given string pointer */
  void print_string_ptr(void*, FILE* out);
  /* Prints the given string */
  void print_string(void*, FILE* out);
  /* Compares two given strings */
  int compare_string(void*, void*);
  /* Free a pointer to char */
  void destroy_string(void*);

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
  int64_t compare_date_strings(char*, const char*);
  /* Compares two given dates in struct tm format */
  int64_t compare_date_tm(struct tm, struct tm);

  /* Given an array of strings it returns the same array without the first string */
  char** prune_command_name(char**, size_t);

  /* Returns the last token given a string and a delimiter */
  char* get_last_token(const char*, char*);

#endif
