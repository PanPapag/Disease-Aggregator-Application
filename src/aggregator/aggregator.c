#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../../includes/common/list.h"
#include "../../includes/common/macros.h"
#include "../../includes/common/io_utils.h"
#include "../../includes/common/utils.h"
#include "../../includes/aggregator/io_utils.h"

program_parameters_t parameters;

int main(int argc, char* argv[]) {
  /* Parse command line arguments and update program parameters */
  parse_arguments(&argc, argv);

  printf("%zu\n", parameters.num_workers);
  printf("%zu\n", parameters.buffer_size);
  printf("%s\n", parameters.input_dir);
  printf("------------------------\n");
  /*
    Initialize a list to store the dir paths which will be distributed by
    disease aggregator to num_workers child proccesses
  */
	list_ptr input_dir_subdirs = list_create(STRING*, compare_string, print_string_ptr, destroy_string);

  struct stat path_stat;
  struct dirent* direntp;
  DIR* dir_ptr = opendir(parameters.input_dir);
  if (dir_ptr == NULL) {
    report_error("Cannot open %s\n", parameters.input_dir);
  }
  else {
    while ((direntp = readdir(dir_ptr)) != NULL) {
      // Skip previous and current folder
      if (strcmp(direntp->d_name, ".") && strcmp(direntp->d_name, "..")) {
        char* full_path = concat(2, parameters.input_dir, direntp->d_name);
        stat(full_path, &path_stat);
        if (S_ISDIR(path_stat.st_mode)){
          list_push_back(&input_dir_subdirs, &full_path);
        }
      }
    }
    closedir(dir_ptr);
  }
  list_print(input_dir_subdirs, stdout);
  list_clear(input_dir_subdirs);
  return EXIT_SUCCESS;
}
