#include <linux/io.h>
#include "Headers/morse_led.h"

#define GPIO_BASE_ADDR  0x3F200000
#define GPIO_RANGE      0xB4

#define SET_ON          0x1C
#define SET_OFF         0x28

#define LED_PIN_BIT     (1 << 29)

static void __ionem *gpio_regs = NULL;


int led_init(void)
{
  gpio_regs = ioremap(GPIO_BASE_ADDR, GPIO_RANGE);
  if (!gpio_regs)
    return -ENOMEM;
  return 0;
}


void led_cleanup(void)

{
  if (gpio_regs) {
    iounmap(gpio_regs);
    gpio_regs = NULL;
  }

}

void led_on(void){

  if (gpio_regs)
    iowrite32(LED_PIN_BIT, gpio_regs + SET_ON);

}


void led_off(void) 
{
  if (gpio_regs)
    iowrite32(LED_PIN_BIT, gpio_regs + SET_OFF)

}

