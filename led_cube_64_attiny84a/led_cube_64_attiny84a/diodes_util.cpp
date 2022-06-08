#include "diodes_util.h"

#include <stdint.h>

uint8_t coords_to_diode_number(uint8_t x, uint8_t y, uint8_t z)
{
	return x * 16 + y * 4 + z;
}

// x; y; z
void diode_number_to_coords(uint8_t* coords, uint8_t diode_number) {
	coords[0] = diode_number / 16; // x
	coords[1] = (diode_number % 16) / 4; // y
	coords[2] = diode_number % 4; // z
}