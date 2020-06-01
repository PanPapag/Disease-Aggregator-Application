#ifndef __COMMON_STATISTICS__
  #define __COMMON_STATISTICS__

  #include "./hash_table.h"

  #include <stdint.h>

  #define FIELDS_DEL '$'
  #define DISEASE_DEL ':'
  #define AGE_GROUPS_DEL '@'
  #define NO_AGE_GROUPS 4

  enum age_groups {
    AGE_GROUP_1,
    AGE_GROUP_2,
    AGE_GROUP_3,
    AGE_GROUP_4
  };

  typedef struct statistics_entry_t {
    char* file_name;
    char* country;
    hash_table_ptr age_groups_ht;
  } statistics_entry_t;
  typedef statistics_entry_t* statistics_entry_ptr;

  typedef struct age_groups_stats_t {
    int age_group;
    int no_patients;
  } age_groups_stats_t;
  typedef age_groups_stats_t* age_groups_stats_ptr;

  /*
    Utility function to create a new statistic entry pointer given the file name,
    the country name and a pointer to age_groups_ht
  */
  statistics_entry_ptr statistics_entry_create(const char*, const char*, hash_table_ptr);
  /* Utility function to destroy the content of a pointer to statistics entry */
  void ptr_to_statistics_entry_destroy(void*);
  /* Utility function to print a pointer to statistics entry */
  void ptr_to_statistics_entry_print(void*, FILE*);
  /* Returns a string representing the content of the given ptr to statistics entry */
  char* ptr_to_statistics_entry_serialize(void*);
  /* Utility function to print a serialized statistics_entry */
  void serialized_statistics_entry_print(char*);
  /* Given age this function returns the enumared age group */
  int get_age_group(uint8_t);
  /* Given the age group index (enum) this function returns the age_group name */
  char* get_age_group_name(int );
  /* Prints the numberf of cases in the array of age groups */
  void age_groups_print(void*, FILE*);
  /* Compare two given age_groups_stats entry by the no_patients field */
  int age_groups_stats_compare(void*, void*);
  /* Destroy the allocated array of age groups, aka a ptr to int */
  void age_groups_destroy(void*);

#endif
