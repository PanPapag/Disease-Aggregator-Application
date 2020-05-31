#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../includes/common/macros.h"
#include "../../includes/common/io_utils.h"
#include "../../includes/common/utils.h"
#include "../../includes/aggregator/commands.h"

int validate_list_countries(int argc, char** argv) {
  return argc == 1 ? VALID_COMMAND : INVALID_COMMAND;
}

int validate_disease_frequency(int argc, char** argv) {
  if (argc != 4 && argc != 5) {
    return INVALID_COMMAND;
  } else {
    if (argc == 4) {
      // Check virus name to contain only letters, numbers and possibly '-'
      char* virus_name = argv[1];
      for (size_t i = 0; i < strlen(virus_name); ++i) {
        if (!isalnum(virus_name[i]) && virus_name[i] != '-')
          return INVALID_COMMAND;
      }
      // Check date1
      char* date1 = argv[2];
      if (!is_valid_date_string(date1))
        return INVALID_COMMAND;
      // Check date2
      char* date2 = argv[3];
      if (!is_valid_date_string(date2))
        return INVALID_COMMAND;
    } else {
      // Check virus name to contain only letters, numbers and possibly '-'
      char* virus_name = argv[1];
      for (size_t i = 0; i < strlen(virus_name); ++i) {
        if (!isalnum(virus_name[i]) && virus_name[i] != '-')
          return INVALID_COMMAND;
      }
      // Check date1
      char* date1 = argv[2];
      if (!is_valid_date_string(date1))
        return INVALID_COMMAND;
      // Check date2
      char* date2 = argv[3];
      if (!is_valid_date_string(date2))
        return INVALID_COMMAND;
      // Check country to contain only letters
      char* country = argv[4];
      if (!is_alphabetical(country))
        return INVALID_COMMAND;
    }
  }
  return VALID_COMMAND;
}

int validate_topk_age_ranges(int argc, char** argv) {
  if (argc != 6) {
    return INVALID_COMMAND;
  } else {
    // Check k
    char* k = argv[1];
    if (!is_number(k))
      return INVALID_COMMAND;
    // Check country to contain only letters
    char* country = argv[2];
    if (!is_alphabetical(country))
      return INVALID_COMMAND;
    // Check virus name to contain only letters, numbers and possibly '-'
    char* virus_name = argv[3];
    for (size_t i = 0; i < strlen(virus_name); ++i) {
      if (!isalnum(virus_name[i]) && virus_name[i] != '-')
        return INVALID_COMMAND;
    }
    // Check date1
    char* date1 = argv[4];
    if (!is_valid_date_string(date1))
      return INVALID_COMMAND;
    // Check date2
    char* date2 = argv[5];
    if (!is_valid_date_string(date2))
      return INVALID_COMMAND;
  }
}

int validate_search_patient_record(int argc, char** argv) {
  return argc == 2 ? VALID_COMMAND : INVALID_COMMAND;
}

int validate_num_patient_admissions(int argc, char** argv) {
  if (argc != 4 && argc != 5) {
    return INVALID_COMMAND;
  } else {
    if (argc == 4) {
      // Check virus name to contain only letters, numbers and possibly '-'
      char* virus_name = argv[1];
      for (size_t i = 0; i < strlen(virus_name); ++i) {
        if (!isalnum(virus_name[i]) && virus_name[i] != '-')
          return INVALID_COMMAND;
      }
      // Check date1
      char* date1 = argv[2];
      if (!is_valid_date_string(date1))
        return INVALID_COMMAND;
      // Check date2
      char* date2 = argv[3];
      if (!is_valid_date_string(date2))
        return INVALID_COMMAND;
    } else {
      // Check virus name to contain only letters, numbers and possibly '-'
      char* virus_name = argv[1];
      for (size_t i = 0; i < strlen(virus_name); ++i) {
        if (!isalnum(virus_name[i]) && virus_name[i] != '-')
          return INVALID_COMMAND;
      }
      // Check date1
      char* date1 = argv[2];
      if (!is_valid_date_string(date1))
        return INVALID_COMMAND;
      // Check date2
      char* date2 = argv[3];
      if (!is_valid_date_string(date2))
        return INVALID_COMMAND;
      // Check country to contain only letters
      char* country = argv[4];
      if (!is_alphabetical(country))
        return INVALID_COMMAND;
    }
  }
  return VALID_COMMAND;
}

int validate_num_patient_discharges(int argc, char** argv) {
  if (argc != 4 && argc != 5) {
    return INVALID_COMMAND;
  } else {
    if (argc == 4) {
      // Check virus name to contain only letters, numbers and possibly '-'
      char* virus_name = argv[1];
      for (size_t i = 0; i < strlen(virus_name); ++i) {
        if (!isalnum(virus_name[i]) && virus_name[i] != '-')
          return INVALID_COMMAND;
      }
      // Check date1
      char* date1 = argv[2];
      if (!is_valid_date_string(date1))
        return INVALID_COMMAND;
      // Check date2
      char* date2 = argv[3];
      if (!is_valid_date_string(date2))
        return INVALID_COMMAND;
    } else {
      // Check virus name to contain only letters, numbers and possibly '-'
      char* virus_name = argv[1];
      for (size_t i = 0; i < strlen(virus_name); ++i) {
        if (!isalnum(virus_name[i]) && virus_name[i] != '-')
          return INVALID_COMMAND;
      }
      // Check date1
      char* date1 = argv[2];
      if (!is_valid_date_string(date1))
        return INVALID_COMMAND;
      // Check date2
      char* date2 = argv[3];
      if (!is_valid_date_string(date2))
        return INVALID_COMMAND;
      // Check country to contain only letters
      char* country = argv[4];
      if (!is_alphabetical(country))
        return INVALID_COMMAND;
    }
  }
  return VALID_COMMAND;
}

int validate_exit(int argc, char** argv) {
  return argc == 1 ? VALID_COMMAND : INVALID_COMMAND;
}
