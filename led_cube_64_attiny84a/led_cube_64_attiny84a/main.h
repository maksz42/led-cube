#ifndef MAIN_H_
#define MAIN_H_

#include <stdint.h>

#define NUMBER_OF_DIODES 64

int8_t render_frame(const uint64_t& data, uint16_t duration);
uint8_t rnd();


#endif /* MAIN_H_ */