#include "../acutest.h"
#include "../addressOps.h"

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

TEST_LIST = {
  {"offset_clipping", test_offset_clipping},
  {"file_scanning", test_file_scanning},
  {"diff_pages_count", test_diff_pages_count},
  {NULL, NULL}
};