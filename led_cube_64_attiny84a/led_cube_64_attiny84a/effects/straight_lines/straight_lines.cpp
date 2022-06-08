#include "straight_lines.h"

#include <stdlib.h>
#include <avr/pgmspace.h>

#include "../../main.h"
#include "../../diodes_util.h"


#define offset_phase1 13
#define offset_phase2 24
#define offset_phase3 63
#define offset_phase4 112


int8_t straight_lines(uint16_t number_of_frames) {
	uint16_t absolute_frame_number = 0;
	uint8_t current_frame_number = 0;
	uint8_t phase = 0;

	while (true) {
		uint64_t frame_data = 0;
		int8_t result;
		switch (phase) {
			case 0: {
				uint8_t offset = abs(current_frame_number % 6 - 3) * 16;  // moving layer up/down
				frame_data = (uint64_t)0xFFFF << offset;
				result = render_frame(frame_data, 100);
			}
			break;
			case 1: {
				frame_data = current_frame_number % 2 ? 0 : UINT64_MAX;
				uint16_t duration = (rnd() & 0xFF) + 50;
				result = render_frame(frame_data, duration);
			}
			break;
			case 2: {
				int8_t high_boundary = current_frame_number - offset_phase2 - 1;
				int8_t low_boundary = high_boundary - 21;
				for (int8_t i = low_boundary; i <= high_boundary; i++) {
					if (i >= 0 && i <= 15){
						uint8_t height = 3; // h - 1
						uint8_t difference;
						if (high_boundary < 18) {
							difference = high_boundary - i;
						} else {
							difference = i - low_boundary;
						}
						if (difference < 4) {
							height = difference;
						}

						for (uint8_t j = i; j <= i + height * 16; j += 16) {
							frame_data |= d(j);
						}
					}
				}
				result = render_frame(frame_data, 100);
			}
			break;
			case 3: {
				static const uint64_t data[] PROGMEM =
				{
					d(0)|d(5)|d(10)|d(15),
					d(1)|d(5)|d(10)|d(14),
					d(2)|d(6)|d(9)|d(13),
					d(3)|d(6)|d(9)|d(12),
					d(7)|d(6)|d(9)|d(8),
					d(11)|d(10)|d(5)|d(4)
				};
				uint8_t phase_frame = (current_frame_number - offset_phase3);
				if (phase_frame == 48) {
					phase_frame = 36; // last frame edge condition
				}
				for (uint8_t i = 0; i <= phase_frame / 12; i++) {
					frame_data |= (uint64_t)pgm_read_word(&data[phase_frame % 6]) << (i * 16);
				}
				result = render_frame(frame_data, 100);
			}
			break;
		}
		current_frame_number++;
		
		if (result != 0) { // return if a key was pressed
			return result;
		}
		if (number_of_frames > 0 && ++absolute_frame_number >= number_of_frames) { // return if number of frames exceeded
			return 0;
		}
		if (
		current_frame_number == offset_phase1 ||
		current_frame_number == offset_phase2 ||
		current_frame_number == offset_phase3 ||
		current_frame_number == offset_phase4
		) {
			phase++;
		}
		if (phase == 5) {
			phase = 0;
			current_frame_number = 0;
		}
	}
}