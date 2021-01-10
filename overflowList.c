#include <stdio.h>
#include <stdlib.h>
#include "overflowList.h"

void list_create(oflist** list){
    (*list) = malloc(sizeof(oflist));
    (*list)->front = NULL;
    (*list)->rear = NULL;
}

oflist_node* create_node(unsigned int page_no){
    oflist_node* new_node = malloc(sizeof(oflist_node));
    new_node->page_no = page_no;
    new_node->dirty = 0;
    new_node->next = new_node->prev = NULL;
    return new_node;
}

void new_page(oflist* list, unsigned int page_no){
    oflist_node *new_node = create_node(page_no);
    if(list->front == NULL){ // empty list
        list->front = new_node;
        list->rear = new_node;
    }else{
        new_node->prev = list->rear;
        list->rear->next = new_node;
        list->rear = new_node;
    }
}

oflist_node* searchForPage(oflist *list, unsigned int page_no){
  if(list == NULL){
    return NULL;
  }
  oflist_node *temp = list->front;
  while(temp != NULL){
    if(temp->page_no == page_no){
      // page exists
      return temp;
    }
    temp = temp->next;
  }
  return NULL;
}

void remove_page(oflist* list, unsigned int page_no, int *dirty){
  oflist_node *temp = list->front;
  while(temp){
    if(temp->page_no == page_no){
      if(temp == list->front && temp == list->rear){
        list->front = list->rear = NULL;
      }else if(temp == list->front){
        temp->next->prev = NULL;
        list->front = temp->next;
      }else if(temp == list->rear){
        temp->prev->next = NULL;
        list->rear = temp->prev;
      }else{
        temp->prev->next = temp->next;
        temp->next->prev = temp->prev;
      }
      *dirty = temp->dirty;
      delete_node(&temp);
      return;
    }
    temp = temp->next;
  }
}

// void print_oflist(oflist *list){
  // oflist_node *temp = list->front;
  // while(temp){
    // printf();
  // }
// }

void delete_node(oflist_node** node){
    free(*node);
    *node = NULL;
}

unsigned int destroy_list(oflist** list){
    unsigned int still_in_mem_dirty = 0;
    if(*list == NULL){
      return 0;
    }
    oflist_node* temp = (*list)->rear;
    if(temp != NULL && (*list)->front != NULL){
        oflist_node* prev = NULL;
        while(temp){
            prev = temp->prev;
            if(prev){ // Otherwise, we've reached list front
                prev->next = NULL;
            }
            if(temp->dirty){
              still_in_mem_dirty++;
            }
            delete_node(&temp);
            temp = prev;
        }            
    }
    (*list)->front = NULL;
    (*list)->rear = NULL;
    free(*list);
    *list = NULL;
    return still_in_mem_dirty;
}

