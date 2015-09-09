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
//#define VERSION "v0.0.1-beta - 9/3/13"
//#define VERSION "v0.0.2-beta - 3/20/14"
//#define VERSION "v0.0.3-beta - 4/15/14"
//#define VERSION "v0.0.4-beta - 4/29/14"
//#define VERSION "v0.0.5.1-beta - 6/15/15"
//#define VERSION "v0.0.5.2-beta - 6/15/15"
//#define VERSION "v0.0.5.3-beta - 6/19/15"
#define VERSION "v0.0.5.4-beta - 7/9/15"

void usage() {
  printf("Usage: sam2cov [OPTIONS] fai_file sam_file\n" );
  printf("Options:\n" );
  printf("\t-r\tAligned with RUM? [0/1] Default: 0\n" );
  printf("\t-s\tStrand: 1 for fwd, 2 for rev [0/1/2]. Default: 0\n" );
  printf("\t\tPlease note: Reads are assumed to be in Illumina R2-R1 orientation!\n" );
  printf("\t-e\tPaired end mode [0/1] Default: 1\n" );
  printf("\t-p\tPrefix for coverage files. Default: Unique.cov, NU.cov\n" );
  printf("\t-u\tPrint header for UCSC Genome browser? [0/1] Default: 0\n");
  //printf("\t-c\tAdd \"chr\" in front of location (ensembl genes)? [0/1] Default: 0\n");
  printf("\t-h\tThis helpful message.\n" );
  printf("\t-v\tPrint Version.\n" );
  exit(1);
}

int StartsWith(const char *a, const char *b)
{
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}


void run_sam2cov_single_end(Genome *genome, char *out_file,
  char *sam_file, int num_of_chr, int *chromo_lengths,
  char **chromo_names, int unique_mode, int rum, int strand, int ucsc_header) {

  FILE *file_handler = fopen(sam_file,"r");
  assert(file_handler);
  char line[5000];
  //char line_mate[5000];
  char *sep = "\t";
  char *splitted_line;
  while (fgets( line, sizeof(line), file_handler) != NULL)
  {
    //char *dummy = malloc(strlen("@"));
    //char dummy[1];
    //assert(dummy != NULL);
    //dummy = "@";
    //strncpy(dummy,line,1);
    //s.replace(s.find("$name"), sizeof("Somename")-1, "Somename");
    //*dummy.replace(*dummy.find("\ufffd", sizeof(""),"");
    //[dummy stringByReplacingOccurrencesOfString:@"\ufffd" withString:@" "];
    //log_info("Compare of dummy %s and @ is %d. Line is %s", &dummy[0], strcmp(&dummy[0],"@"),line);
    //fputs (strcmp(dummy,"@"), stdout);
    //fputs (dummy,stdout);
    //fputs (line,stdout);
    Entry *entry = NULL;
    char *line_cpy = malloc(strlen(line)+1);
    strcpy(line_cpy, line);
    //if (!(strcmp(&dummy[0],"@")==0)) {
    if (!StartsWith(line,"@")) {
      //exit(1);
      //log_info("Got here without prob");
      entry = make_entry_for_read(line_cpy,genome);
      if (rum != 1 && entry!=NULL) {
        sep = "NH:i:";
        char *ptr;
        ptr = strstr(line,sep);
        splitted_line = strtok(ptr,"\t");
        if ((strcmp(splitted_line,"NH:i:1")==0 && unique_mode==1) ||
          (strcmp(splitted_line,"NH:i:1")!=0 && unique_mode!=1)) {
          add_reads_to_cov_single(line,genome,chromo_lengths,
            chromo_names,num_of_chr,strand);
        }
      } else if (entry != NULL) {
        sep = "IH:i:";
        char *ptr;
        ptr = strstr(line,sep);
        splitted_line = strtok(ptr,"\t");
        if ((strcmp(splitted_line,"IH:i:1")==0 && unique_mode==1) ||
          (strcmp(splitted_line,"IH:i:1")!=0 && unique_mode!=1)) {
          //fgets( line_mate, sizeof(line_mate), file_handler);
          add_reads_to_cov_single(line,genome,chromo_lengths,
            chromo_names,num_of_chr,strand);
        }
      }
    }
    //free(dummy);
    free(line_cpy); if (entry != NULL) Entry_destroy(entry);
  }
  assert(file_handler);
  fclose(file_handler);
  FILE *fp = fopen(out_file,"w");
  if (ucsc_header == 1 && unique_mode == 1) {
    fprintf(fp, "track type=bedGraph name=\"Coverage Unique for %s\" description=\"Coverage Unique for %s\" visibility=full color=255,0,0 priority=20\n", sam_file, sam_file);
  } else if (ucsc_header == 1) {
    fprintf(fp, "track type=bedGraph name=\"Coverage Non-unique for %s\" description=\"Coverage Non-unique for %s\" visibility=full color=255,0,0 priority=20\n", sam_file, sam_file);
  }
  for (int i = 0; i < num_of_chr; ++i)
  {
    Chromosome_print_to_file(genome->chromosomes[i], fp);
  }
  fclose(fp);
}

void run_sam2cov(Genome *genome, char *out_file,
  char *sam_file, int num_of_chr, int *chromo_lengths,
  char **chromo_names, int unique_mode, int rum, int strand, int ucsc_header) {

  FILE *file_handler = fopen(sam_file,"r");
  assert(file_handler);
  char line[5000];
  char line_cpy[5000];
  char line_mate[5000];
  char mate_cpy[5000];
  char *sep = "\t";
  char *splitted_line;
  char last_read_name[5000];
  strcpy(last_read_name,"");
  while (fgets( line, sizeof(line), file_handler) != NULL)
  {
    //char *dummy = malloc(strlen("@"));
    //char dummy[1];
    //assert(dummy != NULL);
    //dummy = "@";
    //strncpy(dummy,line,1);
    //s.replace(s.find("$name"), sizeof("Somename")-1, "Somename");
    //*dummy.replace(*dummy.find("\ufffd", sizeof(""),"");
    //[dummy stringByReplacingOccurrencesOfString:@"\ufffd" withString:@" "];
    //log_info("Compare of dummy %s and @ is %d. Line is %s", &dummy[0], strcmp(&dummy[0],"@"),line);
    //fputs (strcmp(dummy,"@"), stdout);
    //fputs (dummy,stdout);
    //fputs (line,stdout);
    Entry *entry = NULL;
    Entry *entry_mate = NULL;
    //char *line_cpy = malloc(strlen(line)+1);
    int hit = 0;
    strcpy(line_cpy, line);
    //if (!(strcmp(&dummy[0],"@")==0)) {
    if (!StartsWith(line,"@")) {
      //exit(1);
      //log_info("Got here without prob");
      entry = make_entry_for_read(line_cpy,genome);

      if (strcmp(last_read_name,"")==0) {
        strcpy(last_read_name,entry->read_name);
      }
      //log_info("READNAME %s",last_read_name);
      while (hit != 1) {

        if (fgets( line_mate, sizeof(line_mate), file_handler) == NULL) {
          //log_err("Where is the line_mate?");
          ////free(line_cpy);
          //if (entry != NULL) Entry_destroy(entry);
          ////free(mate_cpy);
          //if (entry_mate != NULL) Entry_destroy(entry_mate);

          //return;
        } else {
          //char *mate_cpy = malloc(strlen(line_mate)+1);
          strcpy(mate_cpy, line_mate);
          if (entry_mate != NULL) Entry_destroy(entry_mate);
          entry_mate = make_entry_for_read(mate_cpy,genome);
        }

        if (strcmp(entry_mate->read_name,entry->read_name) != 0) {
          // Treat entry TODO
          if (entry != NULL) Entry_destroy(entry);
          strcpy(mate_cpy, line_mate);
          strcpy(line, mate_cpy);
          strcpy(line_cpy, line);
          entry = make_entry_for_read(mate_cpy,genome);

        } else {
          hit = 1;
        }
      }

      if (rum != 1 && (entry->chr_num != -1 || entry_mate->chr_num != -1)) {
        sep = "NH:i:";
        char *ptr;
        ptr = strstr(line,sep);
        //log_info("entry: %s, entry_mate: %s", entry->read_name, entry_mate->read_name);
        //log_info("PTRRRR %s",ptr);
        //log_info("LINEEEEE %s",line);
        //splitted_line = strtok(ptr,"\t");
        if (ptr == NULL) {
          ptr = strstr(line_mate,sep);
          splitted_line = strtok(ptr,"\t");
        } else {
          splitted_line = strtok(ptr,"\t");
        }
        //log_info("splitted_line %s" , splitted_line);
        if ((strcmp(splitted_line,"NH:i:1")==0 && unique_mode==1) ||
          (strcmp(splitted_line,"NH:i:1")!=0 && unique_mode!=1)) {
          add_reads_to_cov(entry,entry_mate,genome,chromo_lengths,
            chromo_names,num_of_chr,strand);
        }
      } else if (entry->chr_num != -1 || entry_mate->chr_num != -1) {
        sep = "IH:i:";
        char *ptr;
        ptr = strstr(line,sep);
        splitted_line = strtok(ptr,"\t");
        if ((strcmp(splitted_line,"IH:i:1")==0 && unique_mode==1) ||
          (strcmp(splitted_line,"IH:i:1")!=0 && unique_mode!=1)) {
          fgets( line_mate, sizeof(line_mate), file_handler);
          add_reads_to_cov(entry,entry_mate,genome,chromo_lengths,
            chromo_names,num_of_chr,strand);
        }
      }
    }
    //free(dummy);
    //free(line_cpy);
    if (entry != NULL) Entry_destroy(entry);
    //free(mate_cpy);
    if (entry_mate != NULL) Entry_destroy(entry_mate);
  }
  assert(file_handler);
  fclose(file_handler);
  FILE *fp = fopen(out_file,"w");
  if (ucsc_header == 1 && unique_mode == 1) {
    fprintf(fp, "track type=bedGraph name=\"Coverage Unique for %s\" description=\"Coverage Unique for %s\" visibility=full color=255,0,0 priority=20\n", sam_file, sam_file);
  } else if (ucsc_header == 1) {
    fprintf(fp, "track type=bedGraph name=\"Coverage Non-unique for %s\" description=\"Coverage Non-unique for %s\" visibility=full color=255,0,0 priority=20\n", sam_file, sam_file);
  }
  for (int i = 0; i < num_of_chr; ++i)
  {
    Chromosome_print_to_file(genome->chromosomes[i], fp);
  }
  fclose(fp);
}

int main(int argc, char *argv[])
{
  //char source[1000], destination[1000];
//
  //printf("Input a string\n");
  //gets(source);
//
  //strcpy(destination, source);
//
  //printf("Source string:      \"%s\"\n", source);
  //printf("Destination string: \"%s\"\n", destination);
//
  //printf("Input a string\n");
  //gets(source);
//
//printf("Source string:      \"%s\"\n", source);
  //printf("Destination string: \"%s\"\n", destination);
//
  //return 0;

  char *fai_file=malloc(5000);
  char *sam_file=malloc(5000);
  char *unique_file= malloc(5000);
  char *non_unique_file=malloc(5000);
  int paired_end_mode = 1;
  int unique_mode = 1;
  int rum = 0;
  int strand = 0;
  int ucsc_header = 0;
  //int add_chr = 0;

  int c;
  int errflg = 0;
  char *prefix;
  prefix = "";
  while ((c = getopt(argc, argv, "e:uvhrs:p:")) != -1) {
    switch(c) {
    case 'h':
      errflg++;
      break;
    case 'v':
      fprintf(stderr, "%s\n", VERSION);
      exit(0);
      break;
    case 'p':
      prefix = optarg;
      break;
    case 'r':
      //fprintf(stderr, "%d\n", c);
      rum = 1;
      break;
    case 'u':
      //fprintf(stderr, "%d\n", c);
      ucsc_header = 1;
      break;
    case 'e':
      //fprintf(stderr, "%d\n", c);
      paired_end_mode = atoi(optarg);
      break;
    //case 'c':
    //  fprintf(stderr, "%d\n", c);
    //  add_chr = atoi(optarg);
    //  break;
    case 's':
      strand = atoi(optarg);
      break;
    case ':':       /* -f or -o without operand */
      fprintf(stderr, "Option -%c requires an operand\n", optopt);
      errflg++;
      break;
    case '?':
      fprintf(stderr, "Unrecognized option: '-%c'\n", optopt);
      errflg++;
      break;
    }
  }
  if (errflg) {
    usage();
    exit(2);
  }
  int i = 0;
  for ( ; optind < argc; optind++) {
    //printf("%d\n",optind );
    //printf("%s\n", argv[optind] );
    if (access(argv[optind], R_OK)==0) {
      //printf("HALLO %s\n", argv[optind] );
      if (i == 0) strcpy(fai_file,argv[optind]);
      if (i == 1) strcpy(sam_file,argv[optind]);
      i++;
    }
  }
  if (i != 2) usage();
  //printf("Used that many ar%d\n", argc);
  if (prefix) {
    strcpy(unique_file, prefix);
    strcat(unique_file,"Unique.cov");
    strcpy(non_unique_file, prefix);
    strcat(non_unique_file,"NU.cov");
  } else {
    strcpy(unique_file,"Unique.cov");
    strcpy(non_unique_file, "NU.cov");
  }
  log_info("JOB_SETTINGS:");
  log_info("fai_file:\t%s",fai_file);
  log_info("sam_file:\t%s",sam_file);
  log_info("unique_file:\t%s",unique_file);
  log_info("non_unique_file:\t%s",non_unique_file);
  log_info("strand:\t%d",strand);
  log_info("aligned with RUM?:\t%d", rum);
  log_info("Print header?:\t%d", ucsc_header);
  log_info("Run in paired end mode?:\t%d", paired_end_mode);
  //log_info("Add chromosome for ensemble?:\t%d", add_chr);
  //unique_file = argv[3];
  //non_unique_file = argv[4];
  //rum = atoi(argv[5]);
  //strand = atoi(argv[6]);

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

  if (paired_end_mode == 1) {
    run_sam2cov(genome, unique_file, sam_file,
      num_of_chr, chromo_lengths, chromo_names, unique_mode, rum, strand, ucsc_header);
    Genome_reset(genome);
    run_sam2cov(genome, non_unique_file, sam_file,
      num_of_chr, chromo_lengths, chromo_names, 0, rum, strand, ucsc_header);
  } else {
    run_sam2cov_single_end(genome, unique_file, sam_file,
      num_of_chr, chromo_lengths, chromo_names, unique_mode, rum, strand, ucsc_header);
    Genome_reset(genome);
    run_sam2cov_single_end(genome, non_unique_file, sam_file,
      num_of_chr, chromo_lengths, chromo_names, 0, rum, strand, ucsc_header);
  }

  Genome_destroy(genome);
  for (int i=0; i<num_of_chr; i++) free(chromo_names[i]);

  free(unique_file); free(non_unique_file);
  free(fai_file); free(sam_file);
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
}