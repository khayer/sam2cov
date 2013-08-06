#include "dbg.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/*
Code framework by Zed A. Shaw
*/

/*struct Person
{
  char *name;
  int age;
  int height;
  int weight;
};

struct Person *Person_create(char *name, int age, int height, int weight)
{
  struct Person *who = malloc(sizeof(struct Person));
  assert(who != NULL);

  who -> name = strdup(name);
  who -> age = age;
  who -> height = height;
  who -> weight = weight;

  return who;
};
*/



void test_debug()
{
    // notice you don't need the \n
    debug("I have Brown Hair.");

    // passing in arguments like printf
    debug("I am %d years old.", 37);
}

void test_log_err()
{
    log_err("I believe everything is broken.");
    log_err("There are %d problems in %s.", 0, "space");
}

void test_log_warn()
{
    log_warn("You can safely ignore this.");
    log_warn("Maybe consider looking at: %s.", "/etc/passwd");
}

void test_log_info()
{
    log_info("Well I did something mundane.");
    log_info("It happened %f times today.", 1.3f);
}

int test_check(char *file_name)
{
    FILE *input = NULL;
    char *block = NULL;

    block = malloc(100);
    check_mem(block); // should work

    input = fopen(file_name,"r");
    check(input, "Failed to open %s.", file_name);

    free(block);
    fclose(input);
    return 0;

error:
    if(block) free(block);
    if(input) fclose(input);
    return -1;
}

int test_sentinel(int code)
{
    char *temp = malloc(100);
    check_mem(temp);

    switch(code) {
        case 1:
            log_info("It worked.");
            break;
        default:
            sentinel("I shouldn't run.");
    }

    free(temp);
    return 0;

error:
    if(temp) free(temp);
    return -1;
}

int test_check_mem()
{
    char *test = NULL;
    check_mem(test);

    free(test);
    return 1;

error:
    return -1;
}

int test_check_debug()
{
    int i = 0;
    check_debug(i != 0, "Oops, I was 0.");

    return 0;
error:
    return -1;
}

struct Chromosome
{
  char *name;
  int length;
  int cov[];
  /* data */
};

void Chromosome_destroy(struct Chromosome *chr)
{
  assert(chr != NULL);

  free(chr->name);
  free(chr);
}

void Chromosome_print(struct Chromosome *chr)
{
  printf("Name: %s\n", chr->name);
  printf("\tLength: %d\n", chr->length);
  for(int i = 0; i<chr->length; i++) {
    printf("\tCoverage at %d: %d\n",i, chr->cov[i]);
  }
}

void Chromosome_update(struct Chromosome *chr, int pos){
  chr->cov[pos] = chr->cov[pos]+1;
}

struct Chromosome *Chromosome_create(char *name, int chr_length)
{
  /* data */
  //struct Chromosome *chr = malloc(sizeof(struct Chromosome)+chr_length* sizeof(int));
  //assert(chr != NULL);

  chr -> name = strdup(name);
  chr -> length = chr_length;
  for (int i = 0; i < chr_length; i++) {
    chr -> cov[i] = 0;
  }

  return chr;
};

struct Genome
{
  int size;
  struct Chromosome chromosomes[];
  /* data */
};

struct Genome *Genome_create(int number_of_chr, char *names[], int legth_of_chromosomes[] )
{
  int size_of_chromosomes = 0;
  for (int i=0; i < number_of_chr; i++) {
    size_of_chromosomes = size_of_chromosomes + sizeof(struct Chromosome) + legth_of_chromosomes[i] * sizeof(int));
  }
  struct Genome *genome = malloc(sizeof(struct Genome)+size_of_chromosomes);
  genome->size = number_of_chr;
  for (int i=0; i < number_of_chr; i++) {
    genome->chromosomes[i] = Chromosome_create(names[i],legth_of_chromosomes[i]);
  }
  return genome;
}


int main(int argc, char *argv[])
{
    check(argc == 2, "Need an argument.");

    struct Chromosome *chr = Chromosome_create("chr1",50);
    Chromosome_update(chr,2);
    Chromosome_update(chr,5);
    Chromosome_print(chr);
    Chromosome_destroy(chr);

    test_debug();
    test_log_err();
    test_log_warn();
    test_log_info();

    check(test_check("ex20.c") == 0, "failed with ex20.c");
    check(test_check(argv[1]) == -1, "failed with argv");
    check(test_sentinel(1) == 0, "test_sentinel failed.");
    check(test_sentinel(100) == -1, "test_sentinel failed.");
    check(test_check_mem() == -1, "test_check_mem failed.");
    check(test_check_debug() == -1, "test_check_debug failed.");



    return 0;

  error:
    return 1;
}