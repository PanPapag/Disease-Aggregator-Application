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
  /* Communication Protocol */
  // 1. Write the number of bytes that the reader should read
  // 2. Write message
  char buffer[buffer_size];
  size_t msg_length = strlen(msg);
  ssize_t total_bytes = msg_length + (msg_length / (buffer_size - 1)) + 1;
  write(fd, &total_bytes, sizeof(ssize_t));
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

char* read_in_chunks(int fd, size_t buffer_size) {
  ssize_t total_bytes;
  ssize_t bytes_read = 0;
  char buffer[buffer_size];
  if (read(fd, &total_bytes, sizeof(ssize_t)) < 0) {
    perror("Read");
    exit(EXIT_FAILURE);
  }
  size_t message_size = total_bytes - (total_bytes / (buffer_size - 1));
  char* message = (char*) malloc(message_size * sizeof(char));
  while (bytes_read < total_bytes) {
    bytes_read += read(fd, buffer, buffer_size);
    strcat(message, buffer);
  }
  return message;
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
