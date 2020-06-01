#ifndef __AGGREGATOR_SIGNALS_HANDLING__
  #define __AGGREGATOR_SIGNALS_HANDLING__

  #include <setjmp.h>
  #include <signal.h>

  extern jmp_buf jmp_exit;

  void register_signals_handlers(void);

#endif
