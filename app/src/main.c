/**
 * @file main.c
 */

#include <inttypes.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "BTN.h"
#include "LED.h"

#define SLEEP_MS 500

int main(void) {
  if (0 > BTN_init()) {
    return 0;
  }
  if (0 > LED_init()) {
    return 0;
  }

  LED_blink(LED1, LED_2HZ);

  while (1) {
    LED_toggle(LED0);
    LED_toggle(LED3);
    LED_set(LED2, LED_ON);
    k_msleep(SLEEP_MS);
    LED_set(LED2, LED_OFF);
    k_msleep(SLEEP_MS);
  }
  
  return 0;
}
