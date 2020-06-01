#ifndef __WORKER_COMMANDS__
  #define __WORKER_COMMANDS__

  #include "../common/hash_table.h"
  #include "./patient_record.h"

  #define __USE_XOPEN
  #include <time.h>

  enum command_codes {
      INVALID_COMMAND,
      VALID_COMMAND
  };

  void execute_disease_frequency(int, char**);

  void execute_num_patients_admissions(int, char**);

  void execute_num_patients_discharges(int, char**);

  void execute_topk_age_ranges(char**);

  void execute_search_patient_record(char**);

  void execute_exit(int);

  int execute_insert_patient_record(patient_record_ptr, hash_table_ptr);

  int execute_record_patient_exit(char*, const char*);

#endif
