#ifndef __WORKER_IO_UTILS__
  #define __WORKER_IO_UTILS__

  #include "../common/hash_table.h"

  enum handle_command_codes {
   PASS,
   ERROR
 };

  typedef struct worker_parameters {
    size_t buffer_size;
    int write_fd;
    int read_fd;
    char* dir_paths;
  } worker_parameters_t;

  extern worker_parameters_t parameters;

  /**
    \brief Wrapper function which traverses all the files of a given country
    directory, checks for errors and calls parse_file_and_update_structures
  */
  void parse_directory(const char*);
  /**
    \brief Wrapper function which calls construct a new patient record and updates
    structure or put the exit date of an existing one.
    Returns: age_groups_ht which stores for each disease of the given file the
    total number of cases per age group
  */
  hash_table_ptr parse_file_and_update_structures(const char*, const char*, const char*);
  /* Reading commands from parent process and writing the results back to him */
  void worker_main_loop(void);

#endif
