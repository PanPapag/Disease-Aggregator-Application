#ifndef __WORKER_COMMANDS__
  #define __WORKER_COMMANDS__

  #include "./patient_record.h"

  enum command_codes {
      INVALID_COMMAND,
      VALID_COMMAND
    };

    int execute_insert_patient_record(patient_record_ptr);

    void execute_record_patient_exit(char**);

#endif
