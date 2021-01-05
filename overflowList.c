#include <stdio.h>
#include "overflowList.h"

void list_create(oflist** list){
    (*list) = malloc(sizeof(of_list));
    (*list)->front = NULL;
    (*list)->rear = NULL;
}

oflist_node* create_node(int page_no){
    oflist_node* new_node = malloc(sizeof(oflist_node));
    new_node->page_no = page_no;
    new_node->dirty = 0;
    new_node->next = new_node->prev = NULL;
    return new_node;
}

void new_page(oflist* list, int page_no){
    oflist_node *new_node = create_node(page_no);
    if(list->front == NULL){ // empty list
        list->front = new_node;
        list->rear = new_node;
    }else{
        node->prev = list->rear;
        list->rear->next = node;
        list->rear = node;
    }
}

void remove_page(oflist* list, int page_no){
  oflist_node *temp = list->front;
  while(temp){
    if(temp->page_no == page_no){
      if(temp == list->front && temp == list->rear){
        list->front == list->rear == NULL;
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
      delete_node(&temp);
    }
  }
}

void delete_node(oflist_node** node){
    free(*node);
    *node = NULL;
}

void destroy_list(oflist** list){
    oflist_node* temp = (*list)->rear;
    if(temp != NULL && (*list)->front != NULL){
        oflist_node* prev = NULL;
        while(temp){
            prev = temp->previous;
            if(prev){ // Otherwise, we've reached list front
                prev->next = NULL;
            }
            delete_node(&temp);
            temp = prev;
        }            
    }
    (*list)->front = NULL;
    (*list)->rear = NULL;
    free(*list);
    *list = NULL;
}

