#include <stdio.h>
#include <stdlib.h>
#include "hashedPageTable.h"

void create_page_table(page_table **pt, int no_of_buckets){
  (*pt) = malloc(sizeof(page_table));
  (*pt)->no_of_buckets = no_of_buckets;
  (*pt)->table = malloc(no_of_buckets*sizeof(bucket));
  for(int i = 0; i < no_of_buckets; i++){
    (*pt)->table[i].no_of_entries = 0;
    list_create(&((*pt)->table[i].bucket_list));
  }
}

int hash_function(int no_of_buckets, unsigned int page_no){
  return page_no % no_of_buckets; 
}

int insert_page(page_table *pt, unsigned int page_no){
  unsigned int hash_value = hash_function(pt->no_of_buckets, page_no);
  oflist_node *finder = searchForPage(pt->table[hash_value].bucket_list, page_no);
  if(finder == NULL){
    new_page(pt->table[hash_value].bucket_list, page_no);
    pt->table[hash_value].no_of_entries++;
    return 0;
  }else{
    return 1;
  }
}

void set_dirty(page_table *pt, unsigned int page_no){
  unsigned int hash_value = hash_function(pt->no_of_buckets, page_no);
  oflist_node *finder = searchForPage(pt->table[hash_value].bucket_list, page_no); 
  if(finder->dirty == 0){
    finder->dirty = 1;
  }
}

void remove_page_from_page_table(page_table *pt, unsigned int page_no, int *dirty){
  unsigned int hash_value = hash_function(pt->no_of_buckets, page_no);
  remove_page(pt->table[hash_value].bucket_list, page_no, dirty);
  pt->table[hash_value].no_of_entries--;
}

void destroy_page_table(page_table **pt){
  for(int i = 0; i < (*pt)->no_of_buckets; i++){
    destroy_list(&((*pt)->table[i].bucket_list));
  }
  free((*pt)->table);
  free(*pt);
  *pt = NULL;
}