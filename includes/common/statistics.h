#ifndef __COMMON_STATISTICS__
  #define __COMMON_STATISTICS__

  #include "../worker/hash_table.h"

  typedef struct statistics_t* statistics_ptr;
  typedef struct statistics_data_t* statistics_data_ptr;

  struct statistics_t {
    char* file_name;
    char* country;
    hash_table_ptr age_groups_ht;
  } statistics_t;

  statistics_ptr statistics_create(const char*, const char*, hash_table_ptr);

  void statistics_print(void*, FILE*);

  void age_groups_print(void*, FILE*);

#endif
