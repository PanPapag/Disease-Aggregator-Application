#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../includes/worker/io_utils.h"

void parse_directory_and_update_structures(const char* dir_pathname) {
  return;
}

static void __report(const char* tag, const char* fmt, va_list args) {
  fprintf(stderr, "%s", tag);
  vfprintf(stderr, fmt, args);
  fputc('\n', stderr);
  va_end(args);
}

void report_error(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  __report("[ERROR]: ", fmt, args);
}

void report_warning(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  __report("[WARNING]: ", fmt, args);
}
