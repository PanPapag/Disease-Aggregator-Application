#ifndef __WORKER_IO_UTILS__
  #define __WORKER_IO_UTILS__

  /**
    \brief Parses all the files of a given country directory, checks for errors
    and updates corresponding structures
    @param dir_pathname: The relative path to directory
  */
  void parse_directory_and_update_structures(const char* dir_pathname);

  /* Reports Error Messages */
  void report_error(const char *fmt, ...);
  /* Reports Warning Messages */
  void report_warning(const char *fmt, ...);
#endif