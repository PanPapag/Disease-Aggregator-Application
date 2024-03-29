#include <dirent.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wordexp.h>

#include <sys/types.h>

#include "../../includes/common/hash_table.h"
#include "../../includes/common/list.h"
#include "../../includes/common/macros.h"
#include "../../includes/common/io_utils.h"
#include "../../includes/common/statistics.h"
#include "../../includes/common/utils.h"
#include "../../includes/worker/commands.h"
#include "../../includes/worker/io_utils.h"
#include "../../includes/worker/patient_record.h"
#include "../../includes/worker/signals_handling.h"

hash_table_ptr file_paths_ht;

list_ptr files_statistics;

volatile sig_atomic_t new_files;

jmp_buf interrupt;

void parse_directory(const char* dir_path) {
  hash_table_ptr age_groups_ht;
  statistics_entry_ptr statistics_entry;
  struct dirent* direntp;
  DIR* dir_ptr = opendir(dir_path);
  if (dir_ptr == NULL) {
    report_error("Cannot open %s\n", dir_path);
  }
  else {
    const char* dir_name = get_last_token(dir_path, '/');
    while ((direntp = readdir(dir_ptr)) != NULL) {
      // Skip previous and current folder
      if (strcmp(direntp->d_name, ".") && strcmp(direntp->d_name, "..")) {
        // Allocatte memory to save the constructed file_path
        size_t file_path_size = strlen(dir_path) + strlen(direntp->d_name) + 2;
        char *file_path = (char *) malloc(file_path_size * sizeof(char));
        if (file_path == NULL) {
          report_error("Cannot allocate memory for the file path construction!");
          exit(EXIT_FAILURE);
        }
        // Construct full file path
        snprintf(file_path, file_path_size, "%s/%s", dir_path, direntp->d_name);
        if (new_files == 1) {
          void* result = hash_table_find(file_paths_ht, file_path);
          if (result == NULL) {
            /* New File */
            age_groups_ht = parse_file_and_update_structures(dir_name, file_path,
                                                             direntp->d_name);
            /* Store statistics */                                                 
            statistics_entry = statistics_entry_create(direntp->d_name, dir_name, age_groups_ht);
            list_push_back(&files_statistics, &statistics_entry);
          } else {
            __FREE(file_path);
          }
        } else {
          /* Insert file path to file_paths_ht */
          hash_table_insert(&file_paths_ht, file_path, file_path);
          // The parsing of the file and the update of the structures is executed
          // by the function below
          age_groups_ht = parse_file_and_update_structures(dir_name, file_path,
                                                           direntp->d_name);
          // Create a statistics entry for the current file
          // Store each statistics entry to a global list in order to send them via
          // fifo to the disease aggregator
          statistics_entry = statistics_entry_create(direntp->d_name, dir_name, age_groups_ht);
          list_push_back(&files_statistics, &statistics_entry);
        }
      }
    }
    closedir(dir_ptr);
  }
}

hash_table_ptr parse_file_and_update_structures(const char* dir_name,
                                                const char* file_path,
                                                const char* file_name) {
  char buffer[MAX_BUFFER_SIZE];
  char* patient_record_tokens[NO_PATIENT_RECORD_TOKENS];
  char** file_entry_tokens;
  int file_entry_no_tokens;
  wordexp_t p;
  patient_record_ptr patient_record;
  /* Open file for read only - handles binary fille too */
  FILE* fp = fopen(file_path, "rb+");
  /* age_groups_ht: disease_id --> array of int to store total cases per age group */
  hash_table_ptr age_groups_ht = hash_table_create(NO_BUCKETS, BUCKET_SIZE,
                                                   string_hash, string_compare,
                                                   string_print, age_groups_print,
                                                   NULL, age_groups_destroy);
  /* Read file line by line */
  while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    /* Discard '\n' that fgets() stores */
    buffer[strlen(buffer) - 1] = '\0';
    wordexp(buffer, &p, 0);
    file_entry_tokens = p.we_wordv;
    file_entry_no_tokens = p.we_wordc;
    char* status = file_entry_tokens[1];
    if (!strcmp(status,"ENTER")) {
      patient_record = patient_record_create(file_entry_tokens, file_name, dir_name);
      execute_insert_patient_record(patient_record, age_groups_ht);
    } else {
      execute_record_patient_exit(file_entry_tokens[0], file_name);
    }
    /* Free wordexp object */
    wordfree(&p);
  }
  /* Close file pointer */
  fclose(fp);
  /* return age_groups_ht of the file that just being parsed */
  return age_groups_ht;
}

static inline
void __handle_command(char* command) {
  wordexp_t p;
  char** command_tokens;
  int command_no_tokens;
  char** command_argv;
  int command_argc;
  /* Use API POSIX to extract arguments */
  wordexp(command, &p, 0);
  command_tokens = p.we_wordv;
  command_no_tokens = p.we_wordc;
  /* Call correspoding command function */
  if (!strcmp(command_tokens[0], "/diseaseFrequency")) {
    command_argv = prune_command_name(command_tokens, command_no_tokens);
    command_argc = command_no_tokens - 1;
    execute_disease_frequency(command_argc, command_argv);
    __FREE(command_argv);
  } else if (!strcmp(command_tokens[0], "/topk-AgeRanges")) {
    command_argv = prune_command_name(command_tokens, command_no_tokens);
    execute_topk_age_ranges(command_argv);
  } else if (!strcmp(command_tokens[0], "/searchPatientRecord")) {
    command_argv = prune_command_name(command_tokens, command_no_tokens);
    execute_search_patient_record(command_argv);
    __FREE(command_argv);
  } else if (!strcmp(command_tokens[0], "/numPatientAdmissions")) {
    command_argv = prune_command_name(command_tokens, command_no_tokens);
    command_argc = command_no_tokens - 1;
    execute_num_patients_admissions(command_argc, command_argv);
    __FREE(command_argv);
  } else if (!strcmp(command_tokens[0], "/numPatientDischarges")) {
    command_argv = prune_command_name(command_tokens, command_no_tokens);
    command_argc = command_no_tokens - 1;
    execute_num_patients_discharges(command_argc, command_argv);
    __FREE(command_argv);
  } else if (!strcmp(command_tokens[0], "/exit")) {
    wordfree(&p);
    execute_exit(0);
  }
  /* Free wordexp object */
  wordfree(&p);
}

void worker_main_loop(void) {
  /* Register worker's signals handlers */
  worker_register_handlers();
  /* Enter worker main loop */
  while (1) {
    /* Check for signal interrupt */
    if (setjmp(interrupt) == 1) {
      execute_exit(1);
    }
    /* Otherwise wait to read a command from aggregator */
    char* command = read_in_chunks(parameters.read_fd, parameters.buffer_size);
    __handle_command(command);
  }
}
