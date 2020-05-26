#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../includes/common/macros.h"
#include "../../includes/common/io_utils.h"
#include "../../includes/common/utils.h"

void write_in_chunks(int fd, char* msg, size_t buffer_size) {
  char buffer[buffer_size];
  size_t msg_length = strlen(msg);
  ssize_t total_bytes = msg_length + (msg_length / (buffer_size - 1)) + 1;
  ssize_t bytes_written = 0;
  int counter = 0;
  while (bytes_written < total_bytes) {
    for (size_t i = 0; i < buffer_size - 1; ++i) {
      if (counter <= msg_length) {
        buffer[i] = msg[counter++];
      }
    }
    buffer[buffer_size - 1] = '\0';
    bytes_written += write(fd, buffer, strlen(buffer) + 1);
  }
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
