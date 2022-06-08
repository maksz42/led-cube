#include "rain.h"

#include "../../main.h"
#include "../../diodes_util.h"

uint16_t generate_layer() {
	uint16_t drops_layer_data = 0; // TODO find a better way
	uint8_t number_of_drops = (rnd() & 0x03) + 1; // might be less but that's ok. 1 at least.
	for (uint8_t i = 0; i < number_of_drops; i++) {
		drops_layer_data |= d(rnd() & 0x0F);
	}
	return drops_layer_data;
}

int8_t rain(uint16_t number_of_frames) {
	uint16_t frame_number = 0;
	uint64_t rain_data = 0;
	
	while (true) {
		rain_data >>= 16; // move layer down
		rain_data |= (uint64_t)generate_layer() << 48;
		int8_t result = render_frame(rain_data, 100);
		if (result != 0) { // return if a key was pressed
			return result;
		}
		if (number_of_frames > 0 && ++frame_number >= number_of_frames) { // return if number of frames exceeded
			return 0;
		}
	}
}
