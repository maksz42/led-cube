#include "main.h"

#define F_CPU 20000000UL

#include <avr\io.h>
#include <avr\interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <util/atomic.h>

#include "pin_config.h"
#include "diodes_util.h"

#include "spi.h"

//effects
#include "effects/floating_cube/floating_cube.h"
#include "effects/snake/snake.h"
#include "effects/corner_cube/corner_cube.h"
#include "effects/fog/fog.h"
#include "effects/sliding_walls/sliding_walls.h"
#include "effects/rain/rain.h"
#include "effects/wave2d/wave2d.h"
#include "effects/wave3d/wave3d.h"
#include "effects/straight_lines/straight_lines.h"
#include "effects/circles/circles.h"


static volatile uint8_t timer0_overflow_count = 0;
static volatile uint16_t milliseconds = 0;

static volatile uint64_t diodes_data_buffer = 0;
static volatile uint8_t active_layer = 0;  // 0 - 3

// seed
static volatile uint8_t s = 42;
static volatile uint8_t a = 42;

static volatile bool button_next_flag = false;
static volatile bool button_prev_flag = false;

#define NUMBER_OF_EFFECTS 11 // demo inclusive

#define DURATION_MULTIPLIER 5


uint8_t rnd() { // TODO optimize volatile usage
 	s^=s<<3;
 	s^=s>>5;
 	s^=a++>>2;
 	return s; // TODO check this later
}

uint16_t millis() {
	uint16_t m;
	ATOMIC_BLOCK (ATOMIC_FORCEON) {
		m = milliseconds;
	}
	return m;
}

ISR (EXT_INT0_vect) {
	button_prev_flag = true;
	s = TCNT0;
}

ISR (PCINT0_vect) {
	if ((PINA & _BV(BUTTON_NEXT)) == 0) {
		button_next_flag = true;
	}
	a = TCNT0;
}

ISR (TIM0_OVF_vect)    // Timer0 ISR
{
	uint8_t a_l = active_layer;
	spi_transfer(~(1 << a_l));

	uint8_t offset = a_l * 2 * 8;
	spi_transfer((diodes_data_buffer >> (offset + 8)));
	spi_transfer((diodes_data_buffer >> offset));
	latch();

	a_l++;
	if (a_l == 4) {
		a_l = 0;
	}
	active_layer = a_l;

	uint8_t t = timer0_overflow_count + 1;
	if (t == 10) { // Good Enough™ approximation
		timer0_overflow_count = 0;
		milliseconds++;
		} else {
		timer0_overflow_count = t;
	}
}

uint16_t transformed_duration(uint16_t duration) {
	uint32_t d_min;
	uint32_t d_max;
	uint16_t pot_min;
	const uint16_t pot_half = 1023 / 2 + 1;
	if (ADC >= pot_half) {
		d_min = duration;
		d_max = duration / DURATION_MULTIPLIER; // the more the less
		pot_min = pot_half;
	} else {
		d_min = duration * DURATION_MULTIPLIER;
		d_max = duration;
		pot_min = 0;
	}
	return (ADC - pot_min) * (int32_t)(d_max - d_min) / (1023 - pot_half) + d_min;
}

/*
* returns 1 if the next_button, -1 if the prev_button was pressed in the meantime
*/
int8_t render_frame(const uint64_t& data, uint16_t duration) {
	static uint16_t last_button_prev_press_time = 0;
	static uint16_t last_button_next_press_time = 0;

	ATOMIC_BLOCK (ATOMIC_FORCEON) {
		diodes_data_buffer = data;
	}

	const uint16_t frame_start_time = millis();
	
	do {
		// Read potentiometer
		if ((ADCSRA & _BV(ADSC)) == 0) {
			ADCSRA |= _BV(ADSC);
		}
		// Check buttons
		if (button_prev_flag) {
			button_prev_flag = false;
			if (millis() - last_button_prev_press_time > 200) {
				last_button_prev_press_time = millis();
				return -1;
			}
		}
		if (button_next_flag) {
			button_next_flag = false;
			if (millis() - last_button_next_press_time > 200) {
				last_button_next_press_time = millis();
				return 1;
			}
		}
	} while(millis() - frame_start_time < transformed_duration(duration));
	return 0;
}

int8_t demo_mode() {
	uint8_t current_mode = 0;
	while (true) {
		int8_t result = 0;
		switch (current_mode) {
			case 0:
				result += floating_cube(21);
				break;
			case 1:
				result += snake(42);
				break;
			case 2:
				result += corner_cube(25);
				break;
			case 3:
				result += fog(384);
				break;
			case 4:
				result += sliding_walls(38);
				break;
			case 5:
				result += rain(60);
				break;
			case 6:
				result += wave2d(88);
				break;
			case 7:
				result += wave3d(88);
				break;
			case 8:
				result += straight_lines(112);
				break;
			case 9:
				result += circles(600);
				break;
		}
		if (result != 0) {
			return result;
			} else {
			current_mode++;
		}
	}
}

int main(void)
{
	// ADC init
	ADMUX = _BV(MUX1) | _BV(MUX2);
	ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2);
	while (ADCSRA & _BV(ADSC));
	uint8_t seed_1 = ADCL;
	ADCSRA |= _BV(ADSC);
	while (ADCSRA & _BV(ADSC));
	uint8_t seed_2 = ADCL;
	// change to potentiometer
	ADMUX &= ~_BV(MUX2);
	ADCSRA |= _BV(ADSC);
	
	// IO init
	DDRA |=_BV(MR) | _BV(LATCH) | _BV(SCK_) | _BV(DATA); // MR - low
	PORTA |= _BV(MR) | _BV(_DI) | _BV(BUTTON_NEXT);
	PORTB |=_BV(BUTTON_PREV);
	latch();
	DDRA |= _BV(OE);

	// Timer0 init (8bit)
	TCCR0A = 0;
	TCCR0B = _BV(CS01);
	TIMSK0 = _BV(TOIE0);
	
	// buttons' interrupts init
	MCUCR = _BV(ISC01);
	PCMSK0 =_BV(PCINT7);
	GIMSK = _BV(INT0) | _BV(PCIE0);
	
	// read potentiometer
	while (ADCSRA & _BV(ADSC));
	uint8_t pot = ADCL;
	a = seed_1 + pot;
	s = seed_2 - pot;

	uint8_t current_effect = 0;
	
	while (1) {
		switch (current_effect) {
			case 0:
				current_effect += demo_mode();
				break;
			case 1:
				current_effect += floating_cube();
				break;
			case 2:
				current_effect += snake();
				break;
			case 3:
				current_effect += corner_cube();
				break;
			case 4:
				current_effect += fog();
				break;
			case 5:
				current_effect += sliding_walls();
				break;
			case 6:
				current_effect += rain();
				break;
			case 7:
				current_effect += wave2d();
				break;
			case 8:
				current_effect += wave3d();
				break;
			case 9:
				current_effect += straight_lines();
				break;
			case 10:
				current_effect += circles();
				break;
		};
		if (current_effect == 255) {
			current_effect = NUMBER_OF_EFFECTS - 1;
		} else if (current_effect == NUMBER_OF_EFFECTS) {
			current_effect = 0;
		}
	}
}

