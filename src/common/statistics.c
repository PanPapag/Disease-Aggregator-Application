#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../includes/common/io_utils.h"
#include "../../includes/common/list.h"
#include "../../includes/common/macros.h"
#include "../../includes/common/statistics.h"
#include "../../includes/common/hash_table.h"

statistics_entry_ptr statistics_entry_create(const char* file_name,
                                             const char* country,
                                             hash_table_ptr age_groups_ht) {
  /* Allocate memory for patient_record_ptr */
  statistics_entry_ptr statistics_entry = malloc(sizeof(*statistics_entry));
  if (statistics_entry == NULL) {
    report_error("Could not allocate memory for Statistics Entry. Exiting...");
    exit(EXIT_FAILURE);
  }
  /* Allocate memory for statistics_entry file_name member */
  statistics_entry->file_name = (char*) malloc((strlen(file_name) + 1) * sizeof(char));
  if (statistics_entry->file_name == NULL) {
    report_error("Could not allocate memory for Statistics Entry file_path member. Exiting...");
    exit(EXIT_FAILURE);
  }
  strcpy(statistics_entry->file_name, file_name);
  /* Allocate memory for statistics_entry country member */
  statistics_entry->country = (char*) malloc((strlen(country) + 1) * sizeof(char));
  if (statistics_entry->country == NULL) {
    report_error("Could not allocate memory for Statistics Entry file_path member. Exiting...");
    exit(EXIT_FAILURE);
  }
  strcpy(statistics_entry->country, country);
  /* Store hash_table pointer */
  statistics_entry->age_groups_ht = age_groups_ht;
  /* Return statistics_entry pointer */
  return statistics_entry;
}

void ptr_to_statistics_entry_destroy(void* v) {
  statistics_entry_ptr statistics_entry = *((statistics_entry_ptr*) v);
  if (v != NULL) {
    __FREE(statistics_entry->file_name);
    __FREE(statistics_entry->country);
    hash_table_clear(statistics_entry->age_groups_ht);
  }
  __FREE(statistics_entry);
}

void ptr_to_statistics_entry_print(void* v, FILE* out) {
  statistics_entry_ptr statistics_entry = *((statistics_entry_ptr*) v);
  fprintf(out, "%s\n", statistics_entry->file_name);
  fprintf(out, "%s\n\n", statistics_entry->country);
  hash_table_print(statistics_entry->age_groups_ht, out);
  fprintf(out, "------------------------------\n\n");
}

int get_age_group(uint8_t age) {
  switch (age) {
    case 0 ... 20:
      return AGE_GROUP_1;
    case 21 ... 40:
      return AGE_GROUP_2;
    case 41 ... 60:
      return AGE_GROUP_3;
    case 61 ... 120:
      return AGE_GROUP_4;
  }
  return -1;
}

void age_groups_print(void* v, FILE* out) {
  int* age_groups = (int*) v;
  fprintf(out, "Age range 0-20 years: %d cases\n", age_groups[0]);
  fprintf(out, "Age range 21-40 years: %d cases\n", age_groups[1]);
  fprintf(out, "Age range 41-60 years: %d cases\n", age_groups[2]);
  fprintf(out, "Age range 60+ years: %d cases\n\n", age_groups[3]);
}

void age_groups_destroy(void* v) {
  int* age_groups = (int*) v;
  if (age_groups != NULL) {
    __FREE(age_groups);
  }
}
