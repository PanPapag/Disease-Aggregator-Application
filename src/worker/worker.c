#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../includes/common/macros.h"
#include "../../includes/worker/avl.h"
#include "../../includes/worker/hash_table.h"
#include "../../includes/worker/patient_record.h"
#include "../../includes/worker/io_utils.h"
#include "../../includes/worker/utils.h"

extern hash_table_ptr patient_record_ht;
extern hash_table_ptr disease_ht;
extern hash_table_ptr country_ht;

extern list_ptr diseases_names;
extern list_ptr countries_names;

int main(int argc, char* argv[]) {
  const size_t no_buckets = 20;
  const size_t bucket_size = 64;
  /* patient_record_ht: record id --> pointer to patient record structure */
  patient_record_ht = hash_table_create(no_buckets, bucket_size,
                                       hash_string, compare_string,
                                       print_string, patient_record_print,
                                       NULL, patient_record_delete);
  /* Create Disease Hash Table */
  disease_ht = hash_table_create(no_buckets, bucket_size,
                                 hash_string, compare_string,
                                 print_string, avl_print_inorder,
                                 NULL, avl_clear);
  /* Create Country Hash Table */
  country_ht = hash_table_create(no_buckets, bucket_size,
                                 hash_string, compare_string,
                                 print_string, avl_print_inorder,
                                 NULL, avl_clear);

  /* Initialize a list to store all disease names */
	diseases_names = list_create(STRING*, print_string, compare_string, NULL);
  /* Initialize a list to store all disease names */
	countries_names = list_create(STRING*, print_string, compare_string, NULL);

  // testing
  char* dir_name = "../input_dir/Argentina";
  parse_directory(dir_name);

  // hash_table_print(patient_record_ht, stdout);
  list_print(diseases_names, stdout);
  list_print(countries_names, stdout);
  return EXIT_SUCCESS;
}
