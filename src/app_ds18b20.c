#include "app_ds18b20.h"
#include "mcu_onewire.h"
#include "os_util.h"

#define ERROR_VALUE 100
#define MAX_RETRIES 5

#define DS18B20_CONVERT 0x44
#define DS18B20_WRITE_SCRATCHPAD 0x4E
#define DS18B20_READ_SCRATCHPAD 0xBE
#define DS18B20_COPY_SCRATCHPAD 0x48

void convert_t(ds18b20_t *d) {
    ow_reset(&d->ow);
    ow_write(&d->ow, d->addr);
    ow_write(&d->ow, DS18B20_CONVERT);
}

uint16_t read_temp(ds18b20_t *d) {
    ow_reset(&d->ow);
    ow_write(&d->ow, d->addr);
    ow_write(&d->ow, DS18B20_READ_SCRATCHPAD);

    uint8_t lsb = ow_read(&d->ow);
    uint8_t msb = ow_read(&d->ow);

    // Depending on the resolution, certain bits at the LSB arent used
    // we move and mask them
    uint8_t undefined_bits = 3 - d->resolution;
    uint16_t value = ((uint16_t)msb << 8) | (uint16_t)lsb;
    value >>= undefined_bits;
    value &= ~(((1 << undefined_bits)-1) << (12-undefined_bits));
    return value;
}

void set_resolution(ds18b20_t *d) {
    ow_reset(&d->ow);
    ow_write(&d->ow, d->addr);
    ow_write(&d->ow, DS18B20_WRITE_SCRATCHPAD);
    ow_write(&d->ow, 0x00);
    ow_write(&d->ow, 0x00);
    ow_write(&d->ow, (d->resolution << 5) | 0x1F);
}

float get_res_bit(uint8_t res) {
    switch (res) {
    case DS18B20_RES_12:
        return 0.0625;
    case DS18B20_RES_11:
        return 0.125;
    case DS18B20_RES_10:
        return 0.25;
    case DS18B20_RES_9:
        return 0.5;
    }
    return 0.0625;
}

void wait_convert(uint8_t res) {
    switch (res) {
    case DS18B20_RES_12:
        delay_ms(750);
        break;
    case DS18B20_RES_11:
        delay_ms(375);
        break;
    case DS18B20_RES_10:
        delay_ms(188);
        break;
    case DS18B20_RES_9:
        delay_ms(94);
        break;
    }
}

void ds18b20_init(ds18b20_t *d) {
    d->ow.port = d->port;
    d->ow.pin = d->pin;
    ow_init(&d->ow);
}

int ds18b20_read(ds18b20_t *d, float *temperature) {
    set_resolution(d);
    convert_t(d);

    //wait_convert(d->resolution);

    uint32_t start = millis();
    uint32_t now = start;
    do {
        delay_ms(1);
        now = millis();
        if (now - start > 800)
            return -1;
    } while (!ow_readBit(&d->ow));

    uint16_t raw = read_temp(d);
    if (raw == 0xffff) {
        return -1;
    }

    // Convert
    uint8_t sign = raw >> 15;
    *temperature = (float)(raw & 0x7FFF) * get_res_bit(d->resolution) * (sign ? -1 : 1);
    return 0;
}
