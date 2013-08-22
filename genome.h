#ifndef _genome_h
#define _genome_h

struct GenomeStruct;

typedef struct GenomeStruct
{
  int size;
  Chromosome *chromosomes[];
} Genome;

Genome *Genome_create(int number_of_chr, char **names, int length_of_chromosomes[] );
void Genome_destroy(Genome *genome);

#endif


