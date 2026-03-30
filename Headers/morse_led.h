#ifndef MORSE_LED_H
#define MORSE_LED_H


///mapping once loaded
int led_init(void);

//unmap one unloaded
void led_cleanup(void);

//ledlight on
void led_on(void);

//ledlight off
void led_off(void);


#endif
