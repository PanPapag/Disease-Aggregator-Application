#ifndef __AGGREGATOR_IO_UTILS__
  #define __AGGREGATOR_IO_UTILS__

  /* Parses command line arguments and stores them to program_parameters_t */
  void parse_arguments(int*, char**);

  /* Reads commands from stdin until exit will be given */
  void main_loop(void);

  /* Given a command read from stding determine which function to call */
  void handle_command(char[]);

#endif
