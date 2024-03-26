#include "os.h"

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/atomic.h>

uint8_t os_isBusy(void) { return (os_hasLock()); }

void os_init(void) {
    _PROTECTED_WRITE(BOD_CTRLA, BOD_CTRLA & ~(BOD_SLEEP_gm));
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
}

void os_sleep(void) {
    cli();
    if (!os_isBusy()) {
        os_presleep();
        sei();
        sleep_cpu();
        os_postsleep();
    }
    sei();
}
