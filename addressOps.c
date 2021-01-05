#include <stdio.h>
#include "addressOps.h"

int clip_offset(int address){
  int page_number;
  int mask = 4294963200;
  page_number = address & mask;
  page_number = page_number >> 12;
  return page_number;
}