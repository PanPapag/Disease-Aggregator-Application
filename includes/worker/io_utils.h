#ifndef __WORKER_IO_UTILS__
  #define __WORKER_IO_UTILS__

  /**
    Uncomment the line below if you with the enable extensively checking on
    th records' format
  */
  #define PATIENT_RECORD_CHECKING

  /**
    \brief Parses all the files of a given country directory, checks for errors
    and calls parse_file_and_update_structures
    @param dir_pathname: The relative path to directory
  */
  void parse_directory(const char* dir_name);

  void parse_file_and_update_structures(const char* file_path, const char* date);

  /* Reports Error Messages */
  void report_error(const char *fmt, ...);
  /* Reports Warning Messages */
  void report_warning(const char *fmt, ...);
#endif
