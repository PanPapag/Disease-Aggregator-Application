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

#define NO_BUCKETS 20
#define BUCKET_SIZE 64

int main(int argc, char* argv[]) {
  /* patient_record_ht: record id --> pointer to patient record structure */
  hash_table_ptr patient_record_ht = hash_table_create(
                                      NO_BUCKETS, BUCKET_SIZE,
                                      hash_string, compare_string,
                                      print_string, patient_record_print,
                                      NULL, patient_record_delete
                                    );
  /* Create Disease Hash Table */
  hash_table_ptr disease_ht = hash_table_create(
                                NO_BUCKETS, BUCKET_SIZE,
                                hash_string, compare_string,
                                print_string, avl_print_inorder,
                                NULL, avl_clear
                              );
  /* Create Country Hash Table */
  hash_table_ptr country_ht = hash_table_create(
                                NO_BUCKETS, BUCKET_SIZE,
                                hash_string, compare_string,
                                print_string, avl_print_inorder,
                                NULL, avl_clear
                              );
  // testing
  char* dir_name = "../input_dir/Argentina";
  parse_directory(dir_name);
  return EXIT_SUCCESS;
}
