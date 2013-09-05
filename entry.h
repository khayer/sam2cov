#ifndef _entry_h
#define _entry_h

/*
// makes THE_SIZE in ex22.c available to other .c files
extern int THE_SIZE;

// gets and sets an internal static variable in ex22.c
int get_age();
void set_age(int age);

// updates a static variable that's inside update_ratio
double update_ratio(double ratio);

void print_size();
*/
struct EntryStruct;

typedef struct EntryStruct
{
  char *read_name;
  int strand;
  char *chr_name;
  int pos;
  int chr_num;
  char *cigar_string;
} Entry;

Entry *Entry_create(char *read_name, int strand, char *chr_name, int pos, int chr_num, char *cigar);
void Entry_destroy(Entry *entry);

#endif