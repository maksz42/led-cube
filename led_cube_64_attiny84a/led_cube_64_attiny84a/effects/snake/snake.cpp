#include "snake.h"

#include <stdlib.h>

#include "../../main.h"
#include "../../diodes_util.h"


uint8_t get_random_start_position() {
	return rnd() & 0x3F;
}

void get_next_segment(int8_t* last_move, uint8_t* current_segment) {
	// prepare directions to draw next move
	int8_t directions_lookup_table[5]; // snake can go in 5 directions
	uint8_t len = sizeof(directions_lookup_table) / sizeof(*directions_lookup_table);

	int8_t direction = -3;
	for (uint8_t i = 0; i < len; i++, direction++) {
		if (direction == 0) { // omit 0 and last move
			direction++;
			if (direction == -(*last_move)) {
				direction++;
			}
		} else if (direction == -(*last_move)) {
			direction++;
			if (direction == 0) {
				direction++;
			}
		}
		directions_lookup_table[i] = direction;
	}
	
	uint8_t next_move_index = rnd() % len;
	int8_t next_move = directions_lookup_table[next_move_index];
	*last_move = next_move;

	uint8_t coords[3];
	diode_number_to_coords(coords, *current_segment);
	// TODO optimize abs
	uint8_t* coord_to_change = (coords + (abs(next_move) - 1)); // pointer to one of axis
	if(next_move > 0) { // set new axis value and avoid overflow
		*coord_to_change += 1;
		if (*coord_to_change == 4) {
			*coord_to_change = 0;
		}
	} else {
		*coord_to_change -= 1;
		if (*coord_to_change == 255) {
			*coord_to_change = 3;
		}
	}
	*current_segment = coords_to_diode_number(coords[0], coords[1], coords[2]);
}

int8_t snake(uint8_t number_of_frames) {
	uint8_t snake_data[] = {get_random_start_position(), 0xFF, 0xFF, 0xFF};
	const uint8_t last_segment = sizeof(snake_data) / sizeof(*snake_data) - 1;
	int8_t last_move = (rnd() % 6) - 3;
		if (last_move >= 0) {
		last_move++;
	}

	int8_t result = render_frame(0, 200);
	if (result != 0) {
		return result;
	}
	
	uint16_t frame_number = 1; // 1 frame already displayed
	while (true) {
		if (number_of_frames > 0 && ++frame_number >= number_of_frames) { // return if number of frames exceeded
			return 0;
		}

		uint64_t frame_buffer = 0;
		for (int8_t i = last_segment; i >= 0; i--) {
			// prepare data for display
			if (snake_data[i] != 0xFF) {
				frame_buffer |= d(snake_data[i]);
			}

			// make new frame
			if (i != 0) {
				snake_data[i] = snake_data[i - 1];
			} else {
				get_next_segment(&last_move, &snake_data[0]);
			}
		}
		result = render_frame(frame_buffer, 200);
		if (result != 0) {
			return result;
		}
	}
}