#ifndef __WORKER_COMMANDS__
  #define __WORKER_COMMANDS__

  enum command_codes {
      INVALID_COMMAND,
      VALID_COMMAND
    };

    int execute_insert_patient_record(char**);

    void execute_record_patient_exit(char**);

#endif
