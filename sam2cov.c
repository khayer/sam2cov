#include "dbg.h"
#include "entry.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_STRING_LENGTH 200

/*
Code framework by Zed A. Shaw
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

  //free(chr->name);
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
  struct Chromosome *chr = malloc(sizeof(struct Chromosome)+chr_length* sizeof(int));
  //struct Chromosome *chr = malloc(chr_length* sizeof(int));
  assert(chr != NULL);
  log_info("Creating chromose %s with %d length.", name, chr_length);
  chr->name = name;
  chr->length = chr_length;
  for (int i = 0; i < chr_length; i++) {
    chr->cov[i] = 0;
  }

  return chr;
};

struct Genome
{
  int size;
  struct Chromosome *chromosomes[];
  /* data */
};

struct Genome *Genome_create(int number_of_chr, char **names, int length_of_chromosomes[] )
{
  struct Genome *genome = malloc(sizeof(struct Genome)+number_of_chr*sizeof(struct Chromosome));
  assert(genome != NULL);
  genome->size = number_of_chr;
  for (int i=0; i < number_of_chr; i++) {
    log_info("Creating chromose %s.", names[i]);
    genome->chromosomes[i] = Chromosome_create(names[i],length_of_chromosomes[i]);
  }
  return genome;
}

void Genome_destroy(struct Genome *genome)
{
  assert(genome != NULL);
  for (int i=0; i < genome->size; i++) {
    Chromosome_destroy(genome->chromosomes[i]);
  }
  free(genome);
}

int number_of_chromosomes(char *file_name) {
  FILE *file_handler = fopen(file_name,"r");
  assert(file_handler);
  char line[128];
  int number_of_lines = 0;
  while (fgets( line, sizeof(line), file_handler) != NULL)
  {
    //fputs (line,stdout);
    number_of_lines++;
  }
  fclose(file_handler);
  log_info("There are %d scaffolds/chromosomes in your file.", number_of_lines);
  return number_of_lines;
}

void get_names(char *file_name, int number_of_chromosomes, int *chromo_lengths,char **names) {
  FILE *file_handler = fopen(file_name,"r");
  assert(file_handler);
  char line[90];
  int i = 0;
  char *sep = "\t";
  char *splitted_line;
  while (fgets( line, sizeof(line), file_handler) != NULL)
  {
    splitted_line = strtok(line,sep);
    strcpy(names[i],splitted_line);
    log_info("Name of chromosome is %s.", names[i]);
    chromo_lengths[i] = atoi(strtok(NULL,sep));
    log_info("Length of chromosome is %d.",chromo_lengths[i]);
    i++;
  }
  fclose(file_handler);
}

Entry *make_entry_for_read(char *line, struct Genome *genome) {
  int i = 0;
  char *sep = "\t";
  char *ptr;
  char read_name[100];
  char chr_name[100];
  int pos;
  char cigar[100];
  ptr = strtok(line,sep);
  while (ptr != NULL) {
    switch(i) {
      case 0:
        strcpy(read_name,ptr);
      case 2:
        strcpy(chr_name,ptr);
      case 3:
        pos = atoi(ptr);
      case 5:
        strcpy(cigar,ptr);
    }
    ptr = strtok(NULL,sep);
    i++;
  }
  i = 0;
  int current_chr_number = 123456;
  int found = 0;
  while (i < genome->size) {
    if (strcmp(genome->chromosomes[i]->name,chr_name) == 0) {
      current_chr_number = i;
      found = 1;
      break;
    }
    i++;
  }
  assert(found == 1);
  if (found != 1) {
    log_err("Could not find %s.", chr_name);
    return NULL;
  } else {
    log_info("Chromo_num %d", current_chr_number);
    log_info("Name: %s, Chr: %s, Position: %d, Chromosome Number: %d, Cigar: %s",read_name, chr_name, pos, current_chr_number, cigar);
    Entry *entry = Entry_create(read_name, chr_name, pos, current_chr_number, cigar);
    return entry;
  }
}

void seperate_string(Entry *entry, char *sep, char **array)
{
  char *ptr;
  char *cig = malloc(strlen(entry->cigar_string)+1);//+1 for the zero-terminator
  assert(cig != NULL);
  strcpy(cig,entry->cigar_string);
  ptr = strtok(cig,sep);
  int i = 0;
  while (ptr != NULL) {
    log_info("I am at %s in %s.",ptr,entry->cigar_string);
    strcpy(array[i],ptr);
    ptr = strtok(NULL,sep);
    i++;
  }
  while(i < 10) {
    strcpy(array[i],"0");
    i++;
  }
  //check(i < 10,"Cigar string too long: %s!", entry->cigar_string);
  free(cig);
//error:
  //if (cig) free(cig);
}


int *interpret_cigar_string(Entry *entry) {
  int *a = malloc(10*sizeof(int));
  for (int l = 0; l < 10; l++) {
    a[l] = 0;
  }
  a[0] = entry->pos;

  char *sep_letters = "MIDNSHP";
  char *numbers[10];
  for (int i=0; i<10; i++)
    numbers[i] = malloc(500);
  seperate_string(entry, sep_letters,numbers);

  char *sep_numbers = "0123456789";
  char *letters[10];
  for (int i=0; i<10; i++) {
    letters[i] = malloc(500);
  }
  seperate_string(entry, sep_numbers,letters);

  int j = 1;
  for (int i = 0; i < 10; i++)
  {
    if (letters[i][0] != 0) {
      switch(letters[i][0]) {
        case 'm':
        case 'M':
        case '=':
        case 'X':
          a[j] = a[j-1] + atoi(numbers[i]);
          j++;
          break;
        case 'N':
        case 'D':
          a[j] = a[j-1] + atoi(numbers[i]);
          j++;
          break;
        case 'I':
        case 'S':
        case 'H':
          break;
        case 'P':
          // DON'T KNOW WHAT DO HERE
          break;
        default:
          break;
      }
    }
    i++;
  }

  for (int i = 0; i < 10; i++) {
    printf("Nummer %d: %s\n", i, numbers[i] );
  }
  for (int i = 0; i < 10; i++) {
    printf("Letter %d: %s\n", i, letters[i] );
  }
  for (int i = 0; i < 10; i++) {
    printf("Nummer in a[] %d: %d\n", i, a[i] );
  }
  for (int i=0; i<10; i++) free(numbers[i]);
  for (int i=0; i<10; i++) free(letters[i]);

  return a;
}

int *combine_ranges(int *ranges_r1, int *ranges_r2) {
  int *a = malloc(20*sizeof(int));
  for (int l = 0; l < 20; l++) {
    a[l] = 0;
  }

  int j = 0;
  for (int i = 0; i < 10; i = i + 2)
  {
    for (int k = 0; k < 10; k = k + 2) {
      if (ranges_r1[i] < ranges_r2[k]) {
        a[j] = ranges_r1[i];
        if (ranges_r1[i+1] < ranges_r2[k]) {
          a[j+1] = ranges_r1[i+1];
        } else {
          a[j+1] = ranges_r2[i+1];
        }
        j = j + 2;
      } else {
        a[j] = ranges_r2[i];
        if (ranges_r2[i+1] < ranges_r1[k]) {
          a[j+1] = ranges_r2[i+1];
        } else {
          a[j+1] = ranges_r1[i+1];
        }
        j = j + 2;
      }
    }
  }
  /*
  for (int i = 0; i < 20; i++) {
    printf("Combined in a[] %d: %d\n", i, a[i] );
  }
  */
  return a;
}

void add_reads_to_cov(char *r1_line, char *r2_line, struct Genome *genome,
  int *chromo_lengths,char **names, int num_of_chr){
  Entry *entry_r1 = make_entry_for_read(r1_line,genome);
  Entry *entry_r2 = make_entry_for_read(r2_line,genome);
  if (entry_r1 == NULL || entry_r2 == NULL) {
    log_err("Ending all processes");
    Genome_destroy(genome);
    for (int i=0; i<num_of_chr; i++) free(names[i]);
    exit(1);
  }
  assert(strcmp(entry_r1->read_name,entry_r2->read_name) == 0);
  int *ranges_r1;
  ranges_r1 = interpret_cigar_string(entry_r1);
  int *ranges_r2;
  ranges_r2 = interpret_cigar_string(entry_r2);

  if (strcmp(entry_r1->chr_name,entry_r2->chr_name) == 0){
    /*
    int *combinded_ranges;
    combinded_ranges = combine_ranges(ranges_r1,ranges_r2);
    //update_coverage(combinded_ranges,entry_r1);
    free(combinded_ranges);
    */
  } else {
    //update_coverage(ranges_r1,entry_r1);
    //update_coverage(ranges_r2,entry_r2);
  }


  free(ranges_r2);
  free(ranges_r1);
  Entry_destroy(entry_r1);
  Entry_destroy(entry_r2);
}

int main(int argc, char *argv[])
{

   //char chromo_names[3][14];
    int num_of_chr = number_of_chromosomes("/Users/hayer/Downloads/indexes/danRer7_s.fa.fai");
    //char **chromo_names = malloc(num_of_chr * sizeof(char*));
    int chromo_lengths[num_of_chr];
    //char **chromo_names[num_of_chr] = malloc(num_of_chr * 100 * sizeof(char*));
    char* chromo_names[num_of_chr];
    for (int i=0; i<num_of_chr; ++i)
      chromo_names[i] = malloc(MAX_STRING_LENGTH);

    get_names("/Users/hayer/Downloads/indexes/danRer7_s.fa.fai", num_of_chr,chromo_lengths,chromo_names);
    //log_info("Length of chromosome is nina %d",chromo_lengths[0]);
    //log_info("Length of chromosome is nina %s",chromo_names[0]);
    //check(argc == 2, "Need an argument.");
    struct Genome *genome = Genome_create(num_of_chr,chromo_names,chromo_lengths);

    //Chromosome_update(genome->chromosomes[2],2);
    //Chromosome_update(genome->chromosomes[0],2);
    //Chromosome_update(genome->chromosomes[1],12);
    // OPEN SAM FILE

    char cwd[256];
    getcwd(cwd,sizeof(cwd));
    char *file_name = "/test.sam";

    char *result = malloc(strlen(cwd)+strlen(file_name)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    assert(result != NULL);
    strcpy(result, cwd);
    strcat(result, file_name);
    log_info("Name of sam file is %s.", result);

    FILE *file_handler = fopen(result,"r");
    free(result);
    assert(file_handler);
    char line[500];
    char line_mate[500];
    //int i = 0;
    char *sep = "\t";
    char *splitted_line;

    while (fgets( line, sizeof(line), file_handler) != NULL)
    {
      //fputs (line,stdout);
      sep = "NH:i:";
      char *ptr;
      ptr = strstr(line,sep);
      splitted_line = strtok(ptr,"\t");
      if (strcmp(splitted_line,"NH:i:1")==0) {
        fgets( line_mate, sizeof(line_mate), file_handler);
        add_reads_to_cov(line,line_mate,genome,chromo_lengths,
          chromo_names,num_of_chr);
      }
      //splitted_line = strtok(line,sep);
      fputs (splitted_line,stdout);
      //strcpy(names[i],splitted_line);
      //log_info("Name of chromosome is %s.", names[i]);
      //chromo_lengths[i] = atoi(strtok(NULL,sep));
      //log_info("Length of chromosome is %d.",chromo_lengths[i]);
      //i++;
    }
    assert(file_handler);
    fclose(file_handler);

    //Chromosome_print(genome->chromosomes[0]);
    //Chromosome_print(genome->chromosomes[1]);
    //Chromosome_print(genome->chromosomes[2]);
    Genome_destroy(genome);

    for (int i=0; i<num_of_chr; i++) free(chromo_names[i]);
    //free(chromo_names);
    //struct Chromosome *chr = Chromosome_create("chr1",50);
    //Chromosome_update(chr,2);
    //Chromosome_update(chr,5);
    //Chromosome_print(chr);
    //Chromosome_destroy(chr);

/*
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
*/


    return 0;

  error:
    return 1;
}