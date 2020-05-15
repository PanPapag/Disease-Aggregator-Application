#include <dirent.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

#include "../../includes/common/macros.h"
#include "../../includes/worker/io_utils.h"
#include "../../includes/worker/utils.h"

void parse_directory(const char* dir_name) {
  struct dirent* direntp;
  DIR* dir_ptr = opendir(dir_name);
  if (dir_ptr == NULL) {
    report_error("Cannot open %s\n", dir_name);
  }
  else {
    while ((direntp = readdir(dir_ptr)) != NULL) {
      // Skip previous and current folder
      if (strcmp(direntp->d_name, ".") && strcmp(direntp->d_name, "..")) {
        /* Construct full file path */
        // Allocatte memory to save the constructed file_path
        size_t file_path_size = strlen(dir_name) + strlen(direntp->d_name) + 1;
        char *file_path = (char *) malloc(file_path_size * sizeof(char));
        if (file_path == NULL) {
          report_error("Could not allocate memory for file path construction!");
          exit(EXIT_FAILURE);
        }
        snprintf(file_path, file_path_size, "%s/%s", dir_name, direntp->d_name);
        printf("%s\n", file_path);
        // deallocate memory for the next one
        __FREE(file_path);
      }
    }
    closedir(dir_ptr);
  }
}

static void __report(const char* tag, const char* fmt, va_list args) {
  fprintf(stderr, "%s", tag);
  vfprintf(stderr, fmt, args);
  fputc('\n', stderr);
  va_end(args);
}

void report_error(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  __report("[ERROR]: ", fmt, args);
}

void report_warning(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  __report("[WARNING]: ", fmt, args);
}
