#include <ctype.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>

#include "../../includes/common/hash_table.h"
#include "../../includes/common/macros.h"
#include "../../includes/common/list.h"
#include "../../includes/common/io_utils.h"
#include "../../includes/common/utils.h"
#include "../../includes/aggregator/commands.h"
#include "../../includes/aggregator/utils.h"

hash_table_ptr country_to_pid_ht;
list_ptr countries_names;

program_parameters_t parameters;

int validate_list_countries(int argc, char** argv) {
  return argc == 1 ? VALID_COMMAND : INVALID_COMMAND;
}

void execute_list_countries(void) {
  for (size_t i = 1U; i <= list_size(countries_names); ++i) {
    list_node_ptr list_node = list_get(countries_names, i);
    char* country = (*(char**) list_node->data_);
    void* result = hash_table_find(country_to_pid_ht, country);
    if (result != NULL) {
      pid_t worker_pid = (*(pid_t*) result);
      printf("%s %d\n",country, worker_pid);
    }
  }
}

int validate_disease_frequency(int argc, char** argv) {
  if (argc != 4 && argc != 5) {
    return INVALID_COMMAND;
  } else {
    if (argc == 4) {
      // Check virus name to contain only letters, numbers and possibly '-'
      char* virus_name = argv[1];
      for (size_t i = 0; i < strlen(virus_name); ++i) {
        if (!isalnum(virus_name[i]) && virus_name[i] != '-')
          return INVALID_COMMAND;
      }
      // Check date1
      char* date1 = argv[2];
      if (!is_valid_date_string(date1))
        return INVALID_COMMAND;
      // Check date2
      char* date2 = argv[3];
      if (!is_valid_date_string(date2))
        return INVALID_COMMAND;
    } else {
      // Check virus name to contain only letters, numbers and possibly '-'
      char* virus_name = argv[1];
      for (size_t i = 0; i < strlen(virus_name); ++i) {
        if (!isalnum(virus_name[i]) && virus_name[i] != '-')
          return INVALID_COMMAND;
      }
      // Check date1
      char* date1 = argv[2];
      if (!is_valid_date_string(date1))
        return INVALID_COMMAND;
      // Check date2
      char* date2 = argv[3];
      if (!is_valid_date_string(date2))
        return INVALID_COMMAND;
      // Check country to contain only letters
      char* country = argv[4];
      if (!is_alphabetical(country))
        return INVALID_COMMAND;
    }
  }
  return VALID_COMMAND;
}


void aggregate_disease_frequency(int argc, char** argv, char* command) {
  /*
    If country specified send the command only to the worker who is handling the
    correspoding country directory.
    Otherwise, push the command to every worker
  */
  int total_disease_cases = 0;
  if (argc == 4) {
    void* result = hash_table_find(country_to_pid_ht, argv[argc-1]);
    if (result != NULL) {
      pid_t worker_pid = (*(pid_t*) result);
      int pos = get_worker_fds_pos(worker_pid);
      write_in_chunks(parameters.workers_fd_1[pos], command, parameters.buffer_size);
      char* result = read_in_chunks(parameters.workers_fd_2[pos], parameters.buffer_size);
      if (strcmp(result, NO_RESPONSE)) {
        total_disease_cases += atoi(result);
      }
      __FREE(result);
    }
  } else {
    for (size_t i = 0U; i < parameters.num_workers; ++i) {
      write_in_chunks(parameters.workers_fd_1[i], command, parameters.buffer_size);
      char* result = read_in_chunks(parameters.workers_fd_2[i], parameters.buffer_size);
      if (strcmp(result, NO_RESPONSE)) {
        total_disease_cases += atoi(result);
      }
      __FREE(result);
    }
  }
  printf("%d\n", total_disease_cases);
}

int validate_topk_age_ranges(int argc, char** argv) {
  if (argc != 6) {
    return INVALID_COMMAND;
  } else {
    // Check k
    char* k = argv[1];
    if (!is_number(k))
      return INVALID_COMMAND;
    // Check country to contain only letters
    char* country = argv[2];
    if (!is_alphabetical(country))
      return INVALID_COMMAND;
    // Check virus name to contain only letters, numbers and possibly '-'
    char* virus_name = argv[3];
    for (size_t i = 0; i < strlen(virus_name); ++i) {
      if (!isalnum(virus_name[i]) && virus_name[i] != '-')
        return INVALID_COMMAND;
    }
    // Check date1
    char* date1 = argv[4];
    if (!is_valid_date_string(date1))
      return INVALID_COMMAND;
    // Check date2
    char* date2 = argv[5];
    if (!is_valid_date_string(date2))
      return INVALID_COMMAND;
  }
  return VALID_COMMAND;
}

void aggregate_topk_age_ranges(char** argv, char* command) {
  int k = atoi(argv[0]);
  char* country_name = argv[1];
  void* result = hash_table_find(country_to_pid_ht, country_name);
  if (result != NULL) {
    pid_t worker_pid = (*(pid_t*) result);
    int pos = get_worker_fds_pos(worker_pid);
    write_in_chunks(parameters.workers_fd_1[pos], command, parameters.buffer_size);
    for (size_t i = 0U; i < k; ++i) {
      char* result = read_in_chunks(parameters.workers_fd_2[pos], parameters.buffer_size);
      if (strcmp(result, NO_RESPONSE)) {
        printf("%s\n", result);
      }
      __FREE(result);
    }
  }
}

int validate_search_patient_record(int argc, char** argv) {
  return argc == 2 ? VALID_COMMAND : INVALID_COMMAND;
}

void aggregate_search_patient_record(char* command) {
  for (size_t i = 0U; i < parameters.num_workers; ++i) {
    write_in_chunks(parameters.workers_fd_1[i], command, parameters.buffer_size);
    char* result = read_in_chunks(parameters.workers_fd_2[i], parameters.buffer_size);
    if (strcmp(result, NO_RESPONSE)) {
      printf("%s\n", result);
    }
    __FREE(result);
  }
}

int validate_num_patient_admissions(int argc, char** argv) {
  if (argc != 4 && argc != 5) {
    return INVALID_COMMAND;
  } else {
    if (argc == 4) {
      // Check virus name to contain only letters, numbers and possibly '-'
      char* virus_name = argv[1];
      for (size_t i = 0; i < strlen(virus_name); ++i) {
        if (!isalnum(virus_name[i]) && virus_name[i] != '-')
          return INVALID_COMMAND;
      }
      // Check date1
      char* date1 = argv[2];
      if (!is_valid_date_string(date1))
        return INVALID_COMMAND;
      // Check date2
      char* date2 = argv[3];
      if (!is_valid_date_string(date2))
        return INVALID_COMMAND;
    } else {
      // Check virus name to contain only letters, numbers and possibly '-'
      char* virus_name = argv[1];
      for (size_t i = 0; i < strlen(virus_name); ++i) {
        if (!isalnum(virus_name[i]) && virus_name[i] != '-')
          return INVALID_COMMAND;
      }
      // Check date1
      char* date1 = argv[2];
      if (!is_valid_date_string(date1))
        return INVALID_COMMAND;
      // Check date2
      char* date2 = argv[3];
      if (!is_valid_date_string(date2))
        return INVALID_COMMAND;
      // Check country to contain only letters
      char* country = argv[4];
      if (!is_alphabetical(country))
        return INVALID_COMMAND;
    }
  }
  return VALID_COMMAND;
}

void aggregate_num_patient_admissions(int argc, char** argv, char* command) {
  /*
    If country specified send the command only to the worker who is handling the
    correspoding country directory.
    Otherwise, push the command to every worker
  */
  if (argc == 4) {
    void* result = hash_table_find(country_to_pid_ht, argv[argc-1]);
    if (result != NULL) {
      pid_t worker_pid = (*(pid_t*) result);
      int pos = get_worker_fds_pos(worker_pid);
      write_in_chunks(parameters.workers_fd_1[pos], command, parameters.buffer_size);
      char* result = read_in_chunks(parameters.workers_fd_2[pos], parameters.buffer_size);
      if (strcmp(result, NO_RESPONSE)) {
        printf("%s %d\n", argv[argc-1], atoi(result));
      }
      __FREE(result);
    }
  } else {
    for (size_t i = 0U; i < parameters.num_workers; ++i) {
      write_in_chunks(parameters.workers_fd_1[i], command, parameters.buffer_size);
      char* num_reads_buffer = read_in_chunks(parameters.workers_fd_2[i], parameters.buffer_size);
      int num_reads = atoi(num_reads_buffer);
      for (size_t j = 0U; j < num_reads; ++j) {
        char* result = read_in_chunks(parameters.workers_fd_2[i], parameters.buffer_size);
        if (strcmp(result, NO_RESPONSE)) {
          printf("%s\n", result);
        }
        __FREE(result);
      }
      __FREE(num_reads_buffer);
    }
  }
}

int validate_num_patient_discharges(int argc, char** argv) {
  if (argc != 4 && argc != 5) {
    return INVALID_COMMAND;
  } else {
    if (argc == 4) {
      // Check virus name to contain only letters, numbers and possibly '-'
      char* virus_name = argv[1];
      for (size_t i = 0; i < strlen(virus_name); ++i) {
        if (!isalnum(virus_name[i]) && virus_name[i] != '-')
          return INVALID_COMMAND;
      }
      // Check date1
      char* date1 = argv[2];
      if (!is_valid_date_string(date1))
        return INVALID_COMMAND;
      // Check date2
      char* date2 = argv[3];
      if (!is_valid_date_string(date2))
        return INVALID_COMMAND;
    } else {
      // Check virus name to contain only letters, numbers and possibly '-'
      char* virus_name = argv[1];
      for (size_t i = 0; i < strlen(virus_name); ++i) {
        if (!isalnum(virus_name[i]) && virus_name[i] != '-')
          return INVALID_COMMAND;
      }
      // Check date1
      char* date1 = argv[2];
      if (!is_valid_date_string(date1))
        return INVALID_COMMAND;
      // Check date2
      char* date2 = argv[3];
      if (!is_valid_date_string(date2))
        return INVALID_COMMAND;
      // Check country to contain only letters
      char* country = argv[4];
      if (!is_alphabetical(country))
        return INVALID_COMMAND;
    }
  }
  return VALID_COMMAND;
}

void aggregate_num_patient_discharges(int argc, char** argv, char* command) {
  /*
    If country specified send the command only to the worker who is handling the
    correspoding country directory.
    Otherwise, push the command to every worker
  */
  if (argc == 4) {
    void* result = hash_table_find(country_to_pid_ht, argv[argc-1]);
    if (result != NULL) {
      pid_t worker_pid = (*(pid_t*) result);
      int pos = get_worker_fds_pos(worker_pid);
      write_in_chunks(parameters.workers_fd_1[pos], command, parameters.buffer_size);
      char* result = read_in_chunks(parameters.workers_fd_2[pos], parameters.buffer_size);
      if (strcmp(result, NO_RESPONSE)) {
        printf("%s %d\n", argv[argc-1], atoi(result));
      }
      __FREE(result);
    }
  } else {
    for (size_t i = 0U; i < parameters.num_workers; ++i) {
      write_in_chunks(parameters.workers_fd_1[i], command, parameters.buffer_size);
      char* num_reads_buffer = read_in_chunks(parameters.workers_fd_2[i], parameters.buffer_size);
      int num_reads = atoi(num_reads_buffer);
      for (size_t j = 0U; j < num_reads; ++j) {
        char* result = read_in_chunks(parameters.workers_fd_2[i], parameters.buffer_size);
        if (strcmp(result, NO_RESPONSE)) {
          printf("%s\n", result);
        }
        __FREE(result);
      }
    }
  }
}

int validate_exit(int argc, char** argv) {
  return argc == 1 ? VALID_COMMAND : INVALID_COMMAND;
}

void aggregate_exit(char* command) {
  parameters.terminate_flag = 1;
  /* Send exit command to workers and kill them */
  char* result;
  int total_success_cnt = 0;
  int total_fail_cnt = 0;
  for (size_t i = 0U; i < parameters.num_workers; ++i) {
    write_in_chunks(parameters.workers_fd_1[i], command, parameters.buffer_size);
    /* Read success_cnt from current worker */
    result = read_in_chunks(parameters.workers_fd_2[i], parameters.buffer_size);
    total_success_cnt += atoi(result);
    __FREE(result);
    /* Read fail_cnt from current worker */
    result = read_in_chunks(parameters.workers_fd_2[i], parameters.buffer_size);
    total_fail_cnt += atoi(result);
    __FREE(result);
    /* Send SIGKILL signal */
    kill(parameters.workers_pid[i], SIGKILL);
  }
  /* Create and write log file */
  write_log_file(countries_names, total_success_cnt, total_fail_cnt);
  /* Close file descriptors and delete named pipes */
  char fifo_1[parameters.num_workers][15], fifo_2[parameters.num_workers][15];
  for (size_t i = 0U; i < parameters.num_workers; ++i) {
    close(parameters.workers_fd_1[i]);
    close(parameters.workers_fd_1[i]);
    free(parameters.worker_dir_paths[i]);
    sprintf(fifo_1[i], "pw_cr_%d", parameters.workers_pid[i]);
    sprintf(fifo_2[i], "pr_cw_%d", parameters.workers_pid[i]);
    unlink(fifo_1[i]);
    unlink(fifo_2[i]);
  }
  /* Clear memory */
  free(parameters.worker_dir_paths);
  free(parameters.workers_fd_1);
  free(parameters.workers_fd_2);
  free(parameters.workers_pid);
  list_clear(countries_names);
  hash_table_clear(country_to_pid_ht);
  /* Exit Success */
  exit(EXIT_SUCCESS);
}
