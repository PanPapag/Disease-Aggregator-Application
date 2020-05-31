#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "../../includes/common/hash_table.h"
#include "../../includes/common/list.h"
#include "../../includes/common/macros.h"
#include "../../includes/common/io_utils.h"
#include "../../includes/common/utils.h"
#include "../../includes/aggregator/utils.h"

hash_table_ptr country_to_pid_ht;
list_ptr countries_names;

program_parameters_t parameters;

list_ptr get_all_subdirs(char* parent_dir) {
  list_ptr result = list_create(STRING*, ptr_to_string_compare,
                                ptr_to_string_print, ptr_to_string_destroy);

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

char** distribute_subdirs(list_ptr directories, size_t num_workers) {
  size_t num_directories = list_size(directories);
  char** result = (char**) malloc(num_workers * sizeof(char*));
  /*
    Construct an array with index the worker number and value the number of
    directories which he is going to handle
  */
  int no_dirs_per_worker[num_workers];
  memset(no_dirs_per_worker, 0, sizeof(no_dirs_per_worker));
  /* Case 1: num_workers > num_directories */
  if (num_workers > num_directories) {
    for (size_t i = 0; i < num_workers; ++i) {
      if (i == num_directories)
        break;
      no_dirs_per_worker[i] = 1;
    }
  } else {
    /* Case 2: num_workers <= num_directories */
    for (size_t i = 0; i < num_workers; ++i) {
      no_dirs_per_worker[i] = num_directories / num_workers;
    }
    for (size_t i = 0; i < num_directories % num_workers; ++i) {
      no_dirs_per_worker[i]++;
    }
  }
  /* Copy directories names to result array */
  for (size_t i = 0; i < num_workers; ++i) {
    char buffer[MAX_BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    for (size_t j = 0; j < no_dirs_per_worker[i]; ++j) {
      list_node_ptr list_node = list_pop_front(&directories);
      char* dir_name = (*(char **) list_node->data_);
      strcat(buffer, dir_name);
      if (j < no_dirs_per_worker[i] - 1) {
        strcat(buffer, SPACE);
      }
      /* Clear memory */
      free(list_node);
      free(dir_name);
    }
    result[i] = (char*) malloc(strlen(buffer) + 1);
    strcpy(result[i], buffer);
  }

  return result;
}

void update_country_to_pid_ht(char* worker_dir_paths, pid_t worker_pid) {
  /* Copy to buffer so that dir_paths won't change while tokenized */
  char buffer[MAX_BUFFER_SIZE];
  strcpy(buffer, worker_dir_paths);
  /* Extract countries names */
  char* worker_dir_path = strtok(buffer, SPACE);
  while (worker_dir_path != NULL) {
    char* country_name = get_last_token(worker_dir_path, '/');
    char* ptr_to_country_name = string_create(country_name);
    pid_t* ptr_to_worker_pid = pid_create(worker_pid);
    hash_table_insert(&country_to_pid_ht, ptr_to_country_name, ptr_to_worker_pid);
    list_sorted_insert(&countries_names, &ptr_to_country_name);
    worker_dir_path = strtok(NULL, SPACE);
  }
}

int get_worker_fds_pos(pid_t pid) {
  for (int i = 0; i < parameters.num_workers; ++i) {
    if (parameters.workers_pid[i] == pid) {
      return i;
    }
  }
  return -1;
}
