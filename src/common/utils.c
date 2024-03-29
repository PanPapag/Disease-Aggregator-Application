#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>

#include "../../includes/common/io_utils.h"
#include "../../includes/common/list.h"
#include "../../includes/common/macros.h"
#include "../../includes/common/utils.h"

uint8_t string_to_int64(char* value, int64_t* value_out) {
    char* valid;
    *value_out = strtoll(value, &valid, 10);
    if (*valid != '\0') return 0;
    return 1;
}

char* concat(int count, ...) {
  va_list ap;
  int i;

  // Find required length to store merged string
  int len = 1; // room for NULL
  va_start(ap, count);
  for (i = 0; i < count; i++)
      len += strlen(va_arg(ap, char*));
  va_end(ap);

  // Allocate memory to concat strings
  char *merged = calloc(sizeof(char), len);
  int null_pos = 0;

  // Actually concatenate strings
  va_start(ap, count);
  for (i = 0; i < count; i++) {
    char *s = va_arg(ap, char*);
    strcpy(merged+null_pos, s);
    null_pos += strlen(s);
  }
  va_end(ap);

  return merged;
}

size_t uint_hash(void* key) {
  unsigned int x = atoi(key);
  x = ((x >> 16) ^ x) * 0x45d9f3b;
  x = ((x >> 16) ^ x) * 0x45d9f3b;
  x = (x >> 16) ^ x;
  return x;
}

int* int_create(int v) {
	int* p = malloc(sizeof(int));
	*p = v;
	return p;
}

int int_compare(void* a, void* b) {
	return (*(int*)a) - (*(int*)b);
}

void int_print(void* v, FILE* out) {
	fprintf(out, "%d\n", (*(int*)v));
}

void int_destroy(void* v) {
  if (v != NULL) {
    int* i = (int*) v;
    __FREE(i);
  }
}

pid_t* pid_create(pid_t v) {
	pid_t* p = malloc(sizeof(pid_t));
	*p = v;
	return p;
}

int pid_compare(void* a, void* b) {
	return (*(pid_t*)a) - (*(pid_t*)b);
}

void pid_print(void* v, FILE* out) {
	fprintf(out, "%d\n", (*(pid_t*)v));
}

void pid_destroy(void* v) {
  if (v != NULL) {
    pid_t* i = (pid_t*) v;
    __FREE(i);
  }
}

size_t string_hash(void* value) {
  // djb2 hash function
  size_t hash = 5381;
  for (char* s = value; *s != '\0'; s++) {
    hash = (hash << 5) + hash + *s;
  }
  return hash;
}

char* string_create(char* v){
  char* result = (char*) malloc((strlen(v) + 1) * sizeof(char));
  strcpy(result, v);
  return result;
}

void string_print(void* v, FILE* out) {
  fprintf(out, "%s\n", (char*) v);
}

int string_compare(void* v, void* w) {
  char* str1 = (char*) v;
  char* str2 = (char*) w;
  return strcmp(str1, str2);
}

void string_destroy(void *v) {
  if (v != NULL) {
    char* s = (char*) v;
    __FREE(s);
  }
}

void ptr_to_string_print(void* v, FILE* out) {
  fprintf(out, "%s\n", *((char**) v));
}

int ptr_to_string_compare(void* v, void* w) {
  char* str1 = (*(char**) v);
  char* str2 = (*(char**) w);
  return strcmp(str1, str2);
}

void ptr_to_string_destroy(void *v) {
  if (v != NULL) {
    char* s = (*(char**)v);
    __FREE(s);
  }
}

int is_alpharithmetic(char* str) {
  for (size_t i = 0; i < strlen(str); ++i) {
    if (!isalnum(str[i]))
      return 0;
  }
  return 1;
}

int is_alphabetical(char* str) {
  for (size_t i = 0; i < strlen(str); ++i) {
    if (!isalpha(str[i]))
      return 0;
  }
  return 1;
}

int is_number(char* str) {
  for (size_t i = 0; i < strlen(str); ++i) {
    if (!isdigit(str[i]))
      return 0;
  }
  return 1;
}

/* DD-MM-YYYY format */
int is_valid_date_string(char* date) {
  if (strlen(date) != 10) {
    return 0;
  } else {
    for (size_t i = 0; i < strlen(date); ++i) {
      if (i == 2 || i == 5) {
        if (date[i] != '-')
          return 0;
      } else {
        if (!isdigit(date[i]))
          return 0;
      }
    }
  }
  return 1;
}
/* Unspecified '-' */
int is_unspecified_date_string(char* date) {
  if ((strlen(date) == 1) && (date[0] == '-')) {
    return 1;
  } else {
    return 0;
  }
}

int is_unspecified_date_tm(struct tm date) {
  char date_buf[11];
  strftime(date_buf, sizeof(date_buf), "%d-%m-%Y", &date);
  return !strcmp(date_buf, "00-01-1900");
}

int64_t date_string_compare(char* date1, const char* date2) {
  // Convert date1 string to struct tm
  struct tm date1_tm;
  memset(&date1_tm, 0, sizeof(struct tm));
  strptime(date1, "%d-%m-%Y", &date1_tm);
  // Convert date2 string to struct tm
  struct tm date2_tm;
  memset(&date2_tm, 0, sizeof(struct tm));
  strptime(date2, "%d-%m-%Y", &date2_tm);
  // Convert date1_tm back to seconds
  char date1_buf[MAX_BUFFER_SIZE];
  strftime(date1_buf, sizeof(date1_buf), "%s", &date1_tm);
  size_t date1_to_secs;
  string_to_int64(date1_buf, (int64_t *) &date1_to_secs);
  // Convert date2_tm back to seconds
  char date2_buf[MAX_BUFFER_SIZE];
  strftime(date2_buf, sizeof(date2_buf), "%s", &date2_tm);
  size_t date2_to_secs;
  string_to_int64(date2_buf, (int64_t *) &date2_to_secs);
  // Return the difference
  return date1_to_secs - date2_to_secs;
}

int64_t date_tm_compare(struct tm date1_tm, struct tm date2_tm) {
  // Convert date1_tm to seconds
  char date1_buf[MAX_BUFFER_SIZE];
  strftime(date1_buf, sizeof(date1_buf), "%s", &date1_tm);
  size_t date1_to_secs;
  string_to_int64(date1_buf, (int64_t *) &date1_to_secs);
  // Convert date2_t to seconds
  char date2_buf[MAX_BUFFER_SIZE];
  strftime(date2_buf, sizeof(date2_buf), "%s", &date2_tm);
  size_t date2_to_secs;
  string_to_int64(date2_buf, (int64_t *) &date2_to_secs);
  // Return the difference
  return date1_to_secs - date2_to_secs;
}

char** prune_command_name(char** src, size_t size) {
  char** dest = (char**) malloc((size - 1) * sizeof(char*));
  for (size_t i = 1; i < size; ++i) {
    *(dest + i - 1) = src[i];
  }
  return dest;
}

char* get_last_token(const char* str, char del) {
  char* last_token = strrchr(str, del);
  return last_token+1;
}

void write_log_file(list_ptr countries_list, int success_cnt, int fail_cnt) {
  char log_file[15];
  char buffer[MAX_BUFFER_SIZE];
  /* Create log file */
  sprintf(log_file, "log_file.%d", getpid());
  FILE* fp = fopen(log_file, "w");
  if (fp == NULL) {
    report_error("Unable to create file %s. Exiting...", log_file);
    exit(EXIT_FAILURE);
  }
  /* Write countries */
  for (size_t i = 1U; i <= list_size(countries_list); ++i) {
    list_node_ptr list_node = list_get(countries_list, i);
    char* country = (*(char**) list_node->data_);
    fputs(country, fp);
    fputs("\n", fp);
  }
  /* Compute total number of requests */
  int total = success_cnt + fail_cnt;
  sprintf(buffer, "TOTAL %d\n", total);
  fputs(buffer, fp);
  /* Write success number of requests */
  sprintf(buffer, "SUCCESS %d\n", success_cnt);
  fputs(buffer, fp);
  /* Write success number of requests */
  sprintf(buffer, "FAIL %d\n", fail_cnt);
  fputs(buffer, fp);
  /* Close file pointer */
  fclose(fp);
}
