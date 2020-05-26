#ifndef __COMMON_IO_UTILS__
  #define __COMMON_IO_UTILS__

  /* Writes in chunks a given buffer to the defined file descriptor */
  void write_in_chunks(int, char*, size_t);
  /* Read in chunks a given buffer from the defined file descriptor */
  char* read_in_chunks(int, size_t);
  /* Reports Error Messages */
  void report_error(const char *fmt, ...);
  /* Reports Warning Messages */
  void report_warning(const char *fmt, ...);

#endif
