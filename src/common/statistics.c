#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../includes/common/io_utils.h"
#include "../../includes/common/list.h"
#include "../../includes/common/macros.h"
#include "../../includes/common/statistics.h"

statistics_ptr statistics_create(char* file_path) {
  /* Allocate memory for patient_record_ptr */
  statistics_ptr statistics = malloc(sizeof(*statistics));
  if (statistics == NULL) {
    report_error("Could not allocate memory for Statistics. Exiting...");
    exit(EXIT_FAILURE);
  }
  /* Allocate memory for statistics file_path member */
  statistics->file_path = (char*) malloc((strlen(file_path) + 1) * sizeof(char));
  if (statistics->file_path == NULL) {
    report_error("Could not allocate memory for Statistics file_path member. Exiting...");
    exit(EXIT_FAILURE);
  }
  strcpy(statistics->file_path, file_path);
  /* Initialize the list which will hold the data for each disease */

  /* Return statistics pointer */
  return statistics;
}

void statistics_print(void* v, FILE* out) {
  statistics_ptr statistics = (statistics_ptr) v;
  fprintf(out, "%s\n",statistics->file_path);
}
