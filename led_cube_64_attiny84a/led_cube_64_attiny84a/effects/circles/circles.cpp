#include "circles.h"

#include <avr/pgmspace.h>
#include <stdlib.h>

#include "../../main.h"
#include "../../diodes_util.h"

int8_t circles(uint16_t number_of_frames) {
	uint16_t absolute_frame_number = 0;
	const static uint8_t circles_data[][2] PROGMEM = {
		{0, 60},
		{16, 44},
		{32, 28},
		{48, 12},
		{53, 9},
		{58, 6},
		{63, 3},
		{47, 19},
		{31, 35},
		{15, 51},
		{10, 54},
		{5, 57},
	};
	uint8_t len = sizeof circles_data / sizeof circles_data[0]; 
	uint16_t current_frame_number = 0;
	while (true) {
		for (uint8_t i = 0; i < len; i++) {
			uint64_t frame_data = 0;
			uint16_t transformed_frame_number;
			if (current_frame_number == 200) {
				transformed_frame_number = 201;
			} else {
				transformed_frame_number = current_frame_number;
			}
			uint16_t duration = abs(12000 / int16_t(transformed_frame_number - 200)); // fancy math function
			if (duration >= 105) {
				duration = 15;
			} else {
				duration = 120 - duration;
			}
			uint8_t trail_length;
			if (current_frame_number < 20 || current_frame_number > 380) {
				trail_length = 1;
			} else if (current_frame_number < 30 || current_frame_number > 370) {
				trail_length = 2;
			} else if (current_frame_number < 42 || current_frame_number > 358) {
				trail_length = 3;
			} else if (current_frame_number < 55 || current_frame_number > 345) {
				trail_length = 4;
			} else if (current_frame_number < 70 || current_frame_number > 330) {
				trail_length = 5;
			} else if (current_frame_number < 85 || current_frame_number > 315) {
				trail_length = 6;
			} else {
				trail_length = 7;
			}
			for (uint8_t j = 0; j < trail_length; j++) {
				int8_t offset = i - j;
				if (offset < 0) {
					offset += len;
				}
				frame_data |= d(pgm_read_byte(&circles_data[offset][0]));
				frame_data |= d(pgm_read_byte(&circles_data[offset][1]));
			}
			uint8_t result = render_frame(frame_data, duration);
			if (current_frame_number++ == 400) {
				current_frame_number = 0;
			}
			if (result != 0) { // return if a key was pressed
				return result;
			}
			if (number_of_frames > 0 && ++absolute_frame_number >= number_of_frames) { // return if number of frames exceeded
				return 0;
			}
		}
	}
}