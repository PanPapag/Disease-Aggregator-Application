#ifndef __AGGREGATOR_COMMANDS__
  #define __AGGREGATOR_COMMANDS__

  enum command_codes {
    INVALID_COMMAND,
    VALID_COMMAND
  };

  int validate_list_countries(int, char**);
  void execute_list_countries(void);

  int validate_disease_frequency(int, char**);
  void execute_disease_frequency(void);

  int validate_topk_age_ranges(int, char**);
  void execute_topk_age_ranges(void);

  int validate_search_patient_record(int, char**);
  void execute_search_patient_record(void);

  int validate_num_patient_admissions(int, char**);
  void execute_num_patient_admissions(void);

  int validate_num_patient_discharges(int, char**);
  void execute_num_patient_discharges(void);

  int validate_exit(int, char**);
  void execute_exit(void);

#endif
