#include "dbg.h"
#include "chromosome.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

void Chromosome_destroy(Chromosome *chr)
{
  assert(chr != NULL);
  free(chr);
}

void Chromosome_print(Chromosome *chr)
{
  printf("Name: %s\n", chr->name);
  printf("\tLength: %d\n", chr->length);
  for(int i = 0; i<chr->length; i++) {
    printf("\tCoverage at %d: %d\n",i, chr->cov[i]);
  }
}

void Chromosome_print_to_file(Chromosome *chr, FILE *file_handler)
{
  //printf("Name: %s\n", chr->name);
  //printf("\tLength: %d\n", chr->length);
  int range_length = 0;
  int last_number = 0;
  int start_pos = 0;
  for(int i = 0; i<chr->length; i++) {
    if (chr->cov[i] != 0) {
      if (last_number == 0) {
        last_number = chr->cov[i];
        start_pos = i;
      }
      if (last_number != chr->cov[i]) {
        fprintf(file_handler,"%s\t%d\t%d\t%d\n",chr->name, start_pos, start_pos+range_length,last_number);
        start_pos = i;
        last_number = chr->cov[i];
        range_length = 0;
      } 
      range_length++;
    }
  }
}

void Chromosome_update(Chromosome *chr, int pos){
  chr->cov[pos] = chr->cov[pos]+1;
}

Chromosome *Chromosome_create(char *name, int chr_length)
{
  Chromosome *chr = malloc(sizeof(Chromosome)+chr_length* sizeof(int));
  //struct Chromosome *chr = malloc(chr_length* sizeof(int));
  assert(chr != NULL);
  log_info("Creating chromosome %s with %d length.", name, chr_length);
  chr->name = name;
  chr->length = chr_length;
  for (int i = 0; i < chr_length; i++) {
    chr->cov[i] = 0;
  }

  return chr;
};