#include "sliding_walls.h"

#include <stdlib.h>

#include "../../main.h"
#include "../../diodes_util.h"

uint8_t position_to_layer(uint8_t pos) {
	return -abs(pos - 3) + 3;
}

uint64_t generate_wall(uint8_t axis, uint8_t position) {
	uint64_t wall_data = 0;
	switch (axis) {
		case 0: { // x 
			uint8_t x_offset = position * 16;
			for (uint8_t i = x_offset; i < x_offset + 16; i++) {
				wall_data |= d(i);
			}
			break;
		}
		case 1: { // y
			uint8_t y_offset_start = position * 4;
			uint8_t y_offset_end = 48 + 4 * position;
			for (uint8_t i = y_offset_start; i <= y_offset_end; i += 16) {
				for (uint8_t j = i; j < i + 4; j++) {
					wall_data |= d(j);
				}
			}
			break;
		}
		case 2: {
			uint8_t z_offset_start = position;
			uint8_t z_offset_end = position + 12;
			for (uint8_t i = z_offset_start; i <= z_offset_end; i += 4) {
				for (uint8_t j = i; j <= i + 48; j += 16) {
					wall_data |= d(j);
				}
			}
			break;
		}
	}
	return wall_data;
}

int8_t sliding_walls(uint16_t number_of_frames) {
	uint16_t frame_number = 0;
	bool wall_meta[3][2]; // holds current wall position - 0 and speed - 1
	for (uint8_t i = 0; i < 3; i++) { // init with random bools
		for (uint8_t j = 0; j < 2; j++) {
			wall_meta[i][j] = rnd() & 0x01;
		}
	}
	
	while (true) {
		for (uint8_t i = 0; i < 10; i++) {
			uint64_t frame_data = 0;
			for (uint8_t axis = 0; axis < 3; axis++) {
				uint8_t divided_position = i;
				if (divided_position > 6) {
					divided_position = 6;
				}
				if (wall_meta[axis][1]) {
					divided_position /= 2;
				}
				uint8_t layer = position_to_layer(divided_position);
				if (wall_meta[axis][0]) {
					layer = 3 - layer;
				}
				frame_data |= generate_wall(axis, layer);
			}	
			int8_t result = render_frame(frame_data, 220);

			if (result != 0) { // return if a key was pressed
				return result;
			}
			if (number_of_frames > 0 && ++frame_number >= number_of_frames) { // return if number of frames exceeded
				return 0;
			}
		}
		for (uint8_t i = 0; i < 3; i++) {
			if (wall_meta[i][1]) {
				wall_meta[i][0] ^= true;
			}
		}
		for (uint8_t i = 0; i < 3; i++) {
			wall_meta[i][1] = rnd() & 0x01;
		}
	}
}