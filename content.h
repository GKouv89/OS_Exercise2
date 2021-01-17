#ifndef CONTENT_H
#define CONTENT_H

typedef enum {bzip, gcc} trace;

typedef struct content{
  unsigned int page_no;
  trace process;
}NodeContent;

#endif
