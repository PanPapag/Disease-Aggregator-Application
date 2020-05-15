#ifndef __WORKER_UTILS__
  #define __WORKER_UTILS__

  /**
    \brief Joins the directory name with the file name using '/' and returns in
    char* the full file_path
  */
  void construct_file_path(const char* dir_name, const char* file_name, char* file_path);

#endif
