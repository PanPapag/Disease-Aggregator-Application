#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../../includes/common/list.h"
#include "../../includes/common/macros.h"
#include "../../includes/common/io_utils.h"
#include "../../includes/common/statistics.h"
#include "../../includes/common/utils.h"
#include "../../includes/aggregator/io_utils.h"
#include "../../includes/aggregator/utils.h"

extern hash_table_ptr country_to_pid_ht;
extern list_ptr countries_names;

program_parameters_t parameters;

int main(int argc, char* argv[]) {
  /* Parse command line arguments and update program parameters */
  parse_arguments(&argc, argv);
  char buffer_size[12];
  sprintf(buffer_size, "%lu", parameters.buffer_size);
  /* Spawn num_workers child proccesses */
  char fifo_1[15], fifo_2[15];
  pid_t workers_pid[parameters.num_workers];
  pid_t pid;
  size_t proccess_cnt;
  int ret_val, status;
  for (proccess_cnt = 0; proccess_cnt < parameters.num_workers; ++proccess_cnt) {
    pid = fork();
    if (pid == -1) {
      perror("Failed to fork!");
      exit(EXIT_FAILURE);
    }
    if (pid == 0) {
      /* Create named pipe in which parent writes and child reads */
      sprintf(fifo_1, "pw_cr_%d", getpid());
      ret_val = mkfifo(fifo_1, 0666);
      if ((ret_val == -1) && (errno != EEXIST)) {
         perror("Creating Fifo Failed");
         exit(1);
      }
      /* Create named pipe in which parent reads and child writes */
      sprintf(fifo_2, "pr_cw_%d", getpid());
      ret_val = mkfifo(fifo_2, 0666);
      if ((ret_val == -1) && (errno != EEXIST)) {
         perror("Creating Fifo Failed");
         exit(1);
      }
      break;
    } else {
      workers_pid[proccess_cnt] = pid;
    }
  }

  if (pid == 0) {
    execl("./worker", "worker", fifo_1, fifo_2, buffer_size, (char *) NULL);
    perror("execl() Execution Failed");
    exit(EXIT_FAILURE);
  } else {
    /*
      Initialize country_to_pid_ht. key: <country_name> -> value: <pid>
      which parsed the given country directory
    */
    country_to_pid_ht = hash_table_create(NO_BUCKETS, BUCKET_SIZE,
                                          string_hash, string_compare,
                                          string_print, pid_print,
                                          NULL, pid_destroy);
    /* Initialize a list to store the names of all countries directories */
    countries_names = list_create(STRING*, ptr_to_string_compare,
                                  ptr_to_string_print, ptr_to_string_destroy);
    /*
      Each child proccess communicates with the parent via two file descriptors,
      one for writing and one for reading.
    */
    char workers_fifo_1[parameters.num_workers][FIFO_NAME_SIZE];
    char workers_fifo_2[parameters.num_workers][FIFO_NAME_SIZE];
    int workers_fd_1[parameters.num_workers];
    int workers_fd_2[parameters.num_workers];
    /* Store workers fds for writing and reading to program_parameters */
    parameters.workers_pid = (pid_t*) malloc(parameters.num_workers * sizeof(pid_t));
    parameters.workers_fd_1 = (int*) malloc(parameters.num_workers * sizeof(int));
    parameters.workers_fd_2 = (int*) malloc(parameters.num_workers * sizeof(int));
    /* Get a list of all subdirectories included in the input directory */
    list_ptr subdirs = get_all_subdirs(parameters.input_dir);
    /* Distribute sub directories to each worker */
    char** worker_dir_paths = distribute_subdirs(subdirs, parameters.num_workers);
    /* Distribute directories paths and reconstruct named pipes to communicate */
    for (size_t i = 0; i < parameters.num_workers; ++i) {
      // TODO If no directories given in this worker, kill him
      /* Store worker pid to program_parameters */
      parameters.workers_pid[i] = workers_pid[i];
      /* Update for the current worker country_to_pid_ht */
      update_country_to_pid_ht(worker_dir_paths[i], workers_pid[i]);
      /* Reconstruct named pipes given the children proccesses ids */
      sprintf(workers_fifo_1[i], "pw_cr_%d", workers_pid[i]);
      sprintf(workers_fifo_2[i], "pr_cw_%d", workers_pid[i]);
      /*
        Open named pipe for writing the directories paths to be sent as well as
        distributing the application commands given from stdin
      */
      workers_fd_1[i] = open(workers_fifo_1[i], O_WRONLY);
      parameters.workers_fd_1[i] = workers_fd_1[i];
      if (workers_fd_1[i] < 0) {
        report_error("<diseaseAggregator> could not open named pipe: %s", workers_fifo_1[i]);
        exit(EXIT_FAILURE);
      }
      /* Write to the pipe the directories paths */
      write_in_chunks(workers_fd_1[i], worker_dir_paths[i], parameters.buffer_size);
      /*
        Open named pipe for reading files statistics as well as the results
        from the commands sent to workers
      */
      workers_fd_2[i] = open(workers_fifo_2[i], O_RDONLY);
      parameters.workers_fd_2[i] = workers_fd_2[i];
      if (workers_fd_2[i] < 0) {
        report_error("<diseaseAggregator> could not open named pipe: %s", workers_fifo_2[i]);
        exit(EXIT_FAILURE);
      }
      /* Read from the pipe the files statistics */
      char* num_files_buffer = read_in_chunks(workers_fd_2[i], parameters.buffer_size);
      int num_files = atoi(num_files_buffer);
      for (size_t j = 0; j < num_files; ++j) {
        char* serialized_statistics_entry = read_in_chunks(workers_fd_2[i], parameters.buffer_size);
        serialized_statistics_entry_print(serialized_statistics_entry);
        __FREE(serialized_statistics_entry);
      }
      __FREE(num_files_buffer);
      __FREE(worker_dir_paths[i]);
    }
    __FREE(worker_dir_paths);
    list_clear(subdirs);
    /* Execute the app until command exit is given */
    main_loop();
    /* Everything handled correctly return success from parent */
    return EXIT_SUCCESS;
  }
}
