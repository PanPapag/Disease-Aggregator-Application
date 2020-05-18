#ifndef __WORKER_COMMANDS__
  #define __WORKER_COMMANDS__

  #include "./patient_record.h"

  enum command_codes {
      INVALID_COMMAND,
      VALID_COMMAND
    };

    void execute_disease_frequency(char**);

    void execute_search_patient_record(char**);

    int execute_insert_patient_record(patient_record_ptr);

    int execute_record_patient_exit(char*, const char*);

#endif
