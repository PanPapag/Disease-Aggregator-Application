#ifndef __COMMON_STATISTICS__
  #define __COMMON_STATISTICS__

  #include "./list.h"

  typedef struct statistics_t* statistics_ptr;
  typedef struct statistics_data_t* statistics_data_ptr;

  struct statistics_t {
    char* file_path;
    list_ptr data;
  } statistics_t;

  struct statistics_data_t {
    char* disease;
    int age_ranges[4];
  } statistics_data_t;
  
#endif
