#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../../includes/common/macros.h"
#include "../../includes/aggregator/signals_handling.h"
#include "../../includes/aggregator/utils.h"

volatile sig_atomic_t interrupt;
volatile sig_atomic_t worker_replaced;

program_parameters_t parameters;

static void signal_handlers(int signo) {
  if (signo == SIGINT || signo == SIGQUIT) {
    interrupt = 1;
  } else if (signo == SIGCHLD && interrupt != 1 && parameters.terminate_flag != 1) {
    worker_replaced = 1;
    while (1) {
      int status;
      pid_t pid = waitpid(-1, &status, WNOHANG);
      if (pid <= 0) {
        break;
      }
      /* Handle dead worker */
      char buffer_size[12];
      sprintf(buffer_size, "%lu", parameters.buffer_size);
      char fifo_1[FIFO_NAME_SIZE], fifo_2[FIFO_NAME_SIZE];
      int ret_val;
      pid_t new_pid;
      /* Get position of the killed pid */
      int pos = get_worker_fds_pos(pid);
      /* Unlink old named pipes */
      sprintf(fifo_1, "pw_cr_%d", pid);
      unlink(fifo_1);
      sprintf(fifo_2, "pr_cw_%d", pid);
      unlink(fifo_2);
      /* Fork a new worker and update him */
      new_pid = fork();
      if (new_pid == -1) {
        perror("Failed to fork!");
        exit(EXIT_FAILURE);
      }
      if (new_pid == 0) {
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
        raise(SIGSTOP);
        /* Call the new worker */
        execl("./worker", "worker", fifo_1, fifo_2, buffer_size, (char *) NULL);
        perror("execl() Execution Failed");
        exit(EXIT_FAILURE);
      } else {
        /* Aggregator updates the new forked worker */
        waitpid(new_pid, NULL, WUNTRACED);
        kill(new_pid, SIGCONT);
        update_worker(new_pid, pos);
      }
    }
  }
}


void register_signals_handlers(void) {
  struct sigaction act;
  memset(&act, 0, sizeof(act));
  act.sa_handler = signal_handlers;
  sigfillset(&(act.sa_mask));
  if (sigaction(SIGINT, &act, NULL) == -1){
    perror("sigaction");
    exit(1);
  }
  if (sigaction(SIGQUIT, &act, NULL) == -1){
    perror("sigaction");
    exit(1);
  }
  /* Do not allow SIGCHLD to interrupt I/O */
  act.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &act, NULL) == -1){
    perror("sigaction");
    exit(1);
  }
}
