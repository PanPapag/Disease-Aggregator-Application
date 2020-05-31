#ifndef __AGGREGATOR_COMMANDS__
  #define __AGGREGATOR_COMMANDS__

  enum command_codes {
    INVALID_COMMAND,
    VALID_COMMAND
  };

  int validate_list_countries(int, char**);
  void execute_list_countries(void);

  int validate_disease_frequency(int, char**);
  void aggregate_disease_frequency(int, char**);

  int validate_topk_age_ranges(int, char**);
  void aggregate_topk_age_ranges(void);

  int validate_search_patient_record(int, char**);
  void aggregate_search_patient_record(void);

  int validate_num_patient_admissions(int, char**);
  void aggregate_num_patient_admissions(void);

  int validate_num_patient_discharges(int, char**);
  void aggregate_num_patient_discharges(void);

  int validate_exit(int, char**);
  void execute_exit(void);

#endif
