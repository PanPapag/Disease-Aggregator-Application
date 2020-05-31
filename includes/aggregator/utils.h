#ifndef __AGGREGATOR_UTILS__
  #define __AGGREGATOR_UTILS__

  #include "../common/list.h"

  /* Given a parent directory returns all the sub directories inside */
  list_ptr get_all_subdirs(char*);
  /* Distributes the sub directories of the input dir to num_workers */
  char** distribute_subdirs(list_ptr, size_t);
  /*
    Given the dir paths for a specific worker and its pid updates the
    country_to_pid hash table
  */
  void update_country_to_pid_ht(char*, pid_t);

#endif
