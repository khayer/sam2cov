#ifndef _functions_h
#define _functions_h

#include "functions.h"
#include "entry.h"
#include "chromosome.h"
#include "genome.h"

#define MAX_STRING_LENGTH 200

int get_strand(int bit_flag);

int number_of_chromosomes(char *file_name);

void get_names(char *file_name,
  int number_of_chromosomes, int *chromo_lengths,char **names);

Entry *make_entry_for_read(char *line, Genome *genome);

void seperate_string(Entry *entry, char *sep, char **array, int size_of_array);

int *interpret_cigar_string(Entry *entry, int size_of_array);

int *combine_ranges(int *ranges_r1, int *ranges_r2, int size_of_array);

void update_coverage(int *ranges, Entry *entry, Genome *genome, int size_of_array);

int add_reads_to_cov(char *r1_line, char *r2_line, Genome *genome,
  int *chromo_lengths,char **names, int num_of_chr, int strand);

void add_reads_to_cov_single(char *r1_line, Genome *genome,
  int *chromo_lengths,char **names, int num_of_chr, int strand);

int fail_on_purpose(const char *msg);

char* uppercase(char *msg);

char* lowercase(char *msg);

int compare_two_files(char *file1, char *file2);

int string_to_number(char *string, int max_cutoff);

int compare_names(char *line,char *line_mate,Genome *genome);

int write_to_file(char *line, int max_file_num);

#endif