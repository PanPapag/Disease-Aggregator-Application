#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../../includes/common/list.h"
#include "../../includes/common/macros.h"
#include "../../includes/common/io_utils.h"
#include "../../includes/common/utils.h"
#include "../../includes/worker/avl.h"
#include "../../includes/worker/commands.h"
#include "../../includes/worker/hash_table.h"
#include "../../includes/worker/patient_record.h"
#include "../../includes/worker/io_utils.h"

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
	diseases_names = list_create(STRING*, compare_string_ptr, print_string_ptr, NULL);
  /* Initialize a list to store all disease names */
	countries_names = list_create(STRING*, compare_string_ptr, print_string_ptr, NULL);

  /* Open named pipe to read the directories paths */
  int fd_read = open(argv[1], O_RDONLY);
  if (fd_read < 0) {
    report_error("<%s> could not open named pipe: %s", argv[0], argv[1]);
    exit(EXIT_FAILURE);
  }
  /* Read from the pipe the directories paths */
  char* dir_paths = read_in_chunks(fd_read, 10);
  printf("%s\n",dir_paths);

  close(fd_read);

  unlink(argv[1]);
  unlink(argv[2]);

  // testing
  // char* dir_name = "../input_dir/Argentina";
  // parse_directory(dir_name);

  // DiseaseFrequency - NumPatients commands
  // char *ar[3];
  // ar[0] = "SARS-1";
  // ar[1] = "01-01-2020";
  // ar[2] = "04-01-2020";
  // execute_disease_frequency(ar);

  // execute_num_patients_admissions(ar);
  //
  // execute_num_patients_discharges(ar);

  // char* ar[1];
  // ar[0] = "2107";
  // execute_search_patient_record(ar);

  // hash_table_print(patient_record_ht, stdout);
  // hash_table_print(disease_ht, stdout);
  // list_print(diseases_names, stdout);
  // list_print(countries_names, stdout);

  /* Clear memory */
  execute_exit();
  free(dir_paths);

  return EXIT_SUCCESS;
}
