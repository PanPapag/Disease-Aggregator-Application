#ifndef __WORKER_IO_UTILS__
  #define __WORKER_IO_UTILS__

  /**
    \brief Parses all the files of a given country directory, checks for errors
    and calls parse_file_and_update_structures
    @param dir_pathname: The relative path to directory
  */
  void parse_directory(const char* dir_name);

  /* Reports Error Messages */
  void report_error(const char *fmt, ...);
  /* Reports Warning Messages */
  void report_warning(const char *fmt, ...);
#endif
