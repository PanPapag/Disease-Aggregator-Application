#include <fcntl.h>
#include <getopt.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wordexp.h>

#include "../../includes/common/macros.h"
#include "../../includes/common/io_utils.h"
#include "../../includes/common/utils.h"
#include "../../includes/aggregator/commands.h"
#include "../../includes/aggregator/io_utils.h"
#include "../../includes/aggregator/utils.h"

program_parameters_t parameters;

static struct option options[] = {
      {"w",    required_argument, NULL, 'w'},
      {"b",    required_argument, NULL, 'b'},
      {"i",    required_argument, NULL, 'i'},
      {"help", no_argument,       NULL, 'h'},
      {0, 0, 0,                         0}
};

static inline
void __usage() {
    fprintf(stderr,
            "You must provide the following arguments:\n"
            "\t-w <Number of Workers>\n"
            "\t-b <Buffer Size>\n"
            "\t-i <Input Directory>\n");
    exit(EXIT_FAILURE);
}

void parse_arguments(int* argc, char* argv[]) {
  if (*argc < 7) __usage();
  int c;
  while (1) {
    int option_index;
    c = getopt_long_only(*argc, argv, "w:b:i:h", options, &option_index);
    if (c == -1) break;
    switch (c) {
      case 'w': {
        size_t num_workers;
        if (!string_to_int64(optarg, (int64_t *) &num_workers)) {
          report_error("Invalid <Number of Workers> parameter. Exiting...");
          exit(EXIT_FAILURE);
        }
        parameters.num_workers = num_workers;
        break;
      }
      case 'b': {
        size_t buffer_size;
        if (!string_to_int64(optarg, (int64_t *) &buffer_size)) {
          report_error("Invalid <Buffer Size> parameter. Exiting...");
          exit(EXIT_FAILURE);
        }
        parameters.buffer_size = buffer_size;
        break;
      }
      case 'i': {
        parameters.input_dir = optarg;
        break;
      }
      case 'h':
        __usage();
      case '?':
        break;
      default:
        abort();
    }
  }
}

static inline
void __handle_command(char command[]) {
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
  if (!strcmp(command_tokens[0], "/listCountries")) {
    if (validate_list_countries(command_no_tokens, command_tokens)) {
      execute_list_countries();
    } else {
      report_warning("Invalid <%s> command.", command_tokens[0]);
      fprintf(stderr, "Usage: /listCountries\n");
    }
  } else if (!strcmp(command_tokens[0], "/diseaseFrequency")) {
    if (validate_disease_frequency(command_no_tokens, command_tokens)) {
      command_argv = prune_command_name(command_tokens, command_no_tokens);
      command_argc = command_no_tokens - 1;
      printf("DISEASE FREQUENCY CORRECT\n");
      // aggregate_disease_frequency(command_argc, command_argv);
      __FREE(command_argv);
    } else {
      report_warning("Invalid <%s> command.", command_tokens[0]);
      fprintf(stderr, "Usage: /diseaseFrequency disease date1 date2 [country]\n");
    }
  } else if (!strcmp(command_tokens[0], "/topk-AgeRanges")) {
    if (validate_topk_age_ranges(command_no_tokens, command_tokens)) {
      command_argv = prune_command_name(command_tokens, command_no_tokens);
      command_argc = command_no_tokens - 1;
      printf("TOPK-AGE_RANGES CORRECT\n");
      // execute_disease_frequency(command_argc, command_argv);
      __FREE(command_argv);
    } else {
      report_warning("Invalid <%s> command.", command_tokens[0]);
      fprintf(stderr, "Usage: /topk-AgeRanges k country disease date1 date2 \n");
    }
  } else if (!strcmp(command_tokens[0], "/searchPatientRecord")) {
    if (validate_search_patient_record(command_no_tokens, command_tokens)) {
      command_argv = prune_command_name(command_tokens, command_no_tokens);
      command_argc = command_no_tokens - 1;
      aggregate_search_patient_record(command);
      __FREE(command_argv);
    } else {
      report_warning("Invalid <%s> command.", command_tokens[0]);
      fprintf(stderr, "Usage: /searchPatientRecord recordID\n");
    }
  } else if (!strcmp(command_tokens[0], "/numPatientAdmissions")) {
    if (validate_num_patient_admissions(command_no_tokens, command_tokens)) {
      command_argv = prune_command_name(command_tokens, command_no_tokens);
      command_argc = command_no_tokens - 1;
      printf("NUM PATIENT ADMISSIONS CORRECT\n");
      // execute_disease_frequency(command_argc, command_argv);
      __FREE(command_argv);
    } else {
      report_warning("Invalid <%s> command.", command_tokens[0]);
      fprintf(stderr, "Usage: /numPatientAdmissions disease date1 date2 [country]\n");
    }
  } else if (!strcmp(command_tokens[0], "/numPatientDischarges")) {
    if (validate_num_patient_discharges(command_no_tokens, command_tokens)) {
      command_argv = prune_command_name(command_tokens, command_no_tokens);
      command_argc = command_no_tokens - 1;
      printf("NUM PATIENT DISCHARGES CORRECT\n");
      // execute_disease_frequency(command_argc, command_argv);
      __FREE(command_argv);
    } else {
      report_warning("Invalid <%s> command.", command_tokens[0]);
      fprintf(stderr, "Usage: /numPatientDischarges disease date1 date2 [country]\n");
    }
  } else if (!strcmp(command_tokens[0], "/exit")) {
    if (validate_exit(command_no_tokens, command_tokens)) {
      /* Free wordexp object */
      wordfree(&p);
      /* Exit the program */
      printf("EXIT CORRECT\n");
      // execute_exit();
    } else {
      report_warning("Invalid <%s> command.", command_tokens[0]);
      fprintf(stderr, "Usage: /exit\n");
    }
  } else {
    report_warning("Unknown command: <%s>.", command);
  }
  /* Print new line for better format in the console */
  printf("\n");
  /* Free wordexp object */
  wordfree(&p);
}

void main_loop(void) {
  char command[MAX_BUFFER_SIZE];
  while (1) {
    /* Read command from the stdin */
    printf("> ");
    memset(&command, 0, sizeof(command));
    fgets(command, MAX_BUFFER_SIZE, stdin);
    command[strlen(command) - 1] = '\0';
    /* Handle command and call correspodent function until exit will be given */
    __handle_command(command);
  }
}
