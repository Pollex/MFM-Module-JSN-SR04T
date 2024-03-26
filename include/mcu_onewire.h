#if !defined(_DRIVER_ONEWIRE_H_)
#define _DRIVER_ONEWIRE_H_

#include <avr/io.h>
#include <stdint.h>

#ifndef F_CPU
#error F_CPU must be defined to use ONEWIRE
#endif

// Standard OneWire speeds in microseconds
// Duration of pull down before writing/reading a bit
#define OW_TIME_A 6
// Duration of holding 1/0 bit (high/low)
#define OW_TIME_B 64
// Duration of writing zero
#define OW_TIME_C 60
// Duration to wait after write/read to allow recharge
#define OW_TIME_D 10
// Duration after start to sample line
#define OW_TIME_E 9
// Duration to wait after sample
#define OW_TIME_F 55
// Only used in overdrive mode
#define OW_TIME_G 0
// Duration of reset pulldown
#define OW_TIME_H 480
// Duration after reset to sample presence
#define OW_TIME_I 70
// Duration after sample to wait
#define OW_TIME_J 410

// OneWire ROM Commands
#define OW_CMD_SKIP 0xCC
#define OW_CMD_READ 0x33
#define OW_CMD_SEARCH 0xF0
#define OW_CMD_MATCH 0x55

typedef struct onewire_t {
    PORT_t *port;
    uint8_t pin;
} onewire_t;

#ifdef __cplusplus
extern "C" {
#endif

void ow_init(onewire_t *ow);
uint8_t ow_reset(onewire_t *ow);
void ow_write(onewire_t *ow, uint8_t data);
uint8_t ow_readBit(onewire_t *ow);
uint8_t ow_read(onewire_t *ow);

#ifdef __cplusplus
}
#endif

#endif // _DRIVER_ONEWIRE_H_
