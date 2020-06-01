#ifndef __WORKER_SIGNALS_HANDLING__
  #define __WORKER_SIGNALS_HANDLING__

  #include <setjmp.h>
  #include <signal.h>

  extern jmp_buf interrupt;

  void worker_register_handlers(void);

#endif
