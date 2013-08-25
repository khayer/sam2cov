#include "dbg.h"
#include "functions.h"
#include "entry.h"
#include "chromosome.h"
#include "genome.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

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

Entry *make_entry_for_read(char *line, Genome *genome) {
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
    //log_info("Chromo_num %d", current_chr_number);
    //log_info("Name: %s, Chr: %s, Position: %d, Chromosome Number: %d, Cigar: %s",read_name, chr_name, pos, current_chr_number, cigar);
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

