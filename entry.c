#include "dbg.h"
#include "entry.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

Entry *Entry_create(char *read_name, char *chr_name, int pos, int chr_num, char *cigar){
  Entry *entry = malloc(sizeof(Entry));
  assert(entry != NULL);

  entry->read_name = strdup(read_name);
  entry->chr_name = strdup(chr_name);
  entry->pos = pos;
  entry->chr_num = chr_num;
  entry->cigar_string = strdup(cigar);

  return entry;
}

void Entry_destroy(Entry *entry) {
  assert(entry != NULL);

  free(entry->read_name);
  free(entry->chr_name);
  free(entry->cigar_string);
  free(entry);
}