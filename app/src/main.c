/**
 * @file main.c
 */

#include <inttypes.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "BTN.h"
#include "LED.h"

#define SLEEP_MS 1000

int main(void) {
  if (0 > BTN_init()) {
    return 0;
  }
  if (0 > LED_init()) {
    return 0;
  }

  uint8_t counter = 0;

  while(1){
    if(BTN_check_clear_pressed(BTN0)){
      counter += 1;

      uint8_t led0 = (counter >> 0) & 1;
      uint8_t led1 = (counter >> 1) & 1;
      uint8_t led2 = (counter >> 2) & 1;
      uint8_t led3 = (counter >> 3) & 1;

      LED_set(LED0, led0? LED_ON : LED_OFF);
      LED_set(LED1, led1? LED_ON : LED_OFF);
      LED_set(LED2, led2? LED_ON : LED_OFF);
      LED_set(LED3, led3? LED_ON : LED_OFF);
    }

    if(counter > 15){
      counter = 0;
    }

    k_msleep(10);
  }

  //Led Blinking
  /*
  LED_blink(LED1, LED_2HZ);
  while (1) {
    LED_toggle(LED0);
    LED_toggle(LED3);
    LED_set(LED2, LED_ON);
    k_msleep(SLEEP_MS);
    LED_set(LED2, LED_OFF);
    k_msleep(SLEEP_MS);
  }
  */

  //LED Dimming with pwn
  /*
  while(1){
    for(uint8_t counter = 0; counter <= 100; counter += 10){
      LED_pwm(LED0, counter);
      k_msleep(SLEEP_MS);
    }
    for(uint8_t counter = 100; counter >= 0; counter -= 10){
      LED_pwm(LED0, counter);
      k_msleep(SLEEP_MS);
    }
  }
  */

  //Button pressing
  /*
  while(1){
    if(BTN_check_clear_pressed(BTN0)){
      LED_toggle(LED0);
      printk("Button 0 Pressed \n");
    }
    k_msleep(10);
  }
  */
  return 0;
}
