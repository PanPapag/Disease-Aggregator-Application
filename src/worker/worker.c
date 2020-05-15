#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../includes/worker/io_utils.h"

int main(int argc, char* argv[]) {
  char* dir_name = "../input_dir/Argentina";
  parse_directory(dir_name);
  return EXIT_SUCCESS;
}
