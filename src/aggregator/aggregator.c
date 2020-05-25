#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../includes/common/list.h"
#include "../../includes/aggregator/io_utils.h"

program_parameters_t parameters;

int main(int argc, char* argv[]) {
  /* Parse command line arguments and update program parameters */
  parse_arguments(&argc, argv);

  printf("%zu\n", parameters.num_workers);
  printf("%zu\n", parameters.buffer_size);
  printf("%s\n", parameters.input_dir);
  printf("------------------------\n");

  list_ptr subdirs = get_all_subdirs(parameters.input_dir);

  list_print(subdirs, stdout);
  list_clear(subdirs);

  return EXIT_SUCCESS;
}
