#ifndef MORSE_BUFFER_H

#define MORSE_BUFFER_H

#define MORSE_BUFFER_MAX 1024 //maximum capacity  

struct morse_buffer{
  char  data[MORSE_BUFFER_MAX]; ///static data
  int   head; ///writing data
  int   tail; ////reading 
  int   count; ////currnet number of elements stored
  int   size; //// active capacity limit
};

void circlebuff_init(struct morse_buffer *buff, int size);

int circlebuff_resize(struct morse_buffer *buff, int new_size);


int circlebuff_full(struct morse_buffer *buff);
int circlebuff_empty(struct morse_buffer *buff);
int circlebuff_freespace(struct morse_buffer *buff);

int circlebuff_add(struct morse_buffer *buff, char c);
int circlebuff_remove(struct morse_buffer *buff, char *c);


#endif 

