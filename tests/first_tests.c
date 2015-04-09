#include "minunit.h"
#include <dlfcn.h>
//#include "../src/functions.h"

#define MAX_STRING_LENGTH 200

typedef int (*lib_function)(const char *data);

char *lib_file = "build/libsam2cov.so";
void *lib = NULL;

int check_function(const char *func_to_run, const char *data, int expected)
{
    lib_function func = dlsym(lib, func_to_run);
    check(func != NULL, "Did not find %s function in the library %s: %s", func_to_run, lib_file, dlerror());

    int rc = func(data);
    check(rc == expected, "Function %s return %d for data: %s", func_to_run, rc, data);

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

char *test_functions()
{
    //int k = get_strand(83);
    //char *filename = "danRer7_s.fa.fai";
    //mu_assert(k == 1, "error, k != 1" );
    mu_assert(check_function3("get_strand", 83, 1), "get_strand failed.");
    mu_assert(check_function2("uppercase", "Hello", "HELLO"), "uppercase failed.");
    mu_assert(check_function2("lowercase", "Hello", "hello"), "lowercase failed.");

    return NULL;
}

char *test_failures()
{
    mu_assert(check_function("fail_on_purpose", "Hello", 1), "fail_on_purpose should fail.");

    return NULL;
}

char *test_dlclose()
{
    int rc = dlclose(lib);
    mu_assert(rc == 0, "Failed to close lib.");

    return NULL;
}

char *test_chromosome()
{
    char *fai_file = "tests/danRer7_s.fa.fai";
    mu_assert(check_function("number_of_chromosomes", fai_file, 19), "number_of_chromosomes failed");
    int num_of_chr = 19;
    int chromo_lengths[num_of_chr];
    char* chromo_names[num_of_chr];
    for (int i=0; i<num_of_chr; ++i)
        chromo_names[i] = malloc(MAX_STRING_LENGTH);
    typedef void (*lib_function)(char *file_name, int number_of_chromosomes, int *chromo_lengths,char **names);
    lib_function func = dlsym(lib, "get_names");
    check(func != NULL, "Did not find %s function in the library %s: %s", "get_names", lib_file, dlerror());

    func(fai_file, num_of_chr,chromo_lengths,chromo_names);
    check(chromo_names != NULL, "Function %s return %s for data: %s", "get_names", chromo_names[1], fai_file);
    mu_assert(strcmp(chromo_names[1],"Zv9_scaffold3454")==0, "get_names failed!");
    return NULL;
error:
    return 0;

}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_dlopen);
    mu_run_test(test_functions);
    mu_run_test(test_failures);
    mu_run_test(test_chromosome)
    mu_run_test(test_dlclose);
    return NULL;
}

RUN_TESTS(all_tests);