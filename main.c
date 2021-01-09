#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "addressOps.h"
#include "hashedPageTable.h"
#include "lruStack.h"

typedef struct stats{
  unsigned int reads;
  unsigned int writes;
  unsigned int writebacks; 
  unsigned int page_faults;
  unsigned int bzip_refs;
  unsigned int gcc_refs;
  unsigned int overall_refs;
}statistics;

void lru(int, int, int, statistics *);


int main(int argc, char *argv[]){
  if(argc != 4 && argc != 5){
    printf("Correct usage: Algorithm memory_frames_count references_count (max_references_to_read)\n");
    printf("Algorithm is either LRU or 2ND\n");
    return 1;
  }
  char algorithm[3];
  strcpy(algorithm, argv[1]);
  // if(strcmp("LRU", algorithm) == 0){
    // printf("LRU\n");
  // }else if(strcmp("2ND", algorithm) == 0){
    // printf("2nd chance\n");
  // }else{
  if(strcmp("LRU", algorithm) != 0 && strcmp("2ND", algorithm) != 0){
    printf("Invalid algorithm\n");
    return 1;
  }
  int mem_fr_count = atoi(argv[2]);
  int references_count = atoi(argv[3]);
  int max_refs;
  if(argc == 5){
    max_refs = atoi(argv[4]);
  }else{
    max_refs = -1;
  }
  
  statistics stats;
  if(strcmp("LRU", algorithm) == 0){
    lru(int mem_fr_count, references_count, max_refs, &stats);
  }
  
  printf("+---------STATISTICS---------+\n");
  printf("Reads: %u\n", stats.reads);
  printf("Writes: %u\n", stats.writes);
  printf("Writebacks: %u\n", stats.writebacks);
  printf("Page faults: %u\n", stats.page_faults);
  printf("References read from bzip: %u\n", stats.bzip_refs);
  printf("References read from gcc: %u\n", stats.gcc_refs);
  printf("Overall references read: %u\n", stats.overall_refs);
  printf("+----------------------------+\n");
  return 0;
}

void lru(int frames_count, int references_count, int max_refs, statistics *stats){
  FILE *bzip = fopen("bzip.trace", "r");
  FILE *gcc = fopen("gcc.trace", "r");
  assert(bzip != NULL);
  assert(gcc != NULL);
  
  page_table *bzip_pt, *gcc_pt;
  create_page_table(&bzip_pt, BZIP_PROC_BUCKETS);
  create_page_table(&gcc_pt, GCC_PROC_BUCKETS);
  
  unsigned int address, page_no;
  char reading_mode;
  int exists, dirty; 
  
  lruStack *ls;
  create_lrustack(&ls, frames_count);
  lruNodeContent victim;
  
  if(max_refs == -1){
    // we read the files from start to finish
    max_refs = 2000000; //no of lines in both files
  }
  // we read only a limited amount of references
  while(!feof(bzip) && !feof(gcc) && stats->overall_refs < max_refs){
    for(int i = 0; i < references_count && !feof(bzip) && stats->overall_refs < max_refs; i++){
      fscanf(bzip, "%x %c\n", &address, &access_mode);
      
      if(access_mode == 'W'){
        stats->writes++;
      }else{
        stats->reads++;
      }
      stats->refs++;
      stats->bzip_refs++;
      stats->overall_refs++;
      
      page_no = clip_offset(address);
      exists = insert_page(bzip_pt, page_no);
      if(!exists){
        // inserting in stack
        victim = insert_page_in_stack(ls, page_no, bzip);
        if(victim.page_no != -1){
          
          if(victim.process == bzip){
            remove_page_from_page_table(bzip_pt, victim.page_no, &dirty);
          }else{
            remove_page_from_page_table(gcc_pt, victim.page_no, &dirty);
          }
          
          if(dirty){
            stats->writebacks++;
          }
          stats->page_faults++;
        }
      }else{
        // bringing up in stack
        bringPageUp(lruStack *, page_no, bzip);
      }
    }
    for(int i = 0; i < references_count && !feof(gcc) && stats->overall_refs < max_refs; i++){
      fscanf(gcc, "%x %c\n", &address, &access_mode);
      
      if(access_mode == 'W'){
        stats->writes++;
      }else{
        stats->reads++;
      }
      stats->refs++;
      stats->gcc_refs;
      stats->overall_refs++;
      
      page_no = clip_offset(address);
      exists = insert_page(gcc_pt, page_no);
      if(!exists){
        // inserting in stack
        victim = insert_page_in_stack(ls, page_no, gcc);
        if(victim.page_no != -1){
          if(victim.process == bzip){
            remove_page_from_page_table(bzip_pt, victim.page_no, &dirty);
          }else{
            remove_page_from_page_table(gcc_pt, victim.page_no, &dirty);
          }
          
          if(dirty){
            stats->writebacks++;
          }
          
          stats->page_faults++;
        }
      }else{
        // bringing up in stack
        bringPageUp(lruStack *, page_no, gcc);
      }
    }
  }
}