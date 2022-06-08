#include "fog.h"

#include "../../main.h"
#include "../../diodes_util.h"

void shuffle(uint8_t *array) {
	for (uint8_t i = 0; i < NUMBER_OF_DIODES; i++) {
		uint8_t j = rnd() & 0x3F;
		uint8_t t = array[j];
		array[j] = array[i];
		array[i] = t;
	}
}

int8_t fog(uint16_t number_of_frames) {
	uint64_t frame_data = 0;
	uint8_t fog_data[64];
	for (uint8_t i = 0; i < NUMBER_OF_DIODES; i++) {
		fog_data[i] = i;
	}
	
	uint16_t frame_number = 0;
	while (true) {
		shuffle(fog_data);
		for (uint8_t i = 0; i < NUMBER_OF_DIODES; i++) {
			frame_data ^= d(fog_data[i]);
			int8_t result = render_frame(frame_data, 42);
			if (result != 0) { // return if a key was pressed
				return result;
			}
			if (number_of_frames > 0 && ++frame_number >= number_of_frames) { // return if number of frames exceeded
				return 0;
			}
		}
	}
}