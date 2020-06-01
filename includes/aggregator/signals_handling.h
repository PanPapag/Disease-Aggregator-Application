#ifndef __AGGREGATOR_SIGNALS_HANDLING__
  #define __AGGREGATOR_SIGNALS_HANDLING__

  #include <setjmp.h>
  #include <signal.h>
  
  extern volatile sig_atomic_t interrupt;

  void register_signals_handlers(void);

#endif
