#include "dbg.h"
#include "chromosome.h"
#include "genome.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

Genome *Genome_create(int number_of_chr, char **names, int length_of_chromosomes[] )
{
  Genome *genome = malloc(sizeof(Genome)+number_of_chr*sizeof(Chromosome));
  assert(genome != NULL);
  genome->size = number_of_chr;
  for (int i=0; i < number_of_chr; i++) {
    log_info("Creating chromose %s.", names[i]);
    genome->chromosomes[i] = Chromosome_create(names[i],length_of_chromosomes[i]);
  }
  return genome;
}

void Genome_destroy(Genome *genome)
{
  assert(genome != NULL);
  for (int i=0; i < genome->size; i++) {
    Chromosome_destroy(genome->chromosomes[i]);
  }
  free(genome);
}

void Genome_reset(Genome *genome)
{
  assert(genome != NULL);
  for (int i=0; i < genome->size; i++) {
    Chromosome_reset(genome->chromosomes[i]);
  }
}