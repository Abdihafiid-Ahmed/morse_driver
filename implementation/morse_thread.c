#include "morse_thread.h"
#include "morse_buffer.h"
#include "morse_led.h"
#include "morse_encode.h"
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/delay.h>
////all the linux headers and our own headers for the "API" uses


////kernel structure representing thread pointing to thread thats responsible for morse code
static struct task_struct *transmit_task = NULL;

/////stores morse code waiting to be transmittted 
static struct morse_buffer *shared_buff = NULL;

/////waiting station to sleep untill there is work to be done
DECLARE_WAIT_QUEUE_HEAD(morse_wait_queue);

////concurrency control
struct mutex morse_buff_mutex;


///durarion in ms 
static int unit_ms = 300;

////translate the morses to led lights and also handles the timing too
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

void morse_thread_set_unit(int ms)
{
  if (ms > 0)
    unit_ms = ms;
}

////kernel thread function
static int transmit_fn(void *data)
{
  char c;
  const char *symbol;


////////will rin untill told to stop
  while (!kthread_should_stop())
  { ///sleep until buffer is not empty or thread is told to stop
    wait_event_interruptible(morse_wait_queue, !circlebuff_empty(shared_buff) || kthread_should_stop());

    if (kthread_should_stop())
      break;

   //////safely access the shared buffer
    mutex_lock(&morse_buff_mutex);

   ///removes and character from the buffer and retires tje loop if buffer is empty
    if (circlebuff_remove(shared_buff, &c) != 0)
    {
      mutex_unlock(&morse_buff_mutex);
      continue;
    }

    mutex_unlock(&morse_buff_mutex);


    ////conversion to morse and different timing for stuff like word gap and letter gap etc 
    symbol = morse_get_symbol(c);

    if (symbol == NULL)
    {
      continue;
    }

    if (symbol[0] == ' ')
    {
      led_off();
      msleep(unit_ms * 6);

    } else 
  {
      flash_light(symbol);
      led_off();
      msleep(unit_ms * 3);
    }
  }

  led_off();
  return 0;


}

/////starts the morse transmission thread
int morse_thread_start(struct morse_buffer *buff)
{
  shared_buff = buff;
  ////create thread
  transmit_task = kthread_run(transmit_fn, NULL, "morse_transmit");
  if (IS_ERR(transmit_task)) ///error handling
  {
    transmit_task = NULL;
    return -ENOMEM;
  }
  return 0;
}


/////stops tje thread
void morse_thread_stop(void)
{
  if (transmit_task)
  {
   //wake thread in case its sleepin on wait queue 
    wake_up(&morse_wait_queue);

    kthread_stop(transmit_task);

    transmit_task = NULL;
  }
}

 
