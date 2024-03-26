#if !defined(_PERIF_DS18B20_H_)
#define _PERIF_DS18B20_H_

#include "mcu_onewire.h"
#include <avr/io.h>
#include <stdint.h>

#define DS18B20_RES_12 3
#define DS18B20_RES_11 2
#define DS18B20_RES_10 1
#define DS18B20_RES_9 0

typedef struct ds18b20_t {
    uint8_t resolution;
    PORT_t *port;
    uint8_t pin;
    uint8_t addr;
    onewire_t ow;
} ds18b20_t;

#ifdef __cplusplus
extern "C" {
#endif

void ds18b20_init(ds18b20_t *d);
int ds18b20_read(ds18b20_t *d, float *temperature);

#ifdef __cplusplus
}
#endif

#endif // _PERIF_DS18B20_H_
