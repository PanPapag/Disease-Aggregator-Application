#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../includes/common/macros.h"
#include "../../includes/worker/avl.h"
#include "../../includes/worker/commands.h"
#include "../../includes/worker/hash_table.h"
#include "../../includes/worker/heap.h"
#include "../../includes/worker/io_utils.h"
#include "../../includes/worker/list.h"
#include "../../includes/worker/patient_record.h"
#include "../../includes/worker/utils.h"

hash_table_ptr patient_record_ht;
hash_table_ptr disease_ht;
hash_table_ptr country_ht;

list_ptr diseases_names;
list_ptr countries_names;

static inline
int __count_patients_between(avl_node_ptr current_root, int* counter,
                             struct tm date1, struct tm date2, char* country) {

  avl_node_ptr temp = current_root;
  patient_record_ptr patient_record = NULL;
  /* Prune left search below this node as all entries have entry date < date1 */
  if (temp != NULL) {
    patient_record = (patient_record_ptr) temp->data_;
    if (compare_date_tm(date1, patient_record->entry_date) <= 0) {
      __count_patients_between(temp->left_, counter, date1, date2, country);
    }
    patient_record = (patient_record_ptr) temp->data_;
    /* Check if patient_record exit date is not specified */
    if (compare_date_tm(patient_record->entry_date, date2) <= 0) {
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
    __count_patients_between(temp->right_, counter, date1, date2, country);
  }
}

static inline
int __num_patients_between(avl_ptr disease_avl, char* date1, char* date2, char* country) {
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
   __count_patients_between(disease_avl->root_, &counter, date1_tm, date2_tm, country);
  return counter;
}

void execute_disease_frequency(char** argv) {
  void* result = hash_table_find(disease_ht, argv[0]);
  if (result == NULL) {
    report_warning("There is no disease recorded with Disease ID: <%s>", argv[0]);
  } else {
    /* Cast result to avl pointer */
    avl_ptr disease_avl = (avl_ptr) result;
    /* Print total number of patients for given country in the given date range */
    printf("%d\n", __num_patients_between(disease_avl, argv[1], argv[2], NULL));
  }
}

int execute_insert_patient_record(patient_record_ptr patient_record) {
  void* result = hash_table_find(patient_record_ht, patient_record->record_id);
  /* If record Id not found */
  if (result == NULL) {
    /* Update patient record hash table */
    hash_table_insert(&patient_record_ht, patient_record->record_id, patient_record);
    /* Search if patient record disease id exists */
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
    /* Search if patient record country exists */
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
    patient_record_delete(patient_record);
    return ERROR;
  }
  return PASS;
}

int execute_record_patient_exit(char* id, const char* exit_date) {
  /* Search if patient record id exists */
  void* result = hash_table_find(patient_record_ht, id);
  if (result == NULL) {
    report_warning("Patient record with Record ID: <%s> not found.", id);
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
    if (compare_date_strings(entry_date_buffer, exit_date) > 0) {
      report_warning("Entry Date [%s] is after the given Exit Date [%s].",
                      entry_date_buffer, exit_date);
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
      return ERROR;
    }
  }
  return PASS;
}
