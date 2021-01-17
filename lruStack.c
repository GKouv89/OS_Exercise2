#include <stdio.h>
#include <stdlib.h>
#include "lruStack.h"

void create_lrustack(lruStack **ls, int no_of_frames){
  (*ls) = malloc(sizeof(lruStack));
  (*ls)->no_of_frames = no_of_frames;
  (*ls)->currSize = 0;
  (*ls)->top = NULL;
  (*ls)->bottom = NULL;
}

lruStackNode* create_lruStackNode(int page_no, trace process){
  lruStackNode *new_node = malloc(sizeof(lruStackNode));
  new_node->content.page_no = page_no;
  new_node->content.process = process;
  new_node->above = NULL;
  new_node->below = NULL;
  return new_node;
}

NodeContent insert_page_in_stack(lruStack *ls, int page_no, trace process){
  lruStackNode *new_node = create_lruStackNode(page_no, process);
  if(ls->currSize == 0){
    // empty stack
    ls->top = new_node;
    ls->bottom = new_node;
    ls->currSize++;
  }else{
    ls->top->above = new_node;
    new_node->below = ls->top;
    ls->top = new_node;
    ls->currSize++;
  }
  NodeContent victim;
  if(ls->currSize == ls->no_of_frames + 1){
    victim = victimize(ls);
    ls->currSize--;
  }else{
    victim.page_no = -1;
  }
  return victim;
}

void printStack(lruStack *ls){
  lruStackNode *temp = ls->top;
  printf("\n");
  while(temp){
    printf("%u from process %d\n", temp->content.page_no, temp->content.process);
    temp = temp->below;
  }
}

void bringPageUp(lruStack *ls, int page_no, trace process){
  lruStackNode *temp = ls->top;
  while(temp){
    if(temp->content.page_no == page_no && temp->content.process == process){
      if(temp == ls->top){
        // do nothing, already at the top
      }else{
        temp->above->below = temp->below;
        if(temp->below){
          temp->below->above = temp->above;
        }else{
          ls->bottom = temp->above;
        }
        temp->above = NULL;
        temp->below = ls->top;
        ls->top->above = temp;
        ls->top = temp;
        return;
      }
    }
    temp = temp->below;
  }
}

NodeContent victimize(lruStack *ls){
  lruStackNode *to_victimize = ls->bottom;
  NodeContent victim = to_victimize->content;
  to_victimize->above->below = NULL;
  ls->bottom = to_victimize->above;
  delete_lruStackNode(&to_victimize);
  return victim;
}

void delete_lruStackNode(lruStackNode **node){
  free(*node);
  *node = NULL;
}

void destroy_lruStack(lruStack **ls){
  lruStackNode* temp = (*ls)->bottom;
  if(temp != NULL && (*ls)->top != NULL){
    lruStackNode* prev = NULL;
    while(temp){
      prev = temp->above;
      if(prev){ // Otherwise, we've reached list front
        prev->below = NULL;
      }
      delete_lruStackNode(&temp);
      temp = prev;
    }            
  }
  (*ls)->top = NULL;
  (*ls)->bottom = NULL;
  free(*ls);
  *ls = NULL;
}