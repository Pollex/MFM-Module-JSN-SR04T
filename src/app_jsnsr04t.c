#include "app_jsnsr04t.h"
#include "mcu_uart.h"
#include "os_util.h"

void jsnsr04t_init(void) { uart_init(); }

uint16_t jsnsr04t_read(void) {
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
        ((uint16_t)buffer[0] + (uint16_t)buffer[1] + (uint16_t)buffer[2]) &
        0xff;
    if (checksum != buffer[3]) {
        return 0xFFFE;
    }

    // Calculate distance and convert from mm to cm
    uint16_t distance = (buffer[1] << 8) | buffer[2];
    return distance / 10;
}
