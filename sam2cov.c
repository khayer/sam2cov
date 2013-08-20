#include "dbg.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

#define MAX_STRING_LENGTH 200

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

struct Entry
{
  char *read_name;
  char *chr_name;
  int pos;
  int chr_num;
  char *cigar_string;
};

struct Entry *Entry_create(char *read_name, char *chr_name, int pos, int chr_num, char *cigar){
  struct Entry *entry = malloc(sizeof(struct Entry));
  assert(entry != NULL);

  entry->read_name = strdup(read_name);
  entry->chr_name = strdup(chr_name);
  entry->pos = pos;
  entry->chr_num = chr_num;
  entry->cigar_string = strdup(cigar);

  return entry;
}

void Entry_destroy(struct Entry *entry) {
  assert(entry != NULL);

  free(entry->read_name);
  free(entry->chr_name);
  free(entry->cigar_string);
  free(entry);
}

struct Entry *make_entry_for_read(char *line, struct Genome *genome) {
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
    struct Entry *entry = Entry_create(read_name, chr_name, pos, current_chr_number, cigar);
    return entry;
  }
}

int *interpret_cigar_string(struct Entry *entry) {
  static int a[10];
  a[0] = entry->pos;

  char *sep_letters = "MIDNSHP";
  char *ptr_letters;
  char numbers[10][10];
  char *cig = strdup(entry->cigar_string);
  ptr_letters = strtok(cig,sep_letters);
  int i = 0;
  while (ptr_letters != NULL) {

    log_info("I am at %s in %s.",ptr_letters,entry->cigar_string);
    strcpy(numbers[i],ptr_letters);
    ptr_letters = strtok(NULL,sep_letters);
    i++;
  }

  char *sep_numbers = "0123456789";
  char *ptr_numbers;
  char letters[10][1];
  char *cig2 = strdup(entry->cigar_string);
  ptr_numbers = strtok(cig2,sep_numbers);
  i = 0;
  while (ptr_numbers != NULL) {

    log_info("I am at %s in %s.",ptr_numbers,entry->cigar_string);
    strcpy(letters[i],ptr_numbers);
    ptr_numbers = strtok(NULL,sep_numbers);
    i++;
  }
  /*
  int j = 1;
  for (i = 0; i < 10; i++) {
    char letter = letters[i][0];
    switch(letter) {
      case "m":
      case "M":
        a[j] = a[j-1] + atoi(numbers[i]);
        j++;
      case "I":
        printf("Nothing");
      case "D":
        printf("Nothing");
      case "N":
        a[j] = a[j-1] + atoi(numbers[i]);
        j++;
      case "S":
        printf("Nothing");
      case "H":
        printf("Nothing");
      case "P":
        printf("Nothing");
      default:
        break;
    }
  }*/
  for (i = 0; i < 10; i++) {
    printf("Nummer %d: %s\n", i, numbers[i] );
  }
  for (i = 0; i < 10; i++) {
    printf("Nummer %d: %s\n", i, letters[i] );
  }
  for (i = 0; i < 10; i++) {
    printf("Nummer %d: %d\n", i, a[i] );
  }
  return a;
}

void add_reads_to_cov(char *r1_line, char *r2_line, struct Genome *genome,
  int *chromo_lengths,char **names, int num_of_chr){
  struct Entry *entry_r1 = make_entry_for_read(r1_line,genome);
  struct Entry *entry_r2 = make_entry_for_read(r2_line,genome);
  if (entry_r1 == NULL || entry_r2 == NULL) {
    log_err("Ending all processes");
    Genome_destroy(genome);
    for (int i=0; i<num_of_chr; i++) free(names[i]);
    exit(1);
  }
  assert(strcmp(entry_r1->read_name,entry_r2->read_name) == 0);
  int *k;
  k = interpret_cigar_string(entry_r1);
  int *j;
  j = interpret_cigar_string(entry_r2);
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