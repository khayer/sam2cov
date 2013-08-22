#ifndef _chromosome_h
#define _chromosome_h

/*
// makes THE_SIZE in ex22.c available to other .c files
extern int THE_SIZE;

// gets and sets an internal static variable in ex22.c
int get_age();
void set_age(int age);

// updates a static variable that's inside update_ratio
double update_ratio(double ratio);

void print_size();
*/
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
void Chromosome_update(Chromosome *chr, int pos);

#endif