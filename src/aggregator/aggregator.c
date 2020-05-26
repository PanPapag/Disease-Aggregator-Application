#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../../includes/common/list.h"
#include "../../includes/common/io_utils.h"
#include "../../includes/aggregator/io_utils.h"
#include "../../includes/aggregator/utils.h"

program_parameters_t parameters;

int main(int argc, char* argv[]) {
  /* Parse command line arguments and update program parameters */
  parse_arguments(&argc, argv);
  char buffer_size[12];
  sprintf(buffer_size, "%lu", parameters.buffer_size);
  /* Spawn num_workers child proccesses */
  char fifo_1[12], fifo_2[12];
  pid_t workers_pid[parameters.num_workers];
  pid_t pid;
  size_t proccess_cnt;
  int ret_val;
  for (proccess_cnt = 0; proccess_cnt < parameters.num_workers; ++proccess_cnt) {
    pid = fork();
    if (pid == -1) {
      perror("Failed to fork!");
      exit(EXIT_FAILURE);
    }
    if (pid == 0) {
      /* Create named pipe in which parent writes and child reads */
      sprintf(fifo_1, "pw_cr_%d",getpid());
      ret_val = mkfifo(fifo_1, 0666);
      if ((ret_val == -1) && (errno != EEXIST)) {
         perror("Creating Fifo Failed");
         exit(1);
      }
      /* Create named pipe in which parent reads and child writes */
      sprintf(fifo_2, "pr_cw_%d",getpid());
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
      Each child proccess communicates with the parent via two file descriptors,
      one for writing and one for reading.
    */
    char workers_fifo_1[parameters.num_workers][12];
    char workers_fifo_2[parameters.num_workers][12];
    int workers_fd_1[parameters.num_workers];
    int workers_fd_2[parameters.num_workers];
    /* Get a list of all subdirectories included in the input directory */
    list_ptr subdirs = get_all_subdirs(parameters.input_dir);
    /* Distribute sub directories to each worker */
    char** worker_dir_paths = distribute_subdirs(subdirs, parameters.num_workers);
    /* Distribute directories paths and reconstruct named pipes to communicate */
    for (size_t i = 0; i < parameters.num_workers; ++i) {
      // TODO If no directories given in this worker, kill him
      /* Reconstruct named pipes given the children proccesses ids */
      sprintf(workers_fifo_1[i], "pw_cr_%d", workers_pid[i]);
      sprintf(workers_fifo_2[i], "pr_cw_%d", workers_pid[i]);
      /* Open named pipe for writing the directories paths to be sent */
      workers_fd_1[i] = open(workers_fifo_1[i], O_WRONLY);
      if (workers_fd_1[i] < 0) {
        report_error("<diseaseAggregator> could not open named pipe: %s", workers_fifo_1[i]);
        exit(EXIT_FAILURE);
      }
      /* Write to the pipe the directories paths */
      write_in_chunks(workers_fd_1[i], worker_dir_paths[i], parameters.buffer_size);
      /* Close file descriptors and clear memory */
      free(worker_dir_paths[i]);
      close(workers_fd_1[i]);
    }
    // TODO delete it later 
    wait(NULL);
    free(worker_dir_paths);
    /* Clear memory */
    list_clear(subdirs);
  }

  return EXIT_SUCCESS;
}
