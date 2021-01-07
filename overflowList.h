#ifndef OVERFLOWLIST_H
#define OVERFLOWLIST_H

typedef struct overflowlistnode{
  unsigned int page_no;
  int dirty;
  struct overflowlistnode *next;
  struct overflowlistnode *prev;
} oflist_node;

typedef struct overflowlist{
  oflist_node *front;
  oflist_node *rear;
}oflist;

void list_create(oflist**);
oflist_node* create_node(unsigned int);
void new_page(oflist*, unsigned int);
oflist_node* searchForPage(oflist *, unsigned int);
void delete_node(oflist_node**);
void destroy_list(oflist** list);

#endif