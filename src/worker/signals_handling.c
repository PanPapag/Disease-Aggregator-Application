#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../includes/common/io_utils.h"
#include "../../includes/common/macros.h"
#include "../../includes/common/statistics.h"
#include "../../includes/common/utils.h"
#include "../../includes/worker/io_utils.h"
#include "../../includes/worker/signals_handling.h"

volatile sig_atomic_t new_files;

jmp_buf interrupt;

list_ptr files_statistics;

worker_parameters_t parameters;

int success_cnt;

static void signal_handlers(int signo) {
  if (signo == SIGINT || signo == SIGQUIT) {
    longjmp(interrupt, 1);
  } else if (signo == SIGUSR1) {
    new_files = 1;
    /* Initialize file statistics */
    files_statistics = list_create(statistics_entry_ptr*, NULL,
                                   ptr_to_statistics_entry_print,
                                   ptr_to_statistics_entry_destroy);
    /* Parse dirs and search for new files */
    char dir_paths[MAX_BUFFER_SIZE];
    strcpy(dir_paths, parameters.dir_paths);
    char* dir_path = strtok(dir_paths, SPACE);
    while (dir_path != NULL) {
      parse_directory(dir_path);
      dir_path = strtok(NULL, SPACE);
    }
    /*
      Send to parent proccess, aka aggregator a SIGUSR2 to trigger him to read
      statistics from the newly parsed files
    */
    pid_t aggregator_pid = getppid();
    kill(aggregator_pid, SIGUSR2);
    /* Start writing file statistics */
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
    /* Clear file statistics as we do not need them anymore */
    list_clear(files_statistics);
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
