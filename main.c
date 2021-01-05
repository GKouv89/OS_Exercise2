#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){
  if(argc != 4 && argc != 5){
    printf("Correct usage: Algorithm memory_frames_count references_count (max_references_to_read)\n");
    printf("Algorithm is either LRU or 2ND\n");
    return 1;
  }
  char algorithm[3];
  strcpy(algorithm, argv[1]);
  if(strcmp("LRU", algorithm) == 0){
    printf("LRU\n");
  }else if(strcmp("2ND", algorithm) == 0){
    printf("2nd chance\n");
  }else{
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
  return 0;
}