#include "minunit.h"
#include <dlfcn.h>
#include "dbg.h"
//#include "../src/functions.h"

#define MAX_STRING_LENGTH 200

// int compare_two_files(char *file1, char *file2)
typedef int (*lib_function)(const char *file1, const char *file2);

char *lib_file = "build/libsam2cov.so";
void *lib = NULL;

int check_function(const char *func_to_run, const char *file1, const char *file2, int expected)
{
    lib_function func = dlsym(lib, func_to_run);
    check(func != NULL, "Did not find %s function in the library %s: %s", func_to_run, lib_file, dlerror());

    int rc = func(file1,file2);
    check(rc == expected, "Function %s return %d for file1: %s file2: %s", func_to_run, rc, file1, file2);

    return 1;
error:
    return 0;
}

typedef int (*lib_function3)(int input);

int check_function3(const char *func_to_run, int input, int expected)
{
    lib_function3 func = dlsym(lib, func_to_run);
    check(func != NULL, "Did not find %s function in the library %s: %s", func_to_run, lib_file, dlerror());

    int rc = func(input);
    check(rc == expected, "Function %s return %d for data: %d instead of %d", func_to_run, rc, input, expected);

    return 1;
error:
    return 0;
}

typedef char* (*lib_function2)(char *data);
int check_function2(const char *func_to_run, char *data, char *expected)
{
    lib_function2 func = dlsym(lib, func_to_run);

    check(func != NULL, "Did not find %s function in the library %s: %s", func_to_run, lib_file, dlerror());
    //printf("%s\n", expected);
    char* rc = func(data);
    check(strcmp(rc,expected) == 0, "Function %s return %s for data: %s, instead of %s", func_to_run, rc, data, expected);
    free(rc);
    return 1;
error:
    return 0;
}

char *test_dlopen()
{
    lib = dlopen(lib_file, RTLD_NOW);
    mu_assert(lib != NULL, "Failed to open the library to test.");

    return NULL;
}

char *test_dlclose()
{
    int rc = dlclose(lib);
    mu_assert(rc == 0, "Failed to close lib.");
    int status = system("rm tests/*tmp*");
    mu_assert(status == 0, "Remove tmp files was unsuccesful.");
    return NULL;
}

char *test_everything()
{
    //char *fai_file = "tests/danRer7_s.fa.fai";
    //int num_of_chr = 19;
    //char *sam_file="tests/test.sam";
    //char *unique_file="tests/tmp_unique";
    //char *non_unique_file="tests/tmp_NU";
    //int paired_end_mode = 1;
    //int unique_mode = 1;
    //int rum = 0;
    //int strand = 2;
    //int ucsc_header = 0;
    int status = system("command -v valgrind >/dev/null 2>&1 && VALGRIND='valgrind -v --leak-check=full'");
    //printf("Satus %d\n", status);
    status = system("$VALGRIND ./bin/sam2cov  -p tests/both2_tmp_ -s 0  -e 1 tests/danRer7_s.fa.fai tests/test.sam");
    //printf("Satus %d\n", status);
    //check(status == 0, "Function sam2cov return %d", status);
    mu_assert(status == 0,"test_everything failed!");

    //strcpy(msg,"");
    //snprintf(msg, "compare_two_files failed of %s and %s!", file1, file2);
    //mu_assert(status == 1,"compare_two_files failed!");
    //run_sam2cov(Genome *genome, char *out_file,
    //    char *sam_file, int num_of_chr, int *chromo_lengths,
    //    char **chromo_names, int unique_mode, int rum, int strand, int ucsc_header);
    return NULL;

}

char *test_output(){
    //char *file1 = ;
    //char *file2 = ;
    //status = compare_two_files(file1,file2);
    //char msg[500];
    //snprintf(msg, "compare_two_files failed of %s and %s!", file1, file2);
    mu_assert(check_function("compare_two_files", "tests/both2_tmp_NU.cov", "tests/both2NU.cov", 1), "test_output failed!");
    //file1 = "tests/both2_tmp_Unique.cov";
    //file2 = "tests/both2Unique.cov";
    mu_assert(check_function("compare_two_files", "tests/both2_tmp_Unique.cov", "tests/both2Unique.cov", 1), "test_output failed!");
    //free(file1); free(file2);
    return NULL;
}


char *test_everything_single()
{
    int status = system("command -v valgrind >/dev/null 2>&1 && VALGRIND='valgrind -v --leak-check=full'");
    status = system("$VALGRIND ./bin/sam2cov -p tests/sing_2_tmp_ -s 2 -e 0 tests/Pf3D7_genome_one-line-seqs.fa.fai tests/single_end.sam");
    status = system("$VALGRIND ./bin/sam2cov -p tests/sing_1_tmp_ -s 1 -e 0 tests/Pf3D7_genome_one-line-seqs.fa.fai tests/single_end.sam");
    mu_assert(status == 0,"test_everything failed!");
    return NULL;

}

char *test_output_single(){
    mu_assert(check_function("compare_two_files", "tests/sing_2_tmp_NU.cov", "tests/sing_2NU.cov", 1), "test_output failed!");
    mu_assert(check_function("compare_two_files", "tests/sing_2_tmp_Unique.cov", "tests/sing_2Unique.cov", 1), "test_output failed!");
    mu_assert(check_function("compare_two_files", "tests/sing_1_tmp_NU.cov", "tests/sing_1NU.cov", 1), "test_output failed!");
    mu_assert(check_function("compare_two_files", "tests/sing_1_tmp_Unique.cov", "tests/sing_1Unique.cov", 1), "test_output failed!");
    return NULL;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_dlopen);
    mu_run_test(test_everything);
    mu_run_test(test_output);
    mu_run_test(test_everything_single);
    mu_run_test(test_output_single);
    //mu_run_test(test_functions);
    //mu_run_test(test_chromosome)
    //mu_run_test(test_dlclose);
    return NULL;
}

RUN_TESTS(all_tests);