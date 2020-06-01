#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../includes/aggregator/signals_handling.h"

jmp_buf jmp_exit;

static void handle_interrupt(int signo) {
  longjmp(jmp_exit, 1);
}

void register_signals_handlers(void) {
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
  printf("HERE\n");
}
