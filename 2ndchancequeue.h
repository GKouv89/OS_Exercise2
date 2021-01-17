#ifndef secChanceQueue_H
#define secChanceQueue_H

#include "content.h"

typedef struct secchancenode{
  NodeContent content;
  int recently_used; // 0 if not used ever since (re)entering the queue, 1 if it has been
  struct secchancenode *prev;
  struct secchancenode *next;
}secChanceNode;

typedef struct secchance {
  int no_of_frames;
  int currSize;
  secChanceNode *head; // the node we check to see whether it gets a second chance
  secChanceNode *rear; // where we place new nodes and second chance nodes
}secChanceQueue;


void create_secChanceQueue(secChanceQueue **, int);
secChanceNode* create_secChanceNode(int, trace);
NodeContent insert_page_in_queue(secChanceQueue *, int, trace);
void pageUsed(secChanceQueue *, int, trace);
NodeContent victimize_2nd_chance(secChanceQueue *);
void delete_secChanceNode(secChanceNode **);
void destroy_secChanceQueue(secChanceQueue **);

#endif
