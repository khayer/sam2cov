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
  char read_name[500];
  char chr_name[500];
  int pos;
  char cigar[500];
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
    Entry *entry = Entry_create(read_name, chr_name, pos, current_chr_number, cigar);
    return entry;
  }
}

void seperate_string(Entry *entry, char *sep, char **array, int size_of_array)
{
  //int size_of_array = strlen(entry->cigar_string);
  char *ptr;
  char *cig = malloc(strlen(entry->cigar_string)+1);//+1 for the zero-terminator
  assert(cig != NULL);
  strcpy(cig,entry->cigar_string);
  ptr = strtok(cig,sep);
  int i = 0;
  while (ptr != NULL) {
    //log_info("I am at %s in %s.",ptr,entry->cigar_string);
    strcpy(array[i],ptr);
    ptr = strtok(NULL,sep);
    i++;
  }
  while(i < size_of_array) {
    strcpy(array[i],"0");
    i++;
  }

  free(cig);
}


int *interpret_cigar_string(Entry *entry, int size_of_array) {
  //int size_of_array = strlen(entry->cigar_string);
  int *a = malloc(size_of_array*sizeof(int));
  for (int l = 0; l < size_of_array; l++) {
    a[l] = 0;
  }
  a[0] = entry->pos;

  char *sep_letters = "MIDNSHP";
  char *numbers[size_of_array];
  for (int i=0; i<size_of_array; i++)
    numbers[i] = malloc(500);
  seperate_string(entry, sep_letters,numbers, size_of_array);

  char *sep_numbers = "0123456789";
  char *letters[size_of_array];
  for (int i=0; i<size_of_array; i++) {
    letters[i] = malloc(500);
  }
  seperate_string(entry, sep_numbers,letters, size_of_array);
  if (strcmp(&letters[0][0],"*") == 0) a[0] = 0;
  int j = 1;
  int insertion = 0;
  for (int i = 0; i < size_of_array; i++)
  {
    //if (letters[i][0] != 0) {
      switch(letters[i][0]) {
        case 'm':
        case 'M':
        case '=':
        case 'X':
          if (insertion == 0) {
            a[j] = a[j-1] + atoi(numbers[i]);
            j++;
          } else {
            a[j-1] = a[j-1] + atoi(numbers[i]);
            insertion = 0;
          }

          break;
        case 'N':
        case 'D':
          a[j] = a[j-1] + atoi(numbers[i]);
          j++;
          break;
        case 'I':
          insertion = 1;
        case '*':
        case 'S':
        case 'H':
          break;
        case 'P':
          // DON'T KNOW WHAT DO HERE
          break;
        default:
          break;
      }
    //}
  }

  //for (int i = 0; i < size_of_array; i++) {
  //  printf("Nummer %d: %s\n", i, numbers[i] );
  //}
  //for (int i = 0; i < size_of_array; i++) {
  //  printf("Letter %d: %s\n", i, letters[i] );
  //}
  //for (int i = 0; i < size_of_array; i++) {
  //  printf("Nummer in a[] %d: %d\n", i, a[i] );
  //}
  for (int i=0; i<size_of_array; i++) free(numbers[i]);
  for (int i=0; i<size_of_array; i++) free(letters[i]);

  return a;
}

int *combine_ranges(int *ranges_r1, int *ranges_r2, int size_of_array) {
  int *a = malloc(size_of_array*sizeof(int));
  for (int l = 0; l < size_of_array; l++) {
    a[l] = 0;
  }
  //log_info("size_of_array / 2 = %d", size_of_array/2);
  int *starts_r1 = malloc(size_of_array/2*sizeof(int));
  for (int l = 0; l < size_of_array/2; l++) {
    starts_r1[l] = ranges_r1[l*2];
  }

  int *starts_r2 = malloc(size_of_array/2*sizeof(int));
  for (int l = 0; l < size_of_array/2; l++) {
    starts_r2[l] = ranges_r2[l*2];
  }

  int *stops_r1 = malloc(size_of_array/2*sizeof(int));
  for (int l = 0; l < size_of_array/2; l++) {
    stops_r1[l] = ranges_r1[l*2+1];
  }

  int *stops_r2 = malloc(size_of_array/2*sizeof(int));
  for (int l = 0; l < size_of_array/2; l++) {
    stops_r2[l] = ranges_r2[l*2+1];
  }

  int c_a = 0;
  int c_r1 = 0;
  int c_r2 = 0;
  while(stops_r1[c_r1]!=0 || stops_r2[c_r2]!=0)
  {
    if ((starts_r1[c_r1] < starts_r2[c_r2] || stops_r2[c_r2] == 0) && stops_r1[c_r1]!=0) {
      a[c_a] = starts_r1[c_r1];
      if (stops_r1[c_r1] <= starts_r2[c_r2] || stops_r2[c_r2] == 0) {
        a[c_a + 1] = stops_r1[c_r1];
        c_a = c_a + 2;
        if (c_r1 < size_of_array/2) c_r1++;
      } else {
        if (stops_r1[c_r1] <= stops_r2[c_r2]) {
          a[c_a + 1] = stops_r2[c_r2];
          c_a = c_a + 2;
          if (c_r1 < size_of_array/2) c_r1++;
          if (c_r2 < size_of_array/2) c_r2++;
        } else {
          a[c_a + 1] = stops_r1[c_r1];
          c_a = c_a + 2;
          if (c_r1 < size_of_array/2) c_r1++;
          if (c_r2 < size_of_array/2) c_r2++;
        }
      }
    } else if (stops_r2[c_r2]!=0) {
      a[c_a] = starts_r2[c_r2];
      if (stops_r2[c_r2] <= starts_r1[c_r1] || stops_r1[c_r1] == 0) {
        a[c_a + 1] = stops_r2[c_r2];
        c_a = c_a + 2;
        if (c_r2 < size_of_array/2) c_r2++;
      } else {
        if (stops_r2[c_r2] <= stops_r1[c_r1]) {
          a[c_a + 1] = stops_r1[c_r1];
          c_a = c_a + 2;
          if (c_r1 < size_of_array/2) c_r1++;
          if (c_r2 < size_of_array/2) c_r2++;
        } else {
          a[c_a + 1] = stops_r2[c_r2];
          c_a = c_a + 2;
          if (c_r2 < size_of_array/2) c_r2++;
          if (c_r1 < size_of_array/2) c_r1++;
        }
      }
    }
  }
  //log_info("Now I am going to free the memory!");
  free(starts_r1);free(stops_r1);
  free(starts_r2);free(stops_r2);
  //for (int i = 0; i < size_of_array; i++) {
  //  printf("Combined in a[] %d: %d\n", i, a[i] );
  //}
  return a;
}

void update_coverage(int *ranges, Entry *entry, Genome *genome, int size_of_array){
  int *starts = malloc(size_of_array/2*sizeof(int));
  for (int l = 0; l < size_of_array/2; l++) {
    starts[l] = ranges[l*2];
  }
  int *stops = malloc(size_of_array/2*sizeof(int));
  for (int l = 0; l < size_of_array/2; l++) {
    stops[l] = ranges[l*2+1];
  }

  int i = 0;
  while (stops[i] != 0) {
    //printf("Starts %d \n", starts[i] );
    //printf("Stops %d \n", stops[i] );
    for (int k = starts[i]; k < stops[i]; k++) {
      Chromosome_update(genome->chromosomes[entry->chr_num], k);
    }
    i++;
  }
  free(starts); free(stops);
}

void add_reads_to_cov(char *r1_line, char *r2_line, Genome *genome,
  int *chromo_lengths,char **names, int num_of_chr){
  Entry *entry_r1 = make_entry_for_read(r1_line,genome);
  Entry *entry_r2 = make_entry_for_read(r2_line,genome);

  int size_of_array;
  if (strlen(entry_r1->cigar_string) > strlen(entry_r2->cigar_string)) {
    size_of_array = strlen(entry_r1->cigar_string) * 2;
  } else {
    size_of_array = strlen(entry_r2->cigar_string) * 2;
  }

  if (entry_r1 == NULL || entry_r2 == NULL) {
    log_err("Ending all processes");
    Genome_destroy(genome);
    for (int i=0; i<num_of_chr; i++) free(names[i]);
    exit(1);
  }
  assert(strcmp(entry_r1->read_name,entry_r2->read_name) == 0);
  int *ranges_r1;
  ranges_r1 = interpret_cigar_string(entry_r1,size_of_array);
  int *ranges_r2;
  ranges_r2 = interpret_cigar_string(entry_r2,size_of_array);


  if (strcmp(entry_r1->chr_name,entry_r2->chr_name) == 0){

    int *combinded_ranges;
    combinded_ranges = combine_ranges(ranges_r1,ranges_r2,size_of_array);
    update_coverage(combinded_ranges,entry_r1,genome,size_of_array);
    free(combinded_ranges);
  } else {
    update_coverage(ranges_r1,entry_r1,genome,size_of_array);
    update_coverage(ranges_r2,entry_r2,genome,size_of_array);
  }

  free(ranges_r2);
  free(ranges_r1);
  Entry_destroy(entry_r1);
  Entry_destroy(entry_r2);
}
