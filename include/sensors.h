#ifndef SENSORS_H
#define SENSORS_H

#include <stdint.h>
#include "perif/ds18b20.h"

void sensors_poweron(void);
void sensors_poweroff(void);
uint16_t get_distance_to_water(void);
float get_temperature(ds18b20_t *ds18b20);

#endif /* end of include guard: SENSORS_H */
