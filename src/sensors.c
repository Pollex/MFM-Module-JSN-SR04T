#include "sensors.h"
#include "avr/io.h"
#include "board.h"
#include "mcu/util.h"
#include "uart.h"

#define MEDIAN_COUNT 13

uint16_t _get_distance_to_water(void) {
  uint8_t buffer[4];

  // Wait until there is data available or the timeout is reached
  uint32_t start = millis();
  uint8_t frame_start = 0;
  uart_putc(0x55);
  for (;;) {
    // Respect timeout
    if (millis() - start > 200) {
      return 0xFFFF;
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
    return 0xFFFE;
  }

  // Calculate distance and convert from mm to cm
  uint16_t distance = (buffer[1] << 8) | buffer[2];
  return distance / 10;
}

/* Function to sort an array using insertion sort*/
void insertionSort(uint16_t arr[], int n) {
  int i, key, j;
  for (i = 1; i < n; i++) {
    key = arr[i];
    j = i - 1;

    /* Move elements of arr[0..i-1], that are
    greater than key, to one position ahead
    of their current position */
    while (j >= 1 && arr[j] > key) {
      arr[j + 1] = arr[j];
      j = j - 1;
    }
    arr[j + 1] = key;
  }
}

uint16_t get_distance_to_water(void) {
  static uint16_t buf[MEDIAN_COUNT] = {0};
  for (int ix = 0; ix < MEDIAN_COUNT; ix++) {
    buf[ix] = _get_distance_to_water();
    delay_ms(15);
  }
  insertionSort(&buf, MEDIAN_COUNT);
  return buf[MEDIAN_COUNT / 2];
}

float get_temperature(ds18b20_t *ds18b20) {
  ds18b20_read(ds18b20, 0);
  return ds18b20_read(ds18b20, 0);
}

void sensors_poweron(void) {
  SENSOR_5VEN_PORT.DIRSET = 1 << SENSOR_5VEN_PIN;
  SENSOR_5VEN_PORT.OUTSET = 1 << SENSOR_5VEN_PIN;
    OW_EN_PORT.DIRSET = 1 << OW_EN_PIN;
    OW_EN_PORT.OUTSET = 1 << OW_EN_PIN;
  uart_init();
}
void sensors_poweroff(void) {
  SENSOR_5VEN_PORT.OUTCLR = 1 << SENSOR_5VEN_PIN;
    OW_EN_PORT.OUTCLR = 1 << OW_EN_PIN;
  uart_deinit();
}
