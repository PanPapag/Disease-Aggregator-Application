#include <dirent.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wordexp.h>

#include <sys/types.h>

#include "../../includes/common/macros.h"
#include "../../includes/worker/commands.h"
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
        // Allocatte memory to save the constructed file_path
        size_t file_path_size = strlen(dir_name) + strlen(direntp->d_name) + 2;
        char *file_path = (char *) malloc(file_path_size * sizeof(char));
        if (file_path == NULL) {
          report_error("Could not allocate memory for file path construction!");
          exit(EXIT_FAILURE);
        }
        // Construct full file path
        snprintf(file_path, file_path_size, "%s/%s", dir_name, direntp->d_name);
        // The parsing of the file and the update of the structures is executed
        // by the function below
        parse_file_and_update_structures(file_path, direntp->d_name);
        // Deallocate memory for the next one
        __FREE(file_path);
      }
    }
    closedir(dir_ptr);
  }
}

void parse_file_and_update_structures(const char* file_path, const char* date) {
  char buffer[MAX_BUFFER_SIZE];
  char** patient_record_tokens;
  int patient_record_no_tokens;
  wordexp_t p;
  printf("%s %s\n",file_path, date);
  /* Open file for read only - handles binary fille too */
  FILE* fp = fopen(file_path, "rb+");
  /* Read file line by line */
  while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    /* Discard '\n' that fgets() stores */
    buffer[strlen(buffer) - 1] = '\0';
    wordexp(buffer, &p, 0);
    patient_record_tokens = p.we_wordv;
    patient_record_no_tokens = p.we_wordc;
    execute_insert_patient_record(patient_record_tokens);
    /* Free wordexp object */
    wordfree(&p);
  }
  /* Close file pointer */
  fclose(fp);
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
