#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../includes/common/macros.h"
#include "../../includes/worker/avl.h"
#include "../../includes/worker/commands.h"
#include "../../includes/worker/hash_table.h"
#include "../../includes/worker/heap.h"
#include "../../includes/worker/io_utils.h"
#include "../../includes/worker/list.h"
#include "../../includes/worker/patient_record.h"
#include "../../includes/worker/utils.h"

hash_table_ptr patient_record_ht;
hash_table_ptr disease_ht;
hash_table_ptr country_ht;

int execute_insert_patient_record(char** argv) {
  char* status = argv[1];
  if (!strcmp(status, "ENTER")) {
    printf("good\n");
  }
  return PASS;
}
