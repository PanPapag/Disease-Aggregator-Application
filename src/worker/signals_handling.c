#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../includes/common/macros.h"
#include "../../includes/worker/io_utils.h"
#include "../../includes/worker/signals_handling.h"

volatile sig_atomic_t new_files;

jmp_buf interrupt;

worker_parameters_t parameters;

static void signal_handlers(int signo) {
  if (signo == SIGINT || signo == SIGQUIT) {
    longjmp(interrupt, 1);
  } else if (signo == SIGUSR1) {
    new_files = 1;
    char dir_paths[MAX_BUFFER_SIZE];
    strcpy(dir_paths, parameters.dir_paths);
    char* dir_path = strtok(dir_paths, SPACE);
    while (dir_path != NULL) {
      parse_directory(dir_path);
      dir_path = strtok(NULL, SPACE);
    }
  }
}

void worker_register_handlers(void) {
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
  /* Do not allow SIGUSR1 to interrupt I/O */
  act.sa_flags = SA_RESTART;
  if (sigaction(SIGUSR1, &act, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }
}
