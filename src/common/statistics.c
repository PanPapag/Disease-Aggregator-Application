#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../includes/common/io_utils.h"
#include "../../includes/common/list.h"
#include "../../includes/common/macros.h"
#include "../../includes/common/statistics.h"
#include "../../includes/worker/hash_table.h"

list_ptr diseases_names;

statistics_ptr statistics_create(const char* file_name, const char* country,
                                 hash_table_ptr age_groups_ht) {
  /* Allocate memory for patient_record_ptr */
  statistics_ptr statistics = malloc(sizeof(*statistics));
  if (statistics == NULL) {
    report_error("Could not allocate memory for Statistics. Exiting...");
    exit(EXIT_FAILURE);
  }
  /* Allocate memory for statistics file_name member */
  statistics->file_name = (char*) malloc((strlen(file_name) + 1) * sizeof(char));
  if (statistics->file_name == NULL) {
    report_error("Could not allocate memory for Statistics file_path member. Exiting...");
    exit(EXIT_FAILURE);
  }
  strcpy(statistics->file_name, file_name);
  /* Allocate memory for statistics country member */
  statistics->country = (char*) malloc((strlen(country) + 1) * sizeof(char));
  if (statistics->country == NULL) {
    report_error("Could not allocate memory for Statistics file_path member. Exiting...");
    exit(EXIT_FAILURE);
  }
  strcpy(statistics->country, country);
  /* Store hash_table pointer */
  statistics->age_groups_ht = age_groups_ht;
  /* Return statistics pointer */
  return statistics;
}

void statistics_print(void* v, FILE* out) {
  statistics_ptr statistics = (statistics_ptr) v;
  fprintf(out, "%s\n",statistics->file_name);
  fprintf(out, "%s\n\n",statistics->country);
  hash_table_print(statistics->age_groups_ht, out);
  fprintf(out, "------------------------------\n\n");
}

void age_groups_print(void* v, FILE* out) {
  int* age_groups = (int*) v;
  fprintf(out, "Age range 0-20 years: %d cases\n", age_groups[0]);
  fprintf(out, "Age range 21-40 years: %d cases\n", age_groups[1]);
  fprintf(out, "Age range 41-60 years: %d cases\n", age_groups[2]);
  fprintf(out, "Age range 60+ years: %d cases\n\n", age_groups[3]);
}
