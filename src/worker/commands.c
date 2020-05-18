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
      // list_push_front(&diseases_names, &patient_record->disease_id);
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
      // list_push_front(&countries_names, &patient_record->country);
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

int execute_record_patient_exit(char* id, char* exit_date) {
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
