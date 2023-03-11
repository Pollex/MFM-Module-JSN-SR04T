#include <avr/io.h>
#include <os/os.h>
#include "board.h"

void os_presleep(void)
{
  // return;
  // Configure BOD
  _PROTECTED_WRITE(BOD_CTRLA, BOD_CTRLA & ~(BOD_SLEEP_gm));
  //
  // Enable all pullups
  //PORTA.DIRCLR = PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm | PIN6_bm | PIN7_bm;
  //PORTA.PIN1CTRL |= PORT_ISC_INPUT_DISABLE_gc;
  //PORTA.PIN2CTRL |= PORT_ISC_INPUT_DISABLE_gc;
  //PORTA.PIN3CTRL |= PORT_ISC_INPUT_DISABLE_gc;
  //PORTA.PIN4CTRL |= PORT_ISC_INPUT_DISABLE_gc;
  //PORTA.PIN6CTRL |= PORT_ISC_INPUT_DISABLE_gc;


  //USART0.CTRLB &= ~(USART_TXEN_bm | USART_RXEN_bm);
  //PORTB.DIRCLR = PIN2_bm | PIN3_bm;

  //SENSOR_5VEN_PORT.OUTCLR = 1 << SENSOR_5VEN_PIN;
  //SENSOR_3V3EN_PORT.OUTCLR = 1 << SENSOR_3V3EN_PIN;
}

void os_postsleep(void)
{
}
