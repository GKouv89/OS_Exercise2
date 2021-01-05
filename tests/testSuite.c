#include "../acutest.h"
#include "../addressOps.h"

void test_offset_clipping(){
  int address = 129123232;
  int page_number = clip_offset(address);
  TEST_ASSERT(page_number == 31524);
}

void test_file_scanning(){
  FILE *fp = fopen("bzip.trace", "r");
  TEST_ASSERT(fp != NULL);
  int address;
  char access_mode;
  fscanf(fp, "%x %c\n", &address, &access_mode);
  TEST_ASSERT(address == 129123232);
  TEST_ASSERT(access_mode == 'R');
  fscanf(fp, "%x %c\n", &address, &access_mode);  
  TEST_ASSERT(address == 2264384);
  TEST_ASSERT(access_mode == 'R');
  TEST_ASSERT(fclose(fp) == 0);
}

TEST_LIST = {
  {"offset_clipping", test_offset_clipping},
  {"file_scanning", test_file_scanning},
  {NULL, NULL}
};