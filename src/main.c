#include "board.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <drivers/onewire.h>
#include <mcu/twi.h>
#include <mcu/util.h>
#include <os/linked_queue.h>
#include <os/os.h>
#include <perif/ds18b20.h>
#include <util/atomic.h>
#include <util/delay.h>
#include <uart.h>

#define TWI_CMD_PERFORM 0x10
#define TWI_CMD_READ 0x11

// Tasks
void measurement(void);
os_task measurement_task = {
    .func = &measurement,
    .priority = 1,
};

uint16_t get_distance_to_water(void);

typedef struct {
  unsigned short distance;
  float temperature;
} packet_t;

int main(void) {
  SENSOR_5VEN_PORT.DIRSET = 1 << SENSOR_5VEN_PIN;
  SENSOR_3V3EN_PORT.DIRSET = 1 << SENSOR_3V3EN_PIN;
  SENSOR_5VEN_PORT.OUTCLR = 1 << SENSOR_5VEN_PIN;
  SENSOR_3V3EN_PORT.OUTCLR = 1 << SENSOR_3V3EN_PIN;

  sei(); // Enable interrupts
  delay_init();

  os_init();
  twi_init(0x36, 1);
  uart_init();

  for (;;) {
    os_processTasks();
    os_sleep();
  }
}

uint16_t get_distance_to_water(void) {
  uint8_t buffer[4];

  // Wait until there is data available or the timeout is reached
  uint32_t timeout_ms = millis() + 200;
  uint8_t frame_start = 0;
  uart_putc(0x55);
  for (;;) {
    // Respect timeout
    if (millis() > timeout_ms) {
      return -1;
    }

    // JSN response frame starts with 0xFF
    // if there is anything available then check if the first byte
    // is 0xFF, if not then discard it. Do this until the byte is 0xFF
    while (!frame_start && uart_available() > 0) {
      if (uart_peek() == 0xff) {
        frame_start = 1;
      } else {
        uart_getc();
      }
    }
    if (frame_start && uart_available() >= 4) {
      break;
    }
  }

  // Read the JSN data frame into the buffer
  uart_read(buffer, 4);

  // Verify the read data through its checksum
  uint8_t checksum =
      ((uint16_t)buffer[0] + (uint16_t)buffer[1] + (uint16_t)buffer[2]) & 0xff;
  if (checksum != buffer[3]) {
    return -2;
  }

  // Calculate distance and convert from mm to cm
  uint16_t distance = (buffer[1] << 8) | buffer[2];
  return distance / 10;
}

/* Function to sort an array using insertion sort*/
void insertionSort(int arr[], int n)
{
    int i, key, j;
    for (i = 1; i < n; i++)
    {
        key = arr[i];
        j = i - 1;
 
        /* Move elements of arr[0..i-1], that are
        greater than key, to one position ahead
        of their current position */
        while (j >= 0 && arr[j] > key)
        {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

volatile packet_t packet;
void measurement(void) {
  SENSOR_5VEN_PORT.DIRSET = 1 << SENSOR_5VEN_PIN;
  SENSOR_3V3EN_PORT.DIRSET = 1 << SENSOR_3V3EN_PIN;
  SENSOR_5VEN_PORT.OUTSET = 1 << SENSOR_5VEN_PIN;
  SENSOR_3V3EN_PORT.OUTSET = 1 << SENSOR_3V3EN_PIN;
  delay_ms(10);
  uart_init();

  uint16_t distanceMedianBuffer[13] = {};
  for (int i=0;i<13;i++){
      distanceMedianBuffer[i] = get_distance_to_water();
      delay_ms(10);
  }
  insertionSort(distanceMedianBuffer, 13);

  packet = (packet_t){
      .distance = distanceMedianBuffer[6],
      .temperature = ds18b20_read(0)
  };
}

void twi_perform(uint8_t *buf, uint8_t length) {
  os_pushTask(&measurement_task);
}

void twi_read(uint8_t *buf, uint8_t length) {
  buf[0] = sizeof(packet_t);
  memcpy(&buf[1], &packet, sizeof(packet_t));
}

twi_cmd_t twi_cmds[] = {
    {
        .cmd = TWI_CMD_PERFORM,
        .handler = &twi_perform,
    },
    {
        .cmd = TWI_CMD_READ,
        .handler = &twi_read,
    },
};
