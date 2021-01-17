#ifndef LRUSTACK_H
#define LRUSTACK_H

#include "content.h"

typedef struct lruNode{
  NodeContent content;
  struct lruNode *above;
  struct lruNode *below;
}lruStackNode;

typedef struct lru {
  int no_of_frames;
  int currSize;
  lruStackNode *top;
  lruStackNode *bottom;
}lruStack;

void create_lrustack(lruStack **, int);
lruStackNode* create_lruStackNode(int, trace);
NodeContent insert_page_in_stack(lruStack *, int, trace);
void printStack(lruStack *);
void bringPageUp(lruStack *, int, trace);
NodeContent victimize(lruStack *);
void delete_lruStackNode(lruStackNode **);
void destroy_lruStack(lruStack **);

#endif
