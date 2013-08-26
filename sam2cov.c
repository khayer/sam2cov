#include "dbg.h"
#include "entry.h"
#include "chromosome.h"
#include "genome.h"
#include "functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

int *combine_ranges(int *ranges_r1, int *ranges_r2) {
  int *a = malloc(20*sizeof(int));
  for (int l = 0; l < 20; l++) {
    a[l] = 0;
  }

  int *starts_r1 = malloc(5*sizeof(int));
  for (int l = 0; l < 5; l++) {
    starts_r1[l] = ranges_r1[l*2];
  }
  int *starts_r2 = malloc(5*sizeof(int));
  for (int l = 0; l < 5; l++) {
    starts_r2[l] = ranges_r2[l*2];
  }
  int *stops_r1 = malloc(5*sizeof(int));
  for (int l = 0; l < 5; l++) {
    stops_r1[l] = ranges_r1[l*2+1];
  }
  int *stops_r2 = malloc(5*sizeof(int));
  for (int l = 0; l < 5; l++) {
    stops_r2[l] = ranges_r2[l*2+1];
  }

  int c_a = 0;
  int c_r1 = 0;
  int c_r2 = 0;
  while(starts_r1[c_r1]!=0 || starts_r2[c_r2]!=0)
  {
    if ((starts_r1[c_r1] <= starts_r2[c_r2] || starts_r2[c_r2] == 0) && starts_r1[c_r1]!=0) {
      a[c_a] = starts_r1[c_r1];
      if (stops_r1[c_r1] <= starts_r2[c_r2] || starts_r2[c_r2] == 0) {
        a[c_a + 1] = stops_r1[c_r1]+1;
        c_a = c_a + 2;
        if (c_r1 < 4) c_r1++;
      } else {
        if (stops_r1[c_r1] <= stops_r2[c_r2]) {
          a[c_a + 1] = stops_r2[c_r2]+1;
          c_a = c_a + 2;
          if (c_r1 < 4) c_r1++;
          if (c_r2 < 3) c_r2++;
        } else {
          a[c_a + 1] = stops_r1[c_r1]+1;
          c_a = c_a + 2;
          if (c_r1 < 4) c_r1++;
        }
      }
    } else if (starts_r2[c_r2]!=0) {
      a[c_a] = starts_r2[c_r2];
      if (stops_r2[c_r2] <= starts_r1[c_r1] || starts_r1[c_r1] == 0) {
        a[c_a + 1] = stops_r2[c_r2]+1;
        c_a = c_a + 2;
        if (c_r2 < 4) c_r2++;
      } else {
        if (stops_r2[c_r2] <= stops_r1[c_r1]) {
          a[c_a + 1] = stops_r1[c_r1]+1;
          c_a = c_a + 2;
          if (c_r1 < 3) c_r1++;
          if (c_r2 < 4) c_r2++;
        } else {
          a[c_a + 1] = stops_r2[c_r2]+1;
          c_a = c_a + 2;
          if (c_r2 < 4) c_r2++;
        }
      }
    }
  }
  free(starts_r1);free(stops_r1);
  free(starts_r2);free(stops_r2);
  for (int i = 0; i < 20; i++) {
    printf("Combined in a[] %d: %d\n", i, a[i] );
  }
  return a;
}

void update_coverage(int *ranges, Entry *entry, Genome *genome){
  int *starts = malloc(10*sizeof(int));
  for (int l = 0; l < 10; l++) {
    starts[l] = ranges[l*2];
  }
  int *stops = malloc(10*sizeof(int));
  for (int l = 0; l < 10; l++) {
    stops[l] = ranges[l*2+1];
  }

  int i = 0;
  while (starts[i] != 0) {
    //fputs(starts[i],stdout);
    printf("Starts %d \n", starts[i] );
    printf("Stops %d \n", stops[i] );
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

    int *combinded_ranges;
    combinded_ranges = combine_ranges(ranges_r1,ranges_r2);
    update_coverage(combinded_ranges,entry_r1, genome);
    free(combinded_ranges);
  } else {
    update_coverage(ranges_r1,entry_r1,genome);
    update_coverage(ranges_r2,entry_r2,genome);
  }

  free(ranges_r2);
  free(ranges_r1);
  Entry_destroy(entry_r1);
  Entry_destroy(entry_r2);
}

int main(int argc, char *argv[])
{
      char *fai_file=NULL;
      char *sam_file=NULL;
      char *unique_file=NULL;
      int rum;

      if (argc != 5) {
        printf("Usage: sam2cov fai_file sam_file unique_file rum? \n" );
        exit(1);
      }

      fai_file = argv[1];
      sam_file = argv[2];
      unique_file =argv[3];
      rum = atoi(argv[4]);




   //char chromo_names[3][14];
    int num_of_chr = number_of_chromosomes(fai_file);
    //char **chromo_names = malloc(num_of_chr * sizeof(char*));
    int chromo_lengths[num_of_chr];
    //char **chromo_names[num_of_chr] = malloc(num_of_chr * 100 * sizeof(char*));
    char* chromo_names[num_of_chr];
    for (int i=0; i<num_of_chr; ++i)
      chromo_names[i] = malloc(MAX_STRING_LENGTH);
    get_names(fai_file, num_of_chr,chromo_lengths,chromo_names);
    //log_info("Length of chromosome is nina %d",chromo_lengths[0]);
    //log_info("Length of chromosome is nina %s",chromo_names[0]);
    //check(argc == 2, "Need an argument.");

    Genome *genome = Genome_create(num_of_chr,chromo_names,chromo_lengths);

    //Chromosome_update(genome->chromosomes[2],2);
    //Chromosome_update(genome->chromosomes[0],2);
    //Chromosome_update(genome->chromosomes[1],12);
    // OPEN SAM FILE
    //char cwd[256];
    //getcwd(cwd,sizeof(cwd));
    //char *file_name = "/test.sam";

    //char *result = malloc(strlen(cwd)+strlen(file_name)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    //assert(result != NULL);
    //strcpy(result, cwd);
    //strcat(result, file_name);
    //log_info("Name of sam file is %s.", result);

    FILE *file_handler = fopen(sam_file,"r");
    assert(file_handler);
    char line[500];
    char line_mate[500];
    //int i = 0;
    char *sep = "\t";
    char *splitted_line;

    while (fgets( line, sizeof(line), file_handler) != NULL)
    {
      char *dummy = malloc(strlen("@")+1);//+1 for the zero-terminator
      assert(dummy != NULL);
      strncpy(dummy,line,1);
      fputs (line,stdout);
      Entry *entry = NULL;
      char *line_cpy = malloc(strlen(line)+1);//+1 for the zero-terminator
    //assert(result != NULL);
      strcpy(line_cpy, line);

      if (!strcmp(dummy,"@")==0) {
        entry = make_entry_for_read(line_cpy,genome);
        //fputs(entry->chr_name, stdout);
        if (rum != 1 && entry!=NULL) {
          sep = "NH:i:";
          char *ptr;
          ptr = strstr(line,sep);
          splitted_line = strtok(ptr,"\t");
          if (strcmp(splitted_line,"NH:i:1")==0) {
            fgets( line_mate, sizeof(line_mate), file_handler);
            add_reads_to_cov(line,line_mate,genome,chromo_lengths,
              chromo_names,num_of_chr);
          }
        } else if (entry != NULL) {
          sep = "IH:i:";
          char *ptr;
          ptr = strstr(line,sep);
          splitted_line = strtok(ptr,"\t");
          if (strcmp(splitted_line,"IH:i:1")==0) {
            fgets( line_mate, sizeof(line_mate), file_handler);
            add_reads_to_cov(line,line_mate,genome,chromo_lengths,
              chromo_names,num_of_chr);
          }
        }
      }
      free(dummy); free(line_cpy); if (entry != NULL) Entry_destroy(entry);
      //splitted_line = strtok(line,sep);
      //fputs (splitted_line,stdout);
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
    FILE *fp = fopen(unique_file,"a");
    for (int i = 0; i < num_of_chr; ++i)
    {
      //Chromosome_print(genome->chromosomes[i]);
      Chromosome_print_to_file(genome->chromosomes[i], fp);
    }


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