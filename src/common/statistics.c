#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../includes/common/hash_table.h"
#include "../../includes/common/io_utils.h"
#include "../../includes/common/list.h"
#include "../../includes/common/macros.h"
#include "../../includes/common/statistics.h"
#include "../../includes/common/utils.h"

list_ptr diseases_names;

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

char* ptr_to_statistics_entry_serialize(void* v) {
  statistics_entry_ptr statistics_entry = *((statistics_entry_ptr*) v);
  /*
   The serialized string is going to have the following format:
   file_name$country$disease_id1:group1@group2@group3@#group4$disease_id2:...
  */
  char buffer[NO_AGE_GROUPS][12];
  size_t serialized_statistics_entry_size = strlen(statistics_entry->file_name) + \
                                            strlen(statistics_entry->country) + 3;
  char* serialized_statistics_entry = (char*) malloc(serialized_statistics_entry_size);

  size_t pos = strlen(statistics_entry->file_name);
  strcpy(serialized_statistics_entry, statistics_entry->file_name);
  serialized_statistics_entry[pos++] = FIELDS_DEL;
  serialized_statistics_entry[pos] = '\0';
  pos += strlen(statistics_entry->country);
  strcat(serialized_statistics_entry, statistics_entry->country);
  serialized_statistics_entry[pos++] = FIELDS_DEL;
  serialized_statistics_entry[pos] = '\0';

  for (size_t i = 1U; i <= list_size(diseases_names); ++i) {
    list_node_ptr list_node = list_get(diseases_names, i);
    char* disease_id = *((char**) list_node->data_);
    void* result = hash_table_find(statistics_entry->age_groups_ht, disease_id);
    if (result != NULL) {
      int* cases_per_age_group = (int*) result;

      pos += strlen(disease_id);
      serialized_statistics_entry_size += strlen(disease_id) + 2;
      for (size_t j = 0U; j < NO_AGE_GROUPS; ++j) {
        sprintf(buffer[j], "%d", cases_per_age_group[j]);
        serialized_statistics_entry_size += strlen(buffer[j]) + 1;
      }
      serialized_statistics_entry = (char*) realloc(serialized_statistics_entry,
                                                    serialized_statistics_entry_size);

      strcat(serialized_statistics_entry, disease_id);
      serialized_statistics_entry[pos++] = DISEASE_DEL;
      serialized_statistics_entry[pos] = '\0';
      for (size_t j = 0U; j < NO_AGE_GROUPS; ++j) {
        pos += strlen(buffer[j]);
        strcat(serialized_statistics_entry, buffer[j]);
        if (j !=  NO_AGE_GROUPS - 1) {
          serialized_statistics_entry[pos++] = AGE_GROUPS_DEL;
          serialized_statistics_entry[pos] = '\0';
        } else {
          serialized_statistics_entry[pos++] = FIELDS_DEL;
          serialized_statistics_entry[pos] = '\0';
        }
      }
    }
  }
  return serialized_statistics_entry;
}

static inline
void __serialized_cases_token_print(char* serialized_cases_token) {
  char* cases_token = strtok(serialized_cases_token, "@");
  int counter = 0;
  while (cases_token != NULL) {
    switch (counter) {
      case AGE_GROUP_1:
        printf("Age range 0-20 years: %s cases\n", cases_token);
        break;
      case AGE_GROUP_2:
        printf("Age range 21-40 years: %s cases\n", cases_token);
        break;
      case AGE_GROUP_3:
        printf("Age range 41-60 years: %s cases\n", cases_token);
        break;
      case AGE_GROUP_4:
        printf("Age range 60+ years: %s cases\n", cases_token);
        break;
    }
    counter++;
    cases_token = strtok(NULL, "@");
  }
}

static inline
void __serialized_disease_token_print(char* serialized_disease_token) {
  char* disease_id = strtok(serialized_disease_token, ":");
  printf("\n%s\n",disease_id);
  char* num_cases_token = strtok(NULL, ":");
  __serialized_cases_token_print(num_cases_token);
}

void serialized_statistics_entry_print(char* serialized_statistics_entry) {
  /* Store in a list all the field tokens */
  list_ptr field_tokens = list_create(STRING*, ptr_to_string_compare,
                                      ptr_to_string_print, NULL);

  char* field_token = strtok(serialized_statistics_entry, "$");
  while (field_token != NULL) {
    list_push_back(&field_tokens, &field_token);
    field_token = strtok(NULL, "$");
  }

  for (size_t i = 1U; i <= list_size(field_tokens); ++i) {
    list_node_ptr list_node = list_get(field_tokens, i);
    char* data = (*(char**) list_node->data_);
    if (i <= 2) {
      printf("%s\n", data);
    } else {
      __serialized_disease_token_print(data);
    }
  }
  printf("\n-------------------------------------\n\n");
  /* Clear memory allocated by temporary list field_tokens */
  list_clear(field_tokens);
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

char* get_age_group_name(int age_group_index) {
  char* age_group_name = NULL;
  switch (age_group_index) {
    case AGE_GROUP_1:
      age_group_name = "0-20";
      break;
    case AGE_GROUP_2:
      age_group_name = "21-40";
      break;
    case AGE_GROUP_3:
      age_group_name = "41-60";
      break;
    case AGE_GROUP_4:
      age_group_name = "60+";
      break;
  }
  return age_group_name;
}

int age_groups_stats_compare(void* a, void* b) {
  age_groups_stats_ptr ags1 = (age_groups_stats_ptr) a;
  age_groups_stats_ptr ags2 = (age_groups_stats_ptr) b;
  return ags1->no_patients - ags2->no_patients;
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
