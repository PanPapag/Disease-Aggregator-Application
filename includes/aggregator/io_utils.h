#ifndef __AGGREGATOR_IO_UTILS__
  #define __AGGREGATOR_IO_UTILS__

  typedef struct program_parameters {
    size_t num_workers;
    size_t buffer_size;
    char* input_dir;
  } program_parameters_t;

  extern program_parameters_t parameters;

  /* Parses command line arguments and stores them to program_parameters_t */
  void parse_arguments(int*, char**);

#endif