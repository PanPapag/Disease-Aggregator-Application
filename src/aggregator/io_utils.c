#include <fcntl.h>
#include <getopt.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wordexp.h>

#include "../../includes/common/macros.h"
#include "../../includes/common/io_utils.h"
#include "../../includes/common/utils.h"
#include "../../includes/aggregator/io_utils.h"

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
  if (argc < 7) __usage();
  int c;
  while (1) {
    int option_index;
    c = getopt_long_only(argc, argv, "w:b:i:h", options, &option_index);
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
