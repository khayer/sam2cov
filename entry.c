#include "dbg.h"
#include "entry.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

Entry *Entry_create(char *read_name, int strand, int first, char *chr_name, int pos, int chr_num, char *cigar, int hi_tag){
  Entry *entry = malloc(sizeof(Entry));
  assert(entry != NULL);

  entry->read_name = strdup(read_name);
  entry->strand = strand;
  entry->first = first;
  entry->chr_name = strdup(chr_name);
  entry->pos = pos-1;
  entry->chr_num = chr_num;
  entry->cigar_string = strdup(cigar);
  entry->hi_tag = hi_tag;

  return entry;
}

void Entry_destroy(Entry *entry) {
  assert(entry != NULL);

  free(entry->read_name);
  free(entry->chr_name);
  free(entry->cigar_string);
  free(entry);
}