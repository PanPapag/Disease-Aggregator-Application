#include <stdio.h>
#include <stdlib.h>

#include "../../includes/aggregator/io_utils.h"

program_parameters_t parameters;

int main(int argc, char* argv[]) {
  /* Parse command line arguments and update program parameters */
  parse_arguments(argc, argv);

  return EXIT_SUCCESS;
}
