#include "mcu_onewire.h"

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>

#define _high_cycles 4
#define _sample_cycles 4
#define _low_cycles 7

#define usToCycles(us) ceil(fabs(((F_CPU / 1000000) * us * 1.1)))

extern void __builtin_avr_delay_cycles(unsigned long);

#define _low()                                                                 \
    ow->port->DIRSET = 1 << ow->pin;                                           \
    ow->port->OUTCLR = 1 << ow->pin;
#define _high() ow->port->DIRCLR = 1 << ow->pin;
#define _sample() ((ow->port->IN & (1 << ow->pin)) > 0)

void ow_init(onewire_t *ow) { _high(); }

uint8_t ow_reset(onewire_t *ow) {
    uint8_t data;
    _low();
    __builtin_avr_delay_cycles(usToCycles(OW_TIME_H));
    _high();
    __builtin_avr_delay_cycles(usToCycles(OW_TIME_I));
    data = _sample();
    __builtin_avr_delay_cycles(usToCycles(OW_TIME_J));
    return data;
}

void ow_write(onewire_t *ow, uint8_t data) {
    for (uint8_t bit = 0; bit < 8; bit++) {
        if (data & 0x01) {
            _low();
            __builtin_avr_delay_cycles(usToCycles(OW_TIME_A) - _high_cycles);
            _high();
            __builtin_avr_delay_cycles(usToCycles(OW_TIME_B));
        } else {
            _low();
            __builtin_avr_delay_cycles(usToCycles(OW_TIME_C) - _high_cycles);
            _high();
            __builtin_avr_delay_cycles(usToCycles(OW_TIME_D));
        }
        data >>= 1;
    }
}

uint8_t ow_readBit(onewire_t *ow) {
    uint8_t data;
    _low();
    __builtin_avr_delay_cycles(usToCycles(OW_TIME_A));
    _high();
    __builtin_avr_delay_cycles(usToCycles(OW_TIME_E));
    data = _sample();
    __builtin_avr_delay_cycles(usToCycles(OW_TIME_F));
    return data;
}

uint8_t ow_read(onewire_t *ow) {
    uint8_t data;
    for (uint8_t bit = 0; bit < 8; bit++) {
        data >>= 1;
        if (ow_readBit(ow))
            data |= 0x80;
    }
    return data;
}
