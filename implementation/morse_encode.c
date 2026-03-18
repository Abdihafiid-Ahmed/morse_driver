#include <stdio.h>

#include "morse_encode.h"

static const char *morse_letters[26] = {
  //A     b       c       d     e
  ".-", "-...", "-.-.", "-..", ".", 
// f        g    h         i      j       k  
  "..-.", "--.", "....",  "..", ".---", "-.-",
//L        m     n      o      p       q
  ".-..", "--", "-.", "---", ".--.", "--.-", 
//r       s      t     u      v 
  ".-.", "...", "-", "..-", "...-", 
// w       x       y      z   
  ".--", "-..-", "-.--", "--.."

};


static const char *morse_numbers[10] = {
  // 0         1        2         3      4 
    "----", ".----", "..---", "...--", "....-
//   5         6         7        8       9 
    ".....", "-....", "--...", "---..", "----."
  };

const char *morse_get_symbol(char c)

{
  if (c >= 'a' && c <= 'z') //small letters

    return morse_letters[c - 'a'];


  if (c >= 'A' && c <= 'Z') //capital letters
    return morse_letters[c - 'A'];

  if (c >= '0' && c <= '9') 
    return morse_numbers[c - '0'];

  if (c = ' ') //space
    return " ";
  return NULL;
}




