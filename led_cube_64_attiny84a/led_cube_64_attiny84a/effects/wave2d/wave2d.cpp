#include "wave2d.h"

#include <stdlib.h>

#include "../../main.h"
#include "../../diodes_util.h"

int8_t wave2d(uint16_t number_of_frames) {
	uint16_t absolute_frame_number = 0;
	uint8_t current_frame_number = 0;

	while (true) {
		uint64_t wave_data = 0;
		if (current_frame_number == 252) { // modulo 6 fluent overflow
			current_frame_number = 0;
		}

		for (uint8_t i = 0; i < 4; i++) {
			uint8_t y_coord = ((-abs((current_frame_number + i) % 6 - 3) + 3) * 16 + i);
			for (uint8_t j = 0; j < 4; j++) {
				wave_data |= d(y_coord + j * 4);
			}
		}
		uint8_t result = render_frame(wave_data, 150);
		current_frame_number++;
		if (result != 0) { // return if a key was pressed
			return result;
		}
		if (number_of_frames > 0 && ++absolute_frame_number >= number_of_frames) { // return if number of frames exceeded
			return 0;
		}
	}
}