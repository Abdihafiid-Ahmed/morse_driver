#include "morse_buffer.h"
#include <linux/string.h>
#include <linux/errno.h>


void circlebuff_init(struct morse_buffer *buff, int size){
  memset(buff->data, 0, MORSE_BUFFER_MAX);
  buff->head  = 0;
  buff->tail  = 0;
  buff->count = 0;
  buff->size  = size;

  if (buff->size > MORSE_BUFFER_MAX){
    buff->size = MORSE_BUFFER_MAX;
  else
    buff->size = size;
  }
  }

//resize if meet conditions
int circlebuff_resize(struct morse_buffer *buff, int new_size){
    if (new_size < 1 || new_size > MORSE_BUFFER_MAX)
      return -EINVAL;

    if (new_size < buff->count)
        return -EINVAL;
    buff->size = new_size;
    return 0;
  }

///////check if buffer has reached capacity
int circlebuff_full(struct morse_buffer *buff)
{
  return buff->count == buff->size;
}

///check if buffer is empty
int circlebuff_empty(struct morse_buffer *buff)
{
  return buff->count == 0;
}

//calculate reamaining slots  
int circlebuff_freespace(struct morse_buffer *buff)
{
  return buff->size - buf->count;

}


///////start of the circular buffer
int circlebuff_add(struct morse_buffer *buff, char c)
{
  if (circlebuff_full(buff))
      return -EAGAIN;

  buff->data[buff->tail] = c;

  /////core idea for the circular buffer
  buff->tail = (buff->tail + 1) % buff->size;
  buff->count++;
  return 0;

}
int circlebuff_remove(struct morse_buffer *buff, char *c){
  if (circlebuff_empty(buff))
      return -EAGAIN;

  *c = buff->data[buff->head];
  buff->head = (buff->head + 1) % buff->size;
  buff->count--;
  return 0;
}
 
