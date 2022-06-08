#ifndef DIODES_UTIL_H_
#define DIODES_UTIL_H_

#include <stdint.h>


#define d(bit) (1ULL << (bit))

uint8_t coords_to_diode_number(uint8_t x, uint8_t y, uint8_t z);
void diode_number_to_coords(uint8_t* coords, uint8_t diode_number);


#endif /* DIODES_UTIL_H_ */