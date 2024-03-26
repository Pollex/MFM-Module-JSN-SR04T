#include "board.h"
#include "sensors.h"

#include <avr/interrupt.h>
#include <drivers/onewire.h>
#include <mcu/twi.h>
#include <mcu/util.h>
#include <os/linked_queue.h>
#include <os/os.h>
#include <perif/ds18b20.h>
#include <stdint.h>
#include <string.h>
#include <uart.h>

#define TWI_CMD_PERFORM 0x10
#define TWI_CMD_READ 0x11

// Tasks
void measurement(void);
os_task measurement_task = {
    .func = &measurement,
    .priority = 1,
};

ds18b20_t ds18b20 = {.resolution = DS18B20_RES_12};

volatile __attribute__((packed)) struct {
    uint16_t distance;
    float temperature;
} packet = {
    .distance = 0xADDE,
    .temperature = 0xDEAD,
};

int main(void) {
    os_init();

    delay_init();

    sensors_poweron();
    sensors_poweroff();

    sei(); // Enable interrupts

    twi_init(0x36, 1);

    // Disable BOD for lower sleep power
    _PROTECTED_WRITE(BOD_CTRLA, BOD_CTRLA & ~(BOD_SLEEP_gm));

    for (;;) {
        os_processTasks();
        os_sleep();
    }
}

void measurement(void) {
    sensors_poweron();
    delay_ms(50);
    packet.temperature = get_temperature(&ds18b20);
    packet.distance = get_distance_to_water();
    sensors_poweroff();
}

void twi_perform(uint8_t *buf, uint8_t length) {
    os_pushTask(&measurement_task);
}

void twi_read(uint8_t *buf, uint8_t length) {
    buf[0] = sizeof(packet);
    memcpy(&buf[1], &packet, buf[0]);
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
