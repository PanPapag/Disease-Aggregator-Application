#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../../includes/common/list.h"
#include "../../includes/aggregator/io_utils.h"

program_parameters_t parameters;

int main(int argc, char* argv[]) {
  /* Parse command line arguments and update program parameters */
  parse_arguments(&argc, argv);
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
      sprintf(fifo_1,"pw_cr_%d",getpid());
      ret_val = mkfifo(fifo_1,0666);
      if((ret_val == -1) && (errno != EEXIST)) {
         perror("Creating Fifo Failed");
         exit(1);
      }
      /* Create named pipe in which parent reads and child writes */
      sprintf(fifo_2,"pr_cw_%d",getpid());
      ret_val = mkfifo(fifo_2,0666);
      if((ret_val == -1) && (errno != EEXIST)) {
         perror("Creating Fifo Failed");
         exit(1);
      }
      break;
    } else {
      workers_pid[proccess_cnt] = pid;
    }
  }

  if (pid == 0) {
    execl("./worker", "worker", fifo_1, fifo_2, (char *) NULL);
    perror("execl() Execution Failed");
    exit(EXIT_FAILURE);
  } else {
    printf("PARENT\n");
    /* Get a list of all subdirectories included in the input directory */
    list_ptr subdirs = get_all_subdirs(parameters.input_dir);
    for (size_t i = 0; i < parameters.num_workers; ++i)
      printf("%ld\n",workers_pid[i]);
    /* Clear memory */
    list_clear(subdirs);
  }

  return EXIT_SUCCESS;
}
