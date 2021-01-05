#ifndef OVERFLOWLIST_H
#define OVERFLOWLIST_H

typedef struct overflowlistnode{
  int page_no;
  int dirty;
  overflowlistnode *next;
  overflowlistnode *prev;
} oflist_node;

typedef struct overflowlist{
  oflist_node *front;
  oflist_node *rear;
}oflist;

void list_create(oflist**);
oflist_node* create_node(int);
void new_page(oflist*, int);
void delete_node(oflist_node**);
void destroy_list(oflist** list);

#endif