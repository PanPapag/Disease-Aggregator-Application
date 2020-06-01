#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../includes/worker/signals_handling.h"

jmp_buf interrupt;

static void handle_interrupt(int signo) {
  longjmp(interrupt, 1);
}

void worker_register_handlers(void) {
  struct sigaction act;
  act.sa_handler = handle_interrupt;
  sigfillset(&(act.sa_mask));
  if (sigaction(SIGINT, &act, NULL) == -1){
    perror("sigaction");
    exit(1);
  }
  if (sigaction(SIGQUIT, &act, NULL) == -1){
    perror("sigaction");
    exit(1);
  }
}
