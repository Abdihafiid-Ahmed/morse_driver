#ifndef MORSE_THREAD_H
#define MORSE_THREAD_H

#include "morse_buffer.h"
//thread start 
int morse_thread_start(struct morse_buffer *buff);
///thread stop 
void morse_thread_stop(void);

//duration of ledlight blink
void morse_thread_unit(int ms);

#endif
