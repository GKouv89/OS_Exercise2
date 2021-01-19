#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "addressOps.h"
#include "hashedPageTable.h"
#include "lruStack.h"
#include "2ndchancequeue.h"

typedef struct stats{
  unsigned int still_in_memory;
  unsigned int reads;
  unsigned int writes;
  unsigned int writebacks; 
  unsigned int page_faults;
  unsigned int bzip_refs;
  unsigned int gcc_refs;
  unsigned int overall_refs;
}statistics;

void lru(int, int, int *, statistics *);
void secchance(int, int, int *, statistics *);


int main(int argc, char *argv[]){
  if(argc != 4 && argc != 5){
    printf("Correct usage: Algorithm memory_frames_count references_count (max_references_to_read)\n");
    printf("Algorithm is either LRU or 2ND\n");
    return 1;
  }
  char algorithm[3];
  strcpy(algorithm, argv[1]);
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
  
  statistics stats = {.still_in_memory = 0, .reads = 0, .writes = 0, .writebacks = 0, .page_faults = 0, .bzip_refs = 0, .gcc_refs = 0, .overall_refs = 0} ;
  if(strcmp("LRU", algorithm) == 0){
    lru(mem_fr_count, references_count, &max_refs, &stats);
  }else{
    secchance(mem_fr_count, references_count, &max_refs, &stats);
  }
  
  assert(stats.reads + stats.writes == stats.overall_refs);
  assert(stats.overall_refs == max_refs);
  assert(stats.gcc_refs + stats.bzip_refs == stats.overall_refs);

  printf("+---------STATISTICS---------+\n");
  printf("Pages still in memory at time of finishing: %u\n", stats.still_in_memory);
  printf("Releasing all memory...\n");
  printf("Writebacks (writes to hard drive): %u\n", stats.writebacks);
  printf("Page faults (reads from hard drive): %u\n", stats.page_faults);
  printf("References read from bzip: %u\n", stats.bzip_refs);
  printf("References read from gcc: %u\n", stats.gcc_refs);
  printf("Overall references read: %u\n", stats.overall_refs);
  printf("+----------------------------+\n");
  return 0;
}

void lru(int frames_count, int references_count, int *max_refs, statistics *stats){
  FILE *bzip_file = fopen("bzip.trace", "r");
  FILE *gcc_file = fopen("gcc.trace", "r");
  assert(bzip_file != NULL);
  assert(gcc_file != NULL);
  
  page_table *bzip_pt, *gcc_pt;
  create_page_table(&bzip_pt, BZIP_PROC_BUCKET_NO);
  create_page_table(&gcc_pt, GCC_PROC_BUCKET_NO);
  
  unsigned int address, page_no;
  char access_mode;
  int exists, dirty; 
  
  lruStack *ls;
  create_lrustack(&ls, frames_count);
  NodeContent victim;
  
  if(*max_refs == -1){
    // we read the files from start to finish
    *max_refs = 2000000; //no of lines in both files
  }
  // we read only a limited amount of references
  while(!feof(bzip_file) && !feof(gcc_file) && stats->overall_refs < *max_refs){
    for(int i = 0; i < references_count && !feof(bzip_file) && stats->overall_refs < *max_refs; i++){
      fscanf(bzip_file, "%x %c\n", &address, &access_mode);
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
        }
        stats->page_faults++;
      }else{
        // bringing up in stack
        bringPageUp(ls, page_no, bzip);
      }
      
      if(access_mode == 'W'){
        stats->writes++;
        set_dirty(bzip_pt, page_no);
      }else{
        stats->reads++;
      }
      stats->bzip_refs++;
      stats->overall_refs++;
    }
    for(int i = 0; i < references_count /* && !feof(gcc_file) */ && stats->overall_refs < *max_refs; i++){
      fscanf(gcc_file, "%x %c\n", &address, &access_mode);
      page_no = clip_offset(address);
      stats->gcc_refs++;
      stats->overall_refs++;
      
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
        }
        stats->page_faults++;
      }else{
        // bringing up in stack
        bringPageUp(ls, page_no, gcc);
      }
      
      if(access_mode == 'W'){
        stats->writes++;
        set_dirty(gcc_pt, page_no);
      }else{
        stats->reads++;
      }
    }
  }
  for(int i = 0; i < BZIP_PROC_BUCKET_NO; i++){
    stats->still_in_memory += bzip_pt->table[i].no_of_entries;
  }
  for(int i = 0; i < GCC_PROC_BUCKET_NO; i++){
    stats->still_in_memory += gcc_pt->table[i].no_of_entries;
  }
  stats->writebacks += destroy_page_table(&bzip_pt);
  stats->writebacks += destroy_page_table(&gcc_pt);
  destroy_lruStack(&ls);
  assert(fclose(bzip_file) == 0);
  assert(fclose(gcc_file) == 0);
}

void secchance(int frames_count, int references_count, int *max_refs, statistics *stats){
  FILE *bzip_file = fopen("bzip.trace", "r");
  FILE *gcc_file = fopen("gcc.trace", "r");
  assert(bzip_file != NULL);
  assert(gcc_file != NULL);
  
  page_table *bzip_pt, *gcc_pt;
  create_page_table(&bzip_pt, BZIP_PROC_BUCKET_NO);
  create_page_table(&gcc_pt, GCC_PROC_BUCKET_NO);
  
  unsigned int address, page_no;
  char access_mode;
  int exists, dirty; 
  
  secChanceQueue *q;
  create_secChanceQueue(&q, frames_count);
  NodeContent victim;
  
  if(*max_refs == -1){
    // we read the files from start to finish
    *max_refs = 2000000; //no of lines in both files
  }
  // we read only a limited amount of references
  while(!feof(bzip_file) && !feof(gcc_file) && stats->overall_refs < *max_refs){
    for(int i = 0; i < references_count && !feof(bzip_file) && stats->overall_refs < *max_refs; i++){
      fscanf(bzip_file, "%x %c\n", &address, &access_mode);
      page_no = clip_offset(address);
      
      exists = insert_page(bzip_pt, page_no);
      if(!exists){
        // inserting in stack
        victim = insert_page_in_queue(q, page_no, bzip);
        if(victim.page_no != -1){
          if(victim.process == bzip){
            remove_page_from_page_table(bzip_pt, victim.page_no, &dirty);
          }else{
            remove_page_from_page_table(gcc_pt, victim.page_no, &dirty);
          }
          
          if(dirty){
            stats->writebacks++;
          }
        }
        stats->page_faults++;
      }else{
        // bringing up in stack
        pageUsed(q, page_no, bzip);
      }
      
      if(access_mode == 'W'){
        stats->writes++;
        set_dirty(bzip_pt, page_no);
      }else{
        stats->reads++;
      }
      stats->bzip_refs++;
      stats->overall_refs++;
    }
    for(int i = 0; i < references_count && !feof(gcc_file) && stats->overall_refs < *max_refs; i++){
      fscanf(gcc_file, "%x %c\n", &address, &access_mode);
      page_no = clip_offset(address);
      stats->gcc_refs++;
      stats->overall_refs++;
      
      exists = insert_page(gcc_pt, page_no);
      if(!exists){
        // inserting in stack
        victim = insert_page_in_queue(q, page_no, gcc);
        if(victim.page_no != -1){
          if(victim.process == bzip){
            remove_page_from_page_table(bzip_pt, victim.page_no, &dirty);
          }else{
            remove_page_from_page_table(gcc_pt, victim.page_no, &dirty);
          }
          
          if(dirty){
            stats->writebacks++;
          }
        }
        stats->page_faults++;
      }else{
        // bringing up in stack
        pageUsed(q, page_no, gcc);
      }
      
      if(access_mode == 'W'){
        stats->writes++;
        set_dirty(gcc_pt, page_no);
      }else{
        stats->reads++;
      }
    }
  }
  for(int i = 0; i < BZIP_PROC_BUCKET_NO; i++){
    stats->still_in_memory += bzip_pt->table[i].no_of_entries;
  }
  for(int i = 0; i < GCC_PROC_BUCKET_NO; i++){
    stats->still_in_memory += gcc_pt->table[i].no_of_entries;
  }
  stats->writebacks += destroy_page_table(&bzip_pt);
  stats->writebacks += destroy_page_table(&gcc_pt);
  destroy_secChanceQueue(&q);
  assert(fclose(bzip_file) == 0);
  assert(fclose(gcc_file) == 0);
}
