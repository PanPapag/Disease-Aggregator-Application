#include <ctype.h>
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
    }
  } else {
    for (size_t i = 0U; i < parameters.num_workers; ++i) {
      write_in_chunks(parameters.workers_fd_1[i], command, parameters.buffer_size);
      char* result = read_in_chunks(parameters.workers_fd_2[i], parameters.buffer_size);
      if (strcmp(result, NO_RESPONSE)) {
        total_disease_cases += atoi(result);
      }
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
    }
  } else {
    for (size_t i = 0U; i < parameters.num_workers; ++i) {
      write_in_chunks(parameters.workers_fd_1[i], command, parameters.buffer_size);
      char* result = read_in_chunks(parameters.workers_fd_2[i], parameters.buffer_size);
      if (strcmp(result, NO_RESPONSE)) {
        total_disease_cases += atoi(result);
      }
    }
  }
  printf("%d\n", total_disease_cases);
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
    }
  } else {
    for (size_t i = 0U; i < parameters.num_workers; ++i) {
      write_in_chunks(parameters.workers_fd_1[i], command, parameters.buffer_size);
      char* result = read_in_chunks(parameters.workers_fd_2[i], parameters.buffer_size);
      if (strcmp(result, NO_RESPONSE)) {
        total_disease_cases += atoi(result);
      }
    }
  }
  printf("%d\n", total_disease_cases);
}

int validate_exit(int argc, char** argv) {
  return argc == 1 ? VALID_COMMAND : INVALID_COMMAND;
}
