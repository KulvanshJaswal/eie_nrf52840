/**
 * @file main.c
 */

#include <inttypes.h>
#include <stdbool.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "BTN.h"
#include "LED.h"

#define MAX_PASS_LENGTH 8
#define BOOT_WINDOW_MS 3000

typedef enum {
  START_BOOT,
  ENTRY,
  LOCKED,
  WAITING
} app_state;

static void clear_buttons(void) {
  BTN_clear_pressed(BTN0);
  BTN_clear_pressed(BTN1);
  BTN_clear_pressed(BTN2);
  BTN_clear_pressed(BTN3);
}

static int8_t read_digit(void) {
  if (BTN_check_clear_pressed(BTN0)){
    return 0;
  } else if (BTN_check_clear_pressed(BTN1)){
    return 1;
  } else if (BTN_check_clear_pressed(BTN2)) {
    return 2;
  }
  return -1;
}

static void add_press(uint8_t *arr, uint8_t *len, uint8_t value){
  if (*len < MAX_PASS_LENGTH) {
    arr[*len] = value;
    (*len)++;
  }
}

int main(void){
  if (0 > BTN_init()) {
    return 0;
  }
  if (0 > LED_init()) {
    return 0;
  }


  /*
  Password with button program

  Use BTN's 0 1 2 as inputs and use BTN 3 as an ENTER command
  when BTN 3 is clicked an output of Correct or Incorrect
  the program shoudl start with LED0 on as they are in LOCKED state 
  when a user ENTERS with BTN 3 it should go into a WAITING state and LED0 should turn off
  when the user does press enter it should reset after and go back into LOCKED state
  

  when the board starts LED3 should be on for three seconds during which the user can press 
  BTN3 to start typing a password with BTN's 0 1 2 and it is in ENTRY state until BTN 3 is pressed again
  when they click BTN3 again the program should go into LOCKED state and the password should be saved 
  */

  app_state state = START_BOOT;

  uint8_t password[MAX_PASS_LENGTH] = {0, 1, 2};  //default pass
  uint8_t password_len = 3;
  uint8_t attempt[MAX_PASS_LENGTH] = {0};
  uint8_t attempt_len = 0;

  int64_t boot_start = k_uptime_get();
  LED_set(LED3, LED_ON);
  clear_buttons();

  while (1) {
    switch (state) {
      case START_BOOT:{
        if (BTN_check_clear_pressed(BTN3)){
          LED_set(LED3, LED_OFF);
          LED_blink(LED1, LED_2HZ);
          password_len = 0;
          clear_buttons();
          state = ENTRY;

        } else if (k_uptime_get() - boot_start >= BOOT_WINDOW_MS){
          LED_set(LED3, LED_OFF);
          LED_set(LED0, LED_ON);
          clear_buttons();
          state = LOCKED;

        }
        break;
      }

      case ENTRY:{
        int8_t digit = read_digit();
        if (digit >= 0) {
          add_press(password, &password_len, (uint8_t)digit);
        } else if (BTN_check_clear_pressed(BTN3)) {
          if (password_len > 0) {
            LED_set(LED1, LED_OFF);
            LED_set(LED0, LED_ON);
            attempt_len = 0;
            clear_buttons();
            state = LOCKED;

          }
        }
        break;
      }

      case LOCKED:{
        int8_t digit = read_digit();
        if (digit >= 0) {
          add_press(attempt, &attempt_len, (uint8_t)digit);
        } else if (BTN_check_clear_pressed(BTN3)) {
          bool correct = attempt_len == password_len;

          for (uint8_t i = 0; correct && i < password_len; i++){
            if (attempt[i] != password[i]) {
              correct = false;
            }
          }
          printk(correct? "Correct!\n": "Incorrect!\n");
          LED_set(LED0, LED_OFF);
          clear_buttons();
          state = WAITING;
        }
        break;
      }

      case WAITING:{
        BTN_check_clear_pressed(BTN0);
        BTN_check_clear_pressed(BTN1);
        BTN_check_clear_pressed(BTN2);

        if (BTN_check_pressed(BTN3)){
          clear_buttons();
          attempt_len = 0;
          LED_set(LED0, LED_ON);
          state = LOCKED;
          printk("LOCKED\n");
        }
        break;
      }
    }
    k_msleep(10);
  }

  /*  
  Led Blinking

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

  /*
  LED Dimming with pwn

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

  /*
  4-bit binary counter 

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
  */
  return 0;
}