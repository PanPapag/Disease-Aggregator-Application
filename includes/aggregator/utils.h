#ifndef __AGGREGATOR_UTILS__
  #define __AGGREGATOR_UTILS__

  #include "../common/list.h"

  #define FIFO_NAME_SIZE 15

  typedef struct program_parameters {
    size_t num_workers;
    size_t buffer_size;
    char* input_dir;
    char** worker_dir_paths;
    pid_t* workers_pid;
    int* workers_fd_1;
    int* workers_fd_2;
    int terminate_flag;
  } program_parameters_t;

  extern program_parameters_t parameters;

  /* Given a parent directory returns all the sub directories inside */
  list_ptr get_all_subdirs(char*);
  /* Distributes the sub directories of the input dir to num_workers */
  char** distribute_subdirs(list_ptr, size_t);
  /*
   Sends to the workers the directories it has to parse, print stastistics
   and updates program_parameters
  */
  void update_worker(pid_t, int);
  /*
    Given the dir paths for a specific worker and its pid updates the
    country_to_pid hash table
  */
  void update_country_to_pid_ht(char*, pid_t);
  /*
    Given a worker's pid returns the position of fds in the program_parameters
    workers_fd_1 and workers_fd_2 arrays
  */
  int get_worker_fds_pos(pid_t);

#endif
