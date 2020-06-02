#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>

#include "../../includes/common/hash_table.h"
#include "../../includes/common/list.h"
#include "../../includes/common/macros.h"
#include "../../includes/common/io_utils.h"
#include "../../includes/common/statistics.h"
#include "../../includes/common/utils.h"
#include "../../includes/worker/avl.h"
#include "../../includes/worker/commands.h"
#include "../../includes/worker/heap.h"
#include "../../includes/worker/io_utils.h"
#include "../../includes/worker/patient_record.h"
#include "../../includes/worker/signals_handling.h"

hash_table_ptr country_ht;
hash_table_ptr disease_ht;
hash_table_ptr file_paths_ht;
hash_table_ptr patient_record_ht;

list_ptr countries_names;
list_ptr diseases_names;
list_ptr files_statistics;

worker_parameters_t parameters;

int success_cnt;
int fail_cnt;

static inline
void __count_patients_between(avl_node_ptr current_root,
                             struct tm (*cmp_field)(patient_record_ptr),
                             int* counter, struct tm date1, struct tm date2,
                             char* country) {

  avl_node_ptr temp = current_root;
  patient_record_ptr patient_record = NULL;
  /* Prune left search below this node as all entries have entry date < date1 */
  if (temp != NULL) {
    patient_record = (patient_record_ptr) temp->data_;
    if (date_tm_compare(date1, cmp_field(patient_record)) <= 0) {
      __count_patients_between(temp->left_, cmp_field, counter, date1, date2, country);
    }
    patient_record = (patient_record_ptr) temp->data_;
    /* Check if patient_record exit date is not specified */
    if (date_tm_compare(cmp_field(patient_record), date2) <= 0) {
      /* Check upper bound */
      if (country != NULL) {
        if (!strcmp(country, patient_record->country)) {
          /* Update counter */
          (*counter)++;
        }
      } else {
        /* Update counter */
        (*counter)++;
      }
    }
    __count_patients_between(temp->right_, cmp_field, counter, date1, date2, country);
  }
}

static inline
int __num_patients_between(avl_ptr disease_avl,
                           struct tm (*cmp_field)(patient_record_ptr),
                           char* date1, char* date2, char* country) {
  int counter = 0;
  /* Convert string dates to struct tm format */
  struct tm date1_tm;
  memset(&date1_tm, 0, sizeof(struct tm));
  strptime(date1, "%d-%m-%Y", &date1_tm);

  struct tm date2_tm;
  memset(&date2_tm, 0, sizeof(struct tm));
  strptime(date2, "%d-%m-%Y", &date2_tm);
  /*
    Traverse inorder the AVL Tree and each time a patient record is found
    between the given range increase the counter for current patients.
    We can prune the traversal under the node in which a patient record has
    entry date < date1 or exit date > date2.
  */
   __count_patients_between(disease_avl->root_, cmp_field, &counter, date1_tm, date2_tm, country);
  return counter;
}

void execute_disease_frequency(int argc, char** argv) {
  void* result = hash_table_find(disease_ht, argv[0]);
  if (result == NULL) {
    write_in_chunks(parameters.write_fd, NO_RESPONSE, parameters.buffer_size);
    fail_cnt++;
  } else {
    avl_ptr disease_avl = (avl_ptr) result;
    int num_patients;
    if (argc == 3){
      num_patients = __num_patients_between(disease_avl, get_entry_date, argv[1], argv[2], NULL);
    } else {
      num_patients = __num_patients_between(disease_avl, get_entry_date, argv[1], argv[2], argv[3]);
    }
    char num_patients_buffer[12];
    sprintf(num_patients_buffer, "%d", num_patients);
    write_in_chunks(parameters.write_fd, num_patients_buffer, parameters.buffer_size);
    success_cnt++;
  }
}

static inline
void __update_patients_per_group(avl_node_ptr current_root,
                                struct tm (*cmp_field)(patient_record_ptr),
                                struct tm date1, struct tm date2,
                                char* country, int** patients_per_group) {
  avl_node_ptr temp = current_root;
  patient_record_ptr patient_record = NULL;
  /* Prune left search below this node as all entries have entry date < date1 */
  if (temp != NULL) {
    patient_record = (patient_record_ptr) temp->data_;
    if (date_tm_compare(date1, cmp_field(patient_record)) <= 0) {
      __update_patients_per_group(temp->left_, cmp_field, date1, date2,
                                  country, patients_per_group);
    }
    patient_record = (patient_record_ptr) temp->data_;
    /* Check if patient_record exit date is not specified */
    if (date_tm_compare(cmp_field(patient_record), date2) <= 0) {
      /* Check upper bound */
      if (country != NULL) {
        if (!strcmp(country, patient_record->country)) {
          /* Update patients_per_group array */
          int pos = get_age_group(patient_record->age);
          (*patients_per_group)[pos]++;
        }
      }
    }
    __update_patients_per_group(temp->right_, cmp_field, date1, date2,
                                country, patients_per_group);
  }
}

static inline
void __util_execute_topk_age_ranges(avl_ptr disease_avl,
                                    struct tm (*cmp_field)(patient_record_ptr),
                                    char* date1, char* date2, char* country,
                                    int** patients_per_group) {
  /* Convert string dates to struct tm format */
  struct tm date1_tm;
  memset(&date1_tm, 0, sizeof(struct tm));
  strptime(date1, "%d-%m-%Y", &date1_tm);

  struct tm date2_tm;
  memset(&date2_tm, 0, sizeof(struct tm));
  strptime(date2, "%d-%m-%Y", &date2_tm);
  /*
    Traverse inorder the AVL Tree and each time a patient record is found
    between the given range increase the counter for current patients.
    We can prune the traversal under the node in which a patient record has
    entry date < date1 or exit date > date2.
  */
  __update_patients_per_group(disease_avl->root_, cmp_field, date1_tm, date2_tm,
                              country, patients_per_group);
}

void execute_topk_age_ranges(char** argv) {
  int k = atoi(argv[0]);
  char* country = argv[1];
  char* disease = argv[2];
  /* Get for the given country its AVL tree */
  void* result = hash_table_find(disease_ht, disease);
  if (result == NULL) {
    write_in_chunks(parameters.write_fd, NO_RESPONSE, parameters.buffer_size);
  } else {
    avl_ptr disease_avl = (avl_ptr) result;
    /* Store in a map like array num_patients per age group */
    int* patients_per_group = (int*) calloc(NO_AGE_GROUPS, sizeof(int));
    /* Update patients_per_group */
    __util_execute_topk_age_ranges(disease_avl, get_entry_date, argv[3], argv[4],
                                   country, &patients_per_group);
    /* Copy patients_per_group content into struct age_groups_stats_t */
    age_groups_stats_ptr age_groups_stats[NO_AGE_GROUPS];
    int total_num_patients = 0;
    for (int i = 0; i < NO_AGE_GROUPS; ++i) {
      age_groups_stats[i] = (age_groups_stats_ptr) malloc(sizeof(*age_groups_stats[i]));
      age_groups_stats[i]->age_group = i;
      age_groups_stats[i]->no_patients = patients_per_group[i];
      total_num_patients += patients_per_group[i];
    }
    __FREE(patients_per_group);
    /*
      Build on the fly a max heap and insert there the elements
      of the patients_per_group map array
    */
    heap_ptr heap = heap_create(age_groups_stats_compare, NULL, NULL);
    for (size_t i = 0U; i < NO_AGE_GROUPS; ++i) {
      heap_insert_max(&heap, age_groups_stats[i]);
    }
    /* Extract top k */
    for (size_t i = 0; i < k; ++i) {
      result = heap_extract_max(&heap);
      if (result != NULL) {
        age_groups_stats_ptr age_groups_stats_entry = (age_groups_stats_ptr) result;
        double age_group_per = (double) age_groups_stats_entry->no_patients / total_num_patients * 100;
        char output_buffer[MAX_BUFFER_SIZE];
        char* age_group_name = get_age_group_name(age_groups_stats_entry->age_group);
        sprintf(output_buffer, "%s: %0.2f%%", age_group_name, age_group_per);
        write_in_chunks(parameters.write_fd, output_buffer, parameters.buffer_size);
        success_cnt++;
      }
    }
    /* Clear memory allocated */
    heap_clear(heap);
    for (int i = 0; i < NO_AGE_GROUPS; ++i) {
      __FREE(age_groups_stats[i]);
    }
  }
}

void execute_search_patient_record(char** argv) {
  void* result = hash_table_find(patient_record_ht, argv[0]);
  if (result == NULL) {
    write_in_chunks(parameters.write_fd, NO_RESPONSE, parameters.buffer_size);
    fail_cnt++;
  } else {
    char* stringified_patient_record = patient_record_stringify(result);
    write_in_chunks(parameters.write_fd, stringified_patient_record, parameters.buffer_size);
    success_cnt++;
    __FREE(stringified_patient_record);
  }
}

void execute_num_patients_admissions(int argc, char** argv) {
  void* result = hash_table_find(disease_ht, argv[0]);
  if (result == NULL) {
    write_in_chunks(parameters.write_fd, NO_RESPONSE, parameters.buffer_size);
    fail_cnt++;
  } else {
    avl_ptr disease_avl = (avl_ptr) result;
    int num_patients;
    if (argc == 3){
      char num_writes_buffer[12];
      sprintf(num_writes_buffer, "%ld", list_size(countries_names));
      write_in_chunks(parameters.write_fd, num_writes_buffer, parameters.buffer_size);
      for (size_t i = 1U; i <= list_size(countries_names); ++i) {
        list_node_ptr list_node = list_get(countries_names, i);
        char* country = (*(char**) list_node->data_);
        num_patients = __num_patients_between(disease_avl, get_entry_date, argv[1], argv[2], country);
        char output_buffer[MAX_BUFFER_SIZE];
        sprintf(output_buffer, "%s %d", country, num_patients);
        write_in_chunks(parameters.write_fd, output_buffer, parameters.buffer_size);
        success_cnt++;
      }
    } else {
      num_patients = __num_patients_between(disease_avl, get_entry_date, argv[1], argv[2], argv[3]);
      char num_patients_buffer[12];
      sprintf(num_patients_buffer, "%d", num_patients);
      write_in_chunks(parameters.write_fd, num_patients_buffer, parameters.buffer_size);
      success_cnt++;
    }
  }
}

void execute_num_patients_discharges(int argc, char** argv) {
  void* result = hash_table_find(disease_ht, argv[0]);
  if (result == NULL) {
    write_in_chunks(parameters.write_fd, NO_RESPONSE, parameters.buffer_size);
    fail_cnt++;
  } else {
    avl_ptr disease_avl = (avl_ptr) result;
    int num_patients;
    if (argc == 3){
      char num_writes_buffer[12];
      sprintf(num_writes_buffer, "%ld", list_size(countries_names));
      write_in_chunks(parameters.write_fd, num_writes_buffer, parameters.buffer_size);
      for (size_t i = 1U; i <= list_size(countries_names); ++i) {
        list_node_ptr list_node = list_get(countries_names, i);
        char* country = (*(char**) list_node->data_);
        num_patients = __num_patients_between(disease_avl, get_exit_date, argv[1], argv[2], country);
        char output_buffer[MAX_BUFFER_SIZE];
        sprintf(output_buffer, "%s %d", country, num_patients);
        write_in_chunks(parameters.write_fd, output_buffer, parameters.buffer_size);
        success_cnt++;
      }
    } else {
      num_patients = __num_patients_between(disease_avl, get_exit_date, argv[1], argv[2], argv[3]);
      char num_patients_buffer[12];
      sprintf(num_patients_buffer, "%d", num_patients);
      write_in_chunks(parameters.write_fd, num_patients_buffer, parameters.buffer_size);
      success_cnt++;
    }
  }
}

int execute_insert_patient_record(patient_record_ptr patient_record,
                                  hash_table_ptr age_groups_ht) {
  void* result = hash_table_find(patient_record_ht, patient_record->record_id);
  /* If record Id not found */
  if (result == NULL) {
    /* Update patient record hash table */
    hash_table_insert(&patient_record_ht, patient_record->record_id, patient_record);
    /* Get the age group of the patient_record which just inserted */
    int age_group_pos = get_age_group(patient_record->age);
    /* Search if patient record disease_id exists in age_groups_ht */
    result = hash_table_find(age_groups_ht, patient_record->disease_id);
    if (result == NULL) {
      /*
        Create an array of size NO_AGE_GROUPS to store total number of cases
        per age group
      */
      int* cases_per_age_group = (int*) calloc(NO_AGE_GROUPS, sizeof(int));
      /* Update cases for the current age group with the given disease id */
      cases_per_age_group[age_group_pos]++;
      /* Insert to age_groups_ht */
      hash_table_insert(&age_groups_ht, patient_record->disease_id, cases_per_age_group);
    } else {
      /* Update the total number of cases in current age group with the given disease id */
      int* cases_per_age_group = (int*) result;
      cases_per_age_group[age_group_pos]++;
    }
    /* Search if patient record disease_id exists in disease_ht */
    result = hash_table_find(disease_ht, patient_record->disease_id);
    if (result == NULL) {
      /* Store disease to global diseases_names list */
      list_push_front(&diseases_names, &patient_record->disease_id);
      /* If not found create a new AVL tree to store pointers to patient record */
      avl_ptr new_disease_avl = avl_create(patient_record_compare,
                                           patient_record_print);
      avl_insert(&new_disease_avl, patient_record);
      /* Update disease hash table */
      hash_table_insert(&disease_ht, patient_record->disease_id, new_disease_avl);
    } else {
      /* Update disease hash table by insert patient_record pointer to AVL tree */
      avl_ptr disease_avl = (avl_ptr) result;
      avl_insert(&disease_avl, patient_record);
    }
    /* Search if patient record country exists in country_ht */
    result = hash_table_find(country_ht, patient_record->country);
    if (result == NULL) {
      /* Store country to global countries_names list */
      list_push_front(&countries_names, &patient_record->country);
      /* If not found create a new AVL tree to store pointers to patient record */
      avl_ptr new_country_avl = avl_create(patient_record_compare,
                                           patient_record_print);
      avl_insert(&new_country_avl, patient_record);
      /* Update country hash table */
      hash_table_insert(&country_ht, patient_record->country, new_country_avl);
    } else {
      /* Update country hash table by insert patient_record pointer to AVL tree */
      avl_ptr country_avl = (avl_ptr) result;
      avl_insert(&country_avl, patient_record);
    }
  } else {
    report_warning("Patient record with Record ID: <%s> already exists. ",
                  patient_record->record_id);
    /* Delete patient record and return */
    patient_record_destroy(patient_record);
    fail_cnt++;
    return ERROR;
  }
  success_cnt++;
  return PASS;
}

int execute_record_patient_exit(char* id, const char* exit_date) {
  /* Search if patient record id exists */
  void* result = hash_table_find(patient_record_ht, id);
  if (result == NULL) {
    report_warning("Patient record with Record ID: <%s> not found.", id);
    fail_cnt++;
    return ERROR;
  } else {
    // Cast result to patient record pointer
    patient_record_ptr patient_record = (patient_record_ptr) result;
    /* Convert exit_date struct tm to buffer */
    char exit_date_buffer[MAX_BUFFER_SIZE];
    strftime(exit_date_buffer, sizeof(exit_date_buffer), "%d-%m-%Y", &patient_record->exit_date);
    /* Convert entry_date struct tm to buffer */
    char entry_date_buffer[MAX_BUFFER_SIZE];
    strftime(entry_date_buffer, sizeof(entry_date_buffer), "%d-%m-%Y", &patient_record->entry_date);
    /* exit_date has to be greater than entry_date */
    if (date_string_compare(entry_date_buffer, exit_date) > 0) {
      report_warning("Entry Date [%s] is after the given Exit Date [%s].",
                      entry_date_buffer, exit_date);
      fail_cnt++;
      return ERROR;
    }
    /* Check if exit date is not specified */
    if (!strcmp(exit_date_buffer, EXIT_DATE_NOT_SPECIFIED)) {
      // Update exit day of patient record with the given record id
      memset(&patient_record->exit_date, 0, sizeof(struct tm));
      strptime(exit_date, "%d-%m-%Y", &patient_record->exit_date);
    } else {
      report_warning("Patient record Exit Date with Record ID: "
                     "<%s> is already specified.", id);
      fail_cnt++;
      return ERROR;
    }
  }
  success_cnt++;
  return PASS;
}

void execute_exit(int interrupted) {
  if (interrupted) {
    write_log_file(countries_names, success_cnt, fail_cnt);
  }
  hash_table_print(file_paths_ht, stdout);
  /* Free all memory allocated by the program */
  __FREE(parameters.dir_paths);
  hash_table_clear(patient_record_ht);
  hash_table_clear(file_paths_ht);
  hash_table_clear(disease_ht);
  hash_table_clear(country_ht);
  list_clear(countries_names);
  list_clear(diseases_names);
  list_clear(files_statistics);
  /* Write success_cnt and fail_cnt */
  char buffer_success_cnt[12],  buffer_fail_cnt[12];
  sprintf(buffer_success_cnt, "%d", success_cnt);
  write_in_chunks(parameters.write_fd, buffer_success_cnt, parameters.buffer_size);
  sprintf(buffer_fail_cnt, "%d", fail_cnt);
  write_in_chunks(parameters.write_fd, buffer_fail_cnt, parameters.buffer_size);
  /* Close file descriptors */
  close(parameters.write_fd);
  close(parameters.read_fd);
  /* Exit Success */
  exit(EXIT_SUCCESS);
}
