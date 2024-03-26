#include "app_ds18b20.h"
#include "app_jsnsr04t.h"
#include "board.h"
#include "mcu_onewire.h"
#include "mcu_twi.h"
#include "os.h"
#include "os_util.h"
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <string.h>

#define JSNSR04T_MEDIAN_COUNT 13
volatile uint8_t flag_perform_measurement = 0;

ds18b20_t ds18b20 = {
    .resolution = DS18B20_RES_9,
    .port = &DS18B20_PORT,
    .pin = DS18B20_PIN,
    .addr = OW_CMD_SKIP,
};

volatile struct packet_t {
    uint16_t distance;
    float temperature;
} packet;

/**
 * @brief Wait for the Watchdog to synchronize.
 * After changing enabling or disabling the watchdog it needs some time to
 * synchronize before it can be changed safely again
 */
void wdt_sync(void) {
    // Wait for data to synchronize between clock and wdt domain
    while ((WDT.STATUS & WDT_SYNCBUSY_bm) == WDT_SYNCBUSY_bm)
        ;
}

void pwr_5v(uint8_t set) {
    if (set != 0x00) {
        ENABLE_5V_PORT.OUTSET = ENABLE_5V_PIN; // Switch 5V_on to on
    } else {
        ENABLE_5V_PORT.OUTCLR = ENABLE_5V_PIN; // Switch 5V_on to off
    }
}

void pwr_3v3(uint8_t set) {
    if (set != 0x00) {
        ENABLE_3V3_PORT.OUTSET = ENABLE_3V3_PIN; // Switch 3V3_on to on
    } else {
        ENABLE_3V3_PORT.OUTCLR = ENABLE_3V3_PIN; // Switch 3V3_on to off
    }
}

void pwr_init(void) {
    ENABLE_5V_PORT.DIRSET = ENABLE_5V_PIN;   // 5V_on as output
    ENABLE_3V3_PORT.DIRSET = ENABLE_3V3_PIN; // 3V3_on as output

    PORTA.PIN0CTRL |= PORT_ISC_INPUT_DISABLE_gc | PORT_PULLUPEN_bm;
    PORTA.PIN1CTRL |= PORT_ISC_INPUT_DISABLE_gc | PORT_PULLUPEN_bm;
    PORTA.PIN4CTRL |= PORT_ISC_INPUT_DISABLE_gc | PORT_PULLUPEN_bm;
    PORTA.PIN6CTRL |= PORT_ISC_INPUT_DISABLE_gc | PORT_PULLUPEN_bm;
    PORTA.PIN7CTRL |= PORT_ISC_INPUT_DISABLE_gc | PORT_PULLUPEN_bm;

    pwr_3v3(0);
    pwr_5v(0);
}

void os_presleep() {
    wdt_disable();
    wdt_sync();
}
void os_postsleep() {
    wdt_enable(WDT_PERIOD_8KCLK_gc);
    wdt_sync();
}

/* Function to sort an array using insertion sort*/
void insertionSort(uint16_t *arr, int n) {
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

void perform_measurement(void) {
    static float ds18b20_temperature = 0;
    static uint16_t jsnsr04t_distance = 0;
    static uint16_t jsnsr04t_distances[JSNSR04T_MEDIAN_COUNT] = {0};

    pwr_3v3(1);
    pwr_5v(1);
    delay_ms(10);

    // Read temperature, first measurement after poweron is always an error
    int err = ds18b20_read(&ds18b20, &ds18b20_temperature);
    if (err < 0) {
        ds18b20_temperature = -100;
    }
    // Shutdown the DS18B20
    pwr_3v3(0);

    // Do a median filter on the JSN-SR04t distances
    for (int ix = 0; ix < JSNSR04T_MEDIAN_COUNT; ix++) {
        jsnsr04t_distances[ix] = jsnsr04t_read();
    }
    insertionSort(jsnsr04t_distances, JSNSR04T_MEDIAN_COUNT);
    jsnsr04t_distance = jsnsr04t_distances[JSNSR04T_MEDIAN_COUNT / 2];

    // Shutdown the JSN-SR04T
    pwr_5v(0);

    packet.distance = jsnsr04t_distance;
    packet.temperature = ds18b20_temperature;
}

int main(void) {
    // Setup
    os_init();
    delay_init();
    pwr_init();
    jsnsr04t_init();
    ds18b20_init(&ds18b20);

    sei();
    twi_init(0x36, 1);

    // Setup WDT
    wdt_enable(WDT_PERIOD_8KCLK_gc);
    wdt_sync();

    // Loop
    for (;;) {
        if (flag_perform_measurement == 1) {
            flag_perform_measurement = 0;
            perform_measurement();
        }

        wdt_reset();
        os_sleep();
    }
}

void twi_cmd_10_handler(uint8_t *buf, uint8_t len) {
    flag_perform_measurement = 1;
}
void twi_cmd_11_handler(uint8_t *buf, uint8_t len) {
    buf[0] = sizeof(struct packet_t);
    memcpy(&buf[1], (void *)&packet, buf[0]);
    memset((void *)&packet, 0, sizeof(struct packet_t));
}

twi_cmd_t twi_cmds[] = {
    {0x10, &twi_cmd_10_handler},
    {0x11, &twi_cmd_11_handler},
};
