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
#include "../../includes/worker/io_utils.h"
#include "../../includes/worker/patient_record.h"

extern hash_table_ptr country_ht;
extern hash_table_ptr disease_ht;
extern hash_table_ptr file_paths_ht;
extern hash_table_ptr patient_record_ht;

extern list_ptr countries_names;
extern list_ptr diseases_names;
extern list_ptr files_statistics;

worker_parameters_t parameters;

extern int success_cnt;
extern int fail_cnt;

int main(int argc, char* argv[]) {
  /* Initialize success_cnt and fail_cnt */
  success_cnt = 0;
  fail_cnt = 0;
  /* Extract command line arguments */
  char* read_fifo = argv[1];
  char* write_fifo = argv[2];
  parameters.buffer_size = atoi(argv[3]);
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
  /* Create Files paths Hash Table */
  file_paths_ht = hash_table_create(NO_BUCKETS, BUCKET_SIZE,
                                    string_hash, string_compare,
                                    string_print, string_print,
                                    string_destroy, NULL);
  /* Initialize a list to store all disease names */
 	countries_names = list_create(STRING*, ptr_to_string_compare, ptr_to_string_print, NULL);
  /* Initialize a list to store all disease names */
	diseases_names = list_create(STRING*, ptr_to_string_compare, ptr_to_string_print, NULL);
  /* Initialize a list to store statistics for each file */
  files_statistics = list_create(statistics_entry_ptr*, NULL,
                                 ptr_to_statistics_entry_print,
                                 ptr_to_statistics_entry_destroy);

  /* Open named pipe to read the directories paths as well as the application commands */
  parameters.read_fd = open(read_fifo, O_RDONLY);
  if (parameters.read_fd < 0) {
    report_error("<%s> could not open named pipe: %s", argv[0], read_fifo);
    exit(EXIT_FAILURE);
  }
  /* Read from the pipe the directories paths and parse them */
  parameters.dir_paths = read_in_chunks(parameters.read_fd, parameters.buffer_size);
  char dir_paths[MAX_BUFFER_SIZE];
  strcpy(dir_paths, parameters.dir_paths);
  char* dir_path = strtok(dir_paths, SPACE);
	while (dir_path != NULL) {
    parse_directory(dir_path);
		dir_path = strtok(NULL, SPACE);
	}
  /* Open named pipe to write the statistics as well as the results from the commands */
  parameters.write_fd = open(write_fifo, O_WRONLY);
  if (parameters.write_fd < 0) {
    report_error("<%s> could not open named pipe: %s", argv[0], write_fifo);
    exit(EXIT_FAILURE);
  }
  /* Write to the pipe the files statistics */
  char num_files_buffer[12];
  sprintf(num_files_buffer, "%ld", list_size(files_statistics));
  write_in_chunks(parameters.write_fd, num_files_buffer, parameters.buffer_size);
  success_cnt++;
  for (size_t i = 1U; i <= list_size(files_statistics); ++i) {
    list_node_ptr list_node = list_get(files_statistics, i);
    char* serialized_statistics_entry = ptr_to_statistics_entry_serialize(list_node->data_);
    write_in_chunks(parameters.write_fd, serialized_statistics_entry, parameters.buffer_size);
    success_cnt++;
    free(serialized_statistics_entry);
  }

  /* Reading commands from parent process and writing the results back to him */
  worker_main_loop();

  return EXIT_SUCCESS;
}
