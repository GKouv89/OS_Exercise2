#ifndef LRUSTACK_H
#define LRUSTACK_H

typedef enum {bzip, gcc} trace;

typedef struct lruContent{
  unsigned int page_no;
  trace process;
}lruNodeContent;

typedef struct lruNode{
  lruNodeContent content;
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
lruNodeContent insert_page_in_stack(lruStack *, int, trace);
void printStack(lruStack *);
void bringPageUp(lruStack *, int, trace);
lruNodeContent victimize(lruStack *);
void delete_lruStackNode(lruStackNode **);
void destroy_lruStack(lruStack **);

#endif
