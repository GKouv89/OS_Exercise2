#include "../acutest.h"
#include "../addressOps.h"
#include "../hashedPageTable.h"
#include "../overflowList.h"

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

TEST_LIST = {
  {"offset_clipping", test_offset_clipping},
  {"file_scanning", test_file_scanning},
  {"diff_pages_count", test_diff_pages_count},
  {"page_table_create_delete", test_page_table_create_delete},
  {"page_table_no_of_entries", test_no_of_entries_per_bucket},
  {"overflow_list_search", test_list_search},
  {NULL, NULL}
};