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

// run_sam2cov(genome, unique_file, sam_file,
//  num_of_chr, chromo_lengths, chromo_names, unique_mode, rum);

void run_sam2cov(Genome *genome, char *unique_file,
  char *sam_file, int num_of_chr, int *chromo_lengths,
  char **chromo_names, int unique_mode, int rum) {

  FILE *file_handler = fopen(sam_file,"r");
  assert(file_handler);
  char line[500];
  char line_mate[500];
  char *sep = "\t";
  char *splitted_line;
  while (fgets( line, sizeof(line), file_handler) != NULL)
  {
    char *dummy = malloc(sizeof("@")+1);
    assert(dummy != NULL);
    strncpy(dummy,line,1);
    fputs (dummy,stdout);
    fputs (line,stdout);
    Entry *entry = NULL;
    char *line_cpy = malloc(strlen(line)+1);
    strcpy(line_cpy, line);
    if (!strcmp(dummy,"@")==0) {
      entry = make_entry_for_read(line_cpy,genome);
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
  }
  assert(file_handler);
  fclose(file_handler);
  FILE *fp = fopen(unique_file,"a");
  for (int i = 0; i < num_of_chr; ++i)
  {
    Chromosome_print_to_file(genome->chromosomes[i], fp);
  }
}

int main(int argc, char *argv[])
{
  char *fai_file=NULL;
  char *sam_file=NULL;
  char *unique_file=NULL;
  char *non_unique_file=NULL;
  int unique_mode = 1;
  int rum;
  if (argc != 6) {
    printf("Usage: sam2cov fai_file sam_file unique_file non_unique_file rum? \n" );
    exit(1);
  }

  fai_file = argv[1];
  sam_file = argv[2];
  unique_file = argv[3];
  non_unique_file = argv[4];
  rum = atoi(argv[5]);

  int num_of_chr = number_of_chromosomes(fai_file);
  int chromo_lengths[num_of_chr];
  char* chromo_names[num_of_chr];
  for (int i=0; i<num_of_chr; ++i)
    chromo_names[i] = malloc(MAX_STRING_LENGTH);
  get_names(fai_file, num_of_chr,chromo_lengths,chromo_names);
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

  run_sam2cov(genome, unique_file, sam_file,
    num_of_chr, chromo_lengths, chromo_names, unique_mode, rum);




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