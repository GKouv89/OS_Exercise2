#ifndef HASHEDPAGETABLE_H
#define HASHEDPAGETABLE_H

#include "overflowList.h"

#define GCC_PROC_BUCKET_NO 17
#define BZIP_PROC_BUCKET_NO 17 

typedef struct hash_bucket{
  int no_of_entries; // this is mostly necessary for evaluation of hash function
  oflist* bucket_list;
}bucket;

typedef struct hash_table{
  int no_of_buckets;
  bucket *table;
}page_table;

void create_page_table(page_table **, int);
int insert_page(page_table *, unsigned int);
void set_dirty(page_table *, unsigned int);
void remove_page_from_page_table(page_table *, unsigned int, int *);
int hash_val(int, unsigned int);
void destroy_page_table(page_table **);

#endif