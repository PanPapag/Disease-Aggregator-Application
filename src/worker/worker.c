#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../../includes/common/hash_table.h"
#include "../../includes/common/list.h"
#include "../../includes/common/macros.h"
#include "../../includes/common/io_utils.h"
#include "../../includes/common/statistics.h"
#include "../../includes/common/utils.h"
#include "../../includes/worker/avl.h"
#include "../../includes/worker/commands.h"
#include "../../includes/worker/patient_record.h"
#include "../../includes/worker/io_utils.h"

extern hash_table_ptr country_ht;
extern hash_table_ptr disease_ht;
extern hash_table_ptr patient_record_ht;

extern list_ptr countries_names;
extern list_ptr diseases_names;
extern list_ptr files_statistics;

int main(int argc, char* argv[]) {
  // /* Extract command line arguments */
  // char* read_fifo = argv[1];
  // char* write_fifo = argv[2];
  // size_t buffer_size = atoi(argv[3]);
  /* patient_record_ht: record id --> pointer to patient record structure */
  patient_record_ht = hash_table_create(NO_BUCKETS, BUCKET_SIZE,
                                        string_hash, string_compare,
                                        string_print, patient_record_print,
                                        NULL, patient_record_destroy);
  /* Create Disease Hash Table */
  disease_ht = hash_table_create(NO_BUCKETS, BUCKET_SIZE,
                                 string_hash, string_compare,
                                 string_print, avl_print_inorder,
                                 NULL, avl_clear);
  /* Create Country Hash Table */
  country_ht = hash_table_create(NO_BUCKETS, BUCKET_SIZE,
                                 string_hash, string_compare,
                                 string_print, avl_print_inorder,
                                 NULL, avl_clear);

  /* Initialize a list to store all disease names */
 	countries_names = list_create(STRING*, ptr_to_string_compare, ptr_to_string_print, NULL);
  /* Initialize a list to store all disease names */
	diseases_names = list_create(STRING*, ptr_to_string_compare, ptr_to_string_print, NULL);
  /* Initialize a list to store statistics for each file */
  files_statistics = list_create(statistics_entry_ptr*, NULL,
                                 ptr_to_statistics_entry_print,
                                 ptr_to_statistics_entry_destroy);
  //
  // /* Open named pipe to read the directories paths */
  // int read_fd = open(read_fifo, O_RDONLY);
  // if (read_fd < 0) {
  //   report_error("<%s> could not open named pipe: %s", argv[0], read_fifo);
  //   exit(EXIT_FAILURE);
  // }
  // /* Read from the pipe the directories paths and parse them */
  // char* dir_paths = read_in_chunks(read_fd, buffer_size);
  // char* dir_path = strtok(dir_paths, SPACE);
	// while (dir_path != NULL) {
  //   parse_directory(dir_path);
	// 	dir_path = strtok(NULL, SPACE);
	// }
  //
  // list_print(files_statistics, stdout);
  // /* Close file descriptors */
  // close(read_fd);
  // /* Unlink named pipes */
  // unlink(read_fifo);
  // unlink(write_fifo);

  char* dir_path = "../input_dir/Argentina";
  parse_directory(dir_path);

  for (size_t i = 1U; i <= list_size(files_statistics); ++i) {
    list_node_ptr list_node = list_get(files_statistics, i);
    char* res = ptr_to_statistics_entry_serialize(list_node->data_);
    printf("%s\n", res);
    free(res);
  }

  list_clear(files_statistics);

  /* Clear memory */
  // free(dir_paths);
  execute_exit();

  // testing
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



  return EXIT_SUCCESS;
}
