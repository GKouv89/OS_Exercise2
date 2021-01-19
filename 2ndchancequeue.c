#include <stdio.h>
#include <stdlib.h>
#include "2ndchancequeue.h"

void create_secChanceQueue(secChanceQueue **q, int no_of_frames){
  (*q) = malloc(sizeof(secChanceQueue));
  (*q)->no_of_frames = no_of_frames;
  (*q)->currSize = 0;
  (*q)->head = NULL;
  (*q)->rear = NULL;
}

secChanceNode* create_secChanceNode(int page_no, trace process){
  secChanceNode *new_node = malloc(sizeof(secChanceNode));
  new_node->content.page_no = page_no;
  new_node->content.process = process;
  new_node->recently_used = 0;
  new_node->prev = NULL;
  new_node->next = NULL;
  return new_node;
}

NodeContent insert_page_in_queue(secChanceQueue *q, int page_no, trace process){
  secChanceNode *new_node = create_secChanceNode(page_no, process);
  NodeContent victim;  
  if(q->currSize == 0){
    // empty stack
    q->head = new_node;
    q->rear = new_node;
    q->currSize++;
    victim.page_no = -1;
  }else if(q->currSize == q->no_of_frames){
    victim = victimize_2nd_chance(q);
    q->rear->prev = new_node;
    new_node->next = q->rear;
    q->rear = new_node;    
  }else{
    victim.page_no = -1;
    q->rear->prev = new_node;
    new_node->next = q->rear;
    q->rear = new_node;
    q->currSize++;
  }

  return victim;
}

void pageUsed(secChanceQueue *q, int page_no, trace process){
  secChanceNode *temp = q->rear;
  while(temp){
    if(temp->content.page_no == page_no && temp->content.process == process){
      temp->recently_used = 1;
      break;
    }
    temp = temp->next;
  }
}

NodeContent victimize_2nd_chance(secChanceQueue *q){
  secChanceNode *to_review; 
  while(1){
    to_review = q->head;
    if(to_review->recently_used == 1){
      // removing from head and reentering into queue
      to_review->prev->next = NULL;
      q->head = to_review->prev;
      to_review->prev = NULL;
      to_review->next = q->rear;
      q->rear->prev = to_review;
      q->rear = to_review;
      to_review->recently_used = 0;
    }else{
      // removing completely from queue
      NodeContent victim = to_review->content;
      to_review->prev->next = NULL;
      q->head = to_review->prev;
      delete_secChanceNode(&to_review);
      return victim;
    }
  }
}

void delete_secChanceNode(secChanceNode **node){
  free(*node);
  *node = NULL;
}

void destroy_secChanceQueue(secChanceQueue **q){
  secChanceNode* temp = (*q)->rear;
  secChanceNode *to_del;
  if(temp == NULL && (*q)->head == NULL){
    // empty queue
    free(*q);
    *q = NULL;
    return;
  }else{
    // contains at least one node
    while(temp){
      to_del = temp;
      temp = temp->next;
      delete_secChanceNode(&to_del);
    }
  }
  (*q)->head = NULL;
  (*q)->rear = NULL;
  free(*q);
  *q = NULL;
}