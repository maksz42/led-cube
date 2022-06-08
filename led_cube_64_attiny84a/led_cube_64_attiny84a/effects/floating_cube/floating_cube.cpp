#include "floating_cube.h"

#include <stdint.h>
#include <avr/pgmspace.h>

#include "../../main.h"
#include "../../diodes_util.h"


int8_t floating_cube(uint16_t number_of_frames) {
	static const uint64_t cube_data[] PROGMEM =
	{
		d(0)|d(1)|d(2)|d(3)|d(4)|d(7)|d(8)|d(11)|d(12)|d(13)|d(14)|d(15)|d(16)|d(19)|d(28)|d(31)|d(32)|d(35)|d(44)|d(47)|d(48)|d(49)|d(50)|d(51)|d(52)|d(55)|d(56)|d(59)|d(60)|d(61)|d(62)|d(63),
		d(1)|d(2)|d(3)|d(5)|d(7)|d(9)|d(10)|d(11)|d(17)|d(19)|d(25)|d(27)|d(33)|d(34)|d(35)|d(37)|d(39)|d(41)|d(42)|d(43),
		d(2)|d(3)|d(6)|d(7)|d(18)|d(19)|d(22)|d(23),
		d(1)|d(2)|d(5)|d(6)|d(17)|d(18)|d(21)|d(22),
		d(0)|d(1)|d(4)|d(5)|d(16)|d(17)|d(20)|d(21),
		d(21)|d(22)|d(25)|d(26)|d(37)|d(38)|d(41)|d(42),
		d(42)|d(43)|d(46)|d(47)|d(58)|d(59)|d(62)|d(63),
		d(41)|d(42)|d(45)|d(46)|d(57)|d(58)|d(61)|d(62),
		d(40)|d(41)|d(44)|d(45)|d(56)|d(57)|d(60)|d(61),
		d(20)|d(21)|d(22)|d(30)|d(24)|d(26)|d(28)|d(29)|d(36)|d(38)|d(44)|d(46)|d(52)|d(53)|d(54)|d(56)|d(58)|d(60)|d(61)|d(62)
	};

	uint16_t frame_number = 0;
	while (true) {
		for (auto const &frame_flash : cube_data) {
			uint64_t frame_data;
			memcpy_P(&frame_data, &frame_flash, sizeof(*cube_data));
			int8_t result = render_frame(frame_data, 200);
			if (result != 0) { // return if a key was pressed
				return result;
			}
			if (number_of_frames > 0 && ++frame_number >= number_of_frames) { // return if number of frames exceeded
				return 0;
			}
		}
	}
}