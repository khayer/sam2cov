#ifndef _chromosome_h
#define _chromosome_h

struct ChromosomeStruct;

typedef struct ChromosomeStruct
{
  char *name;
  int length;
  int cov[];
  /* data */
} Chromosome;

Chromosome *Chromosome_create(char *name, int chr_length);
void Chromosome_destroy(Chromosome *chr);
void Chromosome_print(Chromosome *chr);
void Chromosome_print_to_file(Chromosome *chr, FILE *file_handler);
void Chromosome_update(Chromosome *chr, int pos);
void Chromosome_update_name(Chromosome *chr, char *new_name);
void Chromosome_reset(Chromosome *chr);

#endif