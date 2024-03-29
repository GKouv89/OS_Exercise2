#include "../acutest.h"
#include "../addressOps.h"
#include "../hashedPageTable.h"
#include "../overflowList.h"
#include "../lruStack.h"
#include "../2ndchancequeue.h"

void test_offset_clipping(){
  unsigned int address = 129123232;
  unsigned int page_number = clip_offset(address);
  TEST_ASSERT(page_number == 31524);
}

void test_file_scanning(){
  FILE *fp = fopen("bzip.trace", "r");
  TEST_ASSERT(fp != NULL);
  unsigned int address;
  char access_mode;
  fscanf(fp, "%x %c\n", &address, &access_mode);
  TEST_ASSERT(address == 129123232);
  TEST_ASSERT(access_mode == 'R');
  fscanf(fp, "%x %c\n", &address, &access_mode);  
  TEST_ASSERT(address == 2264384);
  TEST_ASSERT(access_mode == 'R');
  TEST_ASSERT(fclose(fp) == 0);
}

void test_diff_pages_recognization(){
  unsigned int address0 = 129123232;
  unsigned int address1 = 129123264;
  unsigned int pageno0 = clip_offset(address0);
  unsigned int pageno1 = clip_offset(address1);
  TEST_ASSERT(pageno0 == pageno1);
  unsigned int address2 = 2264384;
  unsigned int pageno2 = clip_offset(address2);
  TEST_ASSERT(pageno2 != pageno0);
  TEST_ASSERT(pageno2 != pageno1);
}

void test_diff_pages_count(){
  unsigned int page_numbers[1000000];
  FILE *fp = fopen("bzip.trace", "r");
  TEST_ASSERT(fp != NULL);
  unsigned int page_no, address, line_counter, page_counter, page_exists;
  char access_mode;
  while(!feof(fp)){
    fscanf(fp, "%x %c\n", &address, &access_mode);
    TEST_ASSERT(address > 0);
    TEST_ASSERT(access_mode == 'R' || access_mode == 'W');
    page_no = clip_offset(address);
    for(int i = 0; i < page_counter; i++){
      if(page_no == page_numbers[i]){
        page_exists = 1;
        break;
      }
    }
    if(!page_exists){
      page_numbers[page_counter] = page_no;
      page_counter++;
    }else{
      page_exists = 0;
    }
    line_counter++;
  }
  printf("\nThere are %d different pages in bzip.trace\n", page_counter);
  TEST_ASSERT(line_counter == 1000000);
  TEST_ASSERT(fclose(fp) == 0);
  fp = fopen("gcc.trace", "r");
  TEST_ASSERT(fp != NULL);
  for(int i = 0; i < page_counter; i++){
    page_numbers[i] = 0;
  }
  page_counter = 0;
  line_counter = 0;
  page_exists = 0;
  while(!feof(fp)){
    fscanf(fp, "%x %c\n", &address, &access_mode);
    TEST_ASSERT(address > 0);
    TEST_ASSERT(access_mode == 'R' || access_mode == 'W');
    page_no = clip_offset(address);
    for(int i = 0; i < page_counter; i++){
      if(page_no == page_numbers[i]){
        page_exists = 1;
        break;
      }
    }
    if(!page_exists){
      page_numbers[page_counter] = page_no;
      page_counter++;
    }else{
      page_exists = 0;
    }
    line_counter++;
  }
  printf("\nThere are %d different pages in gcc.trace\n", page_counter);
  TEST_ASSERT(fclose(fp) == 0);
  TEST_ASSERT(line_counter == 1000000);
}

void test_page_table_create_delete(){
  page_table *bzip_pt;
  create_page_table(&bzip_pt, BZIP_PROC_BUCKET_NO);
  TEST_ASSERT(bzip_pt->no_of_buckets == BZIP_PROC_BUCKET_NO);
  for(int i = 0; i < BZIP_PROC_BUCKET_NO; i++){
    TEST_ASSERT(bzip_pt->table[i].bucket_list->rear == NULL);
    TEST_ASSERT(bzip_pt->table[i].bucket_list->front == NULL);
  }
  destroy_page_table(&bzip_pt);
  TEST_ASSERT(bzip_pt == NULL);
}

void test_no_of_entries_per_bucket(){
  page_table *bzip_pt;
  create_page_table(&bzip_pt, BZIP_PROC_BUCKET_NO);
  FILE *fp = fopen("bzip.trace", "r");
  TEST_ASSERT(fp != NULL);
  unsigned int address, page_no;
  char access_mode;
  
  while(!feof(fp)){
    fscanf(fp, "%x %c\n", &address, &access_mode);
    page_no = clip_offset(address);
    insert_page(bzip_pt, page_no);
  }
  
  int all_no_of_entries = 0;
  for(int i = 0; i < BZIP_PROC_BUCKET_NO; i++){
    all_no_of_entries += bzip_pt->table[i].no_of_entries;
    printf("%d entries in bucket no. %d\n", bzip_pt->table[i].no_of_entries, i);
  }
  TEST_CHECK(all_no_of_entries == 317);
  printf("Avg no of entries: %d\n", all_no_of_entries/BZIP_PROC_BUCKET_NO);
  destroy_page_table(&bzip_pt);
  TEST_ASSERT(fclose(fp) == 0);
  
  fp = fopen("gcc.trace", "r");
  TEST_ASSERT(fp != NULL);
  
  page_table *gcc_pt;
  create_page_table(&gcc_pt, GCC_PROC_BUCKET_NO);
  while(!feof(fp)){
    fscanf(fp, "%x %c\n", &address, &access_mode);
    page_no = clip_offset(address);
    insert_page(gcc_pt, page_no);
  }
  
  all_no_of_entries = 0;
  for(int i = 0; i < GCC_PROC_BUCKET_NO; i++){
    all_no_of_entries += gcc_pt->table[i].no_of_entries;
    printf("%d entries in bucket no. %d\n", gcc_pt->table[i].no_of_entries, i);
  }
  TEST_CHECK(all_no_of_entries == 2852);
  printf("Avg no of entries: %d\n", all_no_of_entries/GCC_PROC_BUCKET_NO);
  destroy_page_table(&gcc_pt);
  TEST_ASSERT(fclose(fp) == 0);
}

void test_list_search(){
  oflist *list;
  list_create(&list);
  oflist_node *finder = searchForPage(list, 0);
  TEST_ASSERT(finder == NULL);
  new_page(list, 0);
  finder = searchForPage(list, 1);
  TEST_ASSERT(finder == NULL);
  new_page(list, 1);
  finder = searchForPage(list, 2);
  TEST_ASSERT(finder == NULL);
  new_page(list, 2);
  finder = searchForPage(list, 0);
  TEST_ASSERT(finder != NULL);
  TEST_ASSERT(finder->dirty == 0);
  finder = searchForPage(list, 1);
  TEST_ASSERT(finder != NULL);
  TEST_ASSERT(finder->dirty == 0);
  finder = searchForPage(list, 2);
  TEST_ASSERT(finder != NULL);
  TEST_ASSERT(finder->dirty == 0);  
  destroy_list(&list);
}

void test_page_table_list_remove(){
  page_table *pt;
  create_page_table(&pt, BZIP_PROC_BUCKET_NO);
  
  int exists;
  exists = insert_page(pt, 0);
  TEST_ASSERT(exists == 0);
  exists = insert_page(pt, 1);
  TEST_ASSERT(exists == 0);
  exists = insert_page(pt, 2);
  TEST_ASSERT(exists == 0);

  exists = insert_page(pt, 1);
  TEST_ASSERT(exists == 1);
  
  set_dirty(pt, 2);
  
  int dirty;
  remove_page_from_page_table(pt, 1, &dirty);
  TEST_ASSERT(dirty == 0);
  remove_page_from_page_table(pt, 2, &dirty);
  TEST_ASSERT(dirty == 1);
  remove_page_from_page_table(pt, 0, &dirty);
  TEST_ASSERT(dirty == 0);
  
  destroy_page_table(&pt);
  TEST_ASSERT(pt == NULL);
}

void test_lruStack_functions(){
  lruStack *ls;
  int no_of_frames = 3;
  create_lrustack(&ls, no_of_frames);
  unsigned int page_0 = 17;
  trace page_0_belongs_to = bzip;
  unsigned int page_1 = 31; 
  trace page_1_belongs_to = gcc;
  unsigned int page_2 = 8;
  trace page_2_belongs_to = gcc;
  unsigned int page_3 = 321;
  trace page_3_belongs_to = bzip;
  NodeContent victim;
  
  victim = insert_page_in_stack(ls, page_0, page_0_belongs_to);
  TEST_ASSERT(ls->currSize == 1);
  TEST_ASSERT(victim.page_no == -1);
  victim = insert_page_in_stack(ls, page_1, page_1_belongs_to);
  TEST_ASSERT(ls->currSize == 2);
  TEST_ASSERT(victim.page_no == -1);
  victim = insert_page_in_stack(ls, page_2, page_2_belongs_to);
  TEST_ASSERT(ls->currSize == 3);
  TEST_ASSERT(victim.page_no == -1);
  
  bringPageUp(ls, page_1, page_1_belongs_to);
  TEST_ASSERT(ls->currSize == 3);
  TEST_ASSERT(ls->top->content.page_no == page_1);
  TEST_ASSERT(ls->top->content.process == page_1_belongs_to);
  
  bringPageUp(ls, page_1, page_1_belongs_to);
  TEST_ASSERT(ls->currSize == 3);
  TEST_ASSERT(ls->top->content.page_no == page_1);
  TEST_ASSERT(ls->top->content.process == page_1_belongs_to);
  
  bringPageUp(ls, page_0, page_0_belongs_to);
  TEST_ASSERT(ls->currSize == 3);
  TEST_ASSERT(ls->top->content.page_no == page_0);
  TEST_ASSERT(ls->top->content.process == page_0_belongs_to);
  
  
  victim = insert_page_in_stack(ls, page_3, page_3_belongs_to);
  TEST_ASSERT(ls->currSize == 3);
  TEST_ASSERT(victim.page_no == page_2);
  TEST_ASSERT(victim.process == page_2_belongs_to);
  
  
  destroy_lruStack(&ls);
}

struct reference{
  unsigned int page_no;
};

struct reference small_trace[] = {
  [0].page_no = 17,
  [1].page_no = 29,
  [2].page_no = 3
};

void test_second_chance_insertion(){
  // we go over the small trace
  // and each of them is referenced twice, like this
  // 17, 29, 3, 17, 29, 3
  // so the page that will be victimized when we attempt to insert a new page number
  // should be page no 17 (1st array element)
  // This should test insertion, multiple removal and reinsertion and therefore linking
  // and correct authentication of page to be inserted
  secChanceQueue *q;
  int frame_no = 3;
  create_secChanceQueue(&q, frame_no);
  NodeContent victim;
  for(int i = 0; i < 3; i++){
    victim  = insert_page_in_queue(q, small_trace[i].page_no, bzip);
    TEST_ASSERT(victim.page_no == -1);
  }
  for(int i = 0; i < 3; i++){
    pageUsed(q, small_trace[i].page_no, bzip);
  }
  victim  = insert_page_in_queue(q, 52, bzip);
  TEST_ASSERT(victim.page_no == 17);
  destroy_secChanceQueue(&q);
  TEST_ASSERT(q == NULL);
}

struct reference_action {
  unsigned int page_no;
  char action;
};

struct reference_action medium_trace[] = {
  [0].page_no = 1, [0].action = 'W',
  [1].page_no = 1, [1].action = 'W',
  [2].page_no = 2, [2].action = 'R',
  [3].page_no = 3, [3].action = 'R',
  [4].page_no = 2, [4].action = 'R',
  [5].page_no = 1, [5].action = 'W',
  [6].page_no = 4, [6].action = 'R',
  [7].page_no = 3, [7].action = 'R'
};

void test_second_chance_writebacks(){
  page_table *pt;
  create_page_table(&pt, BZIP_PROC_BUCKET_NO);
  
  int frames_count = 2;
  secChanceQueue *q;
  create_secChanceQueue(&q, frames_count);
  NodeContent victim;
  int exists, dirty; 
  
  unsigned int writes = 0, reads = 0, page_faults = 0, writebacks = 0;

  for(int i = 0; i < 8; i++){
    exists = insert_page(pt, medium_trace[i].page_no);
    if(!exists){
      // inserting in stack
      victim = insert_page_in_queue(q, medium_trace[i].page_no, bzip);
      if(victim.page_no != -1){
        remove_page_from_page_table(pt, victim.page_no, &dirty);
        switch(i){
          case 3: TEST_ASSERT(victim.page_no == 2);
                break;
          case 4: TEST_ASSERT(victim.page_no == 1);
                break;
          case 5: TEST_ASSERT(victim.page_no == 3); 
                break;
          case 6: TEST_ASSERT(victim.page_no == 2);
                break;
          case 7: TEST_ASSERT(victim.page_no == 1);
                break;
          default: printf("IN CASE %d, VICTIM IS %u\n", i, victim.page_no);
                break;
        }
        if(dirty){
          writebacks++;
        }
      }
      page_faults++;
    }else{
      // bringing up in stack
      pageUsed(q, medium_trace[i].page_no, bzip);
    }
      
    if(medium_trace[i].action == 'W'){
      writes++;
      set_dirty(pt, medium_trace[i].page_no);
    }else{
      reads++;
    }
  }
  TEST_CHECK(writes == 3);
  TEST_MSG("writes - expected = 3, actual = %u", writes);
  TEST_CHECK(reads == 5);
  TEST_MSG("reads - expected = 5, actual = %u", reads);
  TEST_CHECK(writebacks == 2);
  TEST_MSG("writebacks - expected = 2, actual = %u", writebacks);
  TEST_CHECK(page_faults == 7);
  TEST_MSG("PFs - expected = 7, actual = %u", page_faults);
  destroy_secChanceQueue(&q);
  destroy_page_table(&pt);
}

TEST_LIST = {
  {"offset_clipping", test_offset_clipping},
  {"file_scanning", test_file_scanning},
  {"diff_pages_count", test_diff_pages_count},
  {"diff_pages_recognization", test_diff_pages_recognization},
  {"page_table_create_delete", test_page_table_create_delete},
  {"page_table_no_of_entries", test_no_of_entries_per_bucket},
  {"page_table_remove_set_dirty", test_page_table_list_remove},
  {"overflow_list_search", test_list_search},
  {"lruStack_functions", test_lruStack_functions},
  {"second_chance_insertion", test_second_chance_insertion},
  {"second_chance_writebacks", test_second_chance_writebacks},
  {NULL, NULL}
};