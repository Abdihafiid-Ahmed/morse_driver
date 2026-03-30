#include "morse_thread.h"
#include "morse_buffer.h"
#include "morse_ledlight.h"
#include "morse_encode.h"


///durarion in ms 
static int unit_ms = 100;


static void flash_light(const char *symbol)
{
  const char *p;

  for (p = symbol; *p != '\0'; p++){
    if (*p == '.'){

      led_on();
      msleep(unit_ms);

    } else if (*p == '-'){

      led_on();
      msleep(unit_ms * 3);
    }

    led_off();
    msleep(unit_ms);
  }
}

void morse_thread_unit(int ms)
{
  if (ms > 0)
    unit_ms = ms;
}
