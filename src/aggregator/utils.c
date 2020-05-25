#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "../../includes/common/list.h"
#include "../../includes/common/macros.h"
#include "../../includes/common/io_utils.h"
#include "../../includes/common/utils.h"
#include "../../includes/aggregator/utils.h"

list_ptr get_all_subdirs(char* parent_dir) {
  list_ptr result = list_create(STRING*, compare_string_ptr,
                                print_string_ptr, destroy_string_ptr);

  struct stat path_stat;
  struct dirent* direntp;
  DIR* dir_ptr = opendir(parent_dir);
  if (dir_ptr == NULL) {
    report_error("Cannot open %s\n", parent_dir);
  }
  else {
    while ((direntp = readdir(dir_ptr)) != NULL) {
      // Skip previous and current folder
      if (strcmp(direntp->d_name, ".") && strcmp(direntp->d_name, "..")) {
        char* full_path = concat(2, parent_dir, direntp->d_name);
        stat(full_path, &path_stat);
        if (S_ISDIR(path_stat.st_mode)){
          list_push_back(&result, &full_path);
        }
      }
    }
    closedir(dir_ptr);
  }
  return result;
}
