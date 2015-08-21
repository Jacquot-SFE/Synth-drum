/*
 * synth_drum.cpp
 *
 * Created: 7/10/2015 1:16:42 PM
 *  Author: byron.jacquot
 */ 

/*
 * port pin mapping?
 *
 * Rudimentary for now...
 *
 * PortA
 * portB
 * portC = Arduino analog inputs 0 - 5...
 * portD = Parallel outs for R2R DAC...but it'll cost me the hardware serial...
 *
 */
 


#include <avr/io.h>
#include <avr/interrupt.h>


// constants
static const int8_t wavetable [3][129] = 
{
	// The first entry repeats in the last so that "phasor + 1" lookup is valid.
// Sine
	{0,6,12,19,25,31,37,43,49,54,60,65,71,76,81,85,90,94,98,102,106,109,112,115,117,120,122,123,125,126,126,127,127,
		127,126,126,125,123,122,120,117,115,112,109,106,102,98,94,90,85,81,76,71,65,60,54,49,43,37,31,25,19,12,6,
		0,-6,-12,-19,-25,-31,-37,-43,-49,-54,-60,-65,-71,-76,-81,-85,-90,-94,-98,-102,-106,-109,-112,-115,-117,-120,-122,-123,-125,-126,-126,-127,-127,
		-127,-126,-126,-125,-123,-122,-120,-117,-115,-112,-109,-106,-102,-98,-94,-90,-85,-81,-76,-71,-65,-60,-54,-49,-43,-37,-31,-25,-19,-12,-6,0},
// sine octave up AKA - the old, short table doubled up
	{0, 12, 25, 37, 49, 60, 71, 81, 90,	98, 106, 112, 117, 122, 125, 126, 127, 126, 125, 122, 117, 112, 106, 98, 90, 81, 71, 60, 49, 37, 25, 12, 0,
		-12,-25,-37,-49,-60,-71,-81,-90,-98,-106,-112,-117,-122,-125,-126,-127,-126,-125,-122,-117,-112,-106,-98,-90,-81,-71,-60,-49,-37,-25,-12,
		0, 12, 25, 37, 49, 60, 71, 81, 90,	98, 106, 112, 117, 122, 125, 126, 127, 126, 125, 122, 117, 112, 106, 98, 90, 81, 71, 60, 49, 37, 25, 12, 0,
		-12,-25,-37,-49,-60,-71,-81,-90,-98,-106,-112,-117,-122,-125,-126,-127,-126,-125,-122,-117,-112,-106,-98,-90,-81,-71,-60,-49,-37,-25,-12,0	
	},
//tri
	{
	0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,67,71,75,79,83,87,91,95,99,103,107,111,115,119,123,127,
	123,119,115,111,107,103,99,95,91,87,83,79,75,71,67,64,60,56,52,48,44,40,36,32,28,24,20,16,12,8,4,
	0,-4,-8,-12,-16,-20,-24,-28,-32,-36,-40,-44,-48,-52,-56,-60,-64,-67,-71,-75,-79,-83,-87,-91,-95,-99,-103,-107,-111,-115,-119,-123,-127,
	-123,-119,-115,-111,-107,-103,-99,-95,-91,-87,-83,-79,-75,-71,-67,-64,-60,-56,-52,-48,-44,-40,-36,-32,-28,-24,-20,-16,-12,-8,-4,0
	}
//saw	
	 
};


// module-level variables
static uint16_t phasor = 0;
static uint16_t env = 0;

static uint8_t pot_data[6];

// If I'm wiggling pins for debug, configure them here...
void setupDebugPins()
{
	// PB0 = Arduino 8
	// PB1 = Arduino 9
	DDRB = 0x03;
}

#if 1
// do 4 extra bits of linear interpolation between a and b.
int16_t linterp16(int16_t a,
                  int16_t b,
                  int8_t dist)
{
    volatile int16_t window;

    window = (int16_t)b - (int16_t)a ;

    // Out of order to maintain precision: mul then div...
    // mul by the number of intermediate steps ( 4 bits: 0 to 15)
    //then div by 16
    window *= dist;

    window >>= 4;

    return a+window;
}
#endif

#if 0
uint8_t linterp8(uint8_t a, uint8_t b, uint8_t dist)
{
	int16_t window = b - a;
	
	window *= dist;
	
	window >>= 4;
	
	return window;
}
#endif

// Functionalized and modular so it can get broken up
// across port pins without too much trouble.
void writeDAC(uint8_t data_out)
{
	PORTD = data_out;
}

void setupDAC()
{
	// set a 1 to enable as output
	DDRD = 0xff;
}

void setupClocks()
{
	// Configure clock scaling.
	// 2-step procedure to keep stray pointers from mangling the value.
	// Using 16 mHz resonator, we'll divide by 2 for 8 MHz.
	
	CLKPR = 0x80;
	CLKPR = 0x00; // clk div 2 = 8 MHz.
}

uint8_t waveLookup()
{
	int8_t l, r, dist;
	uint8_t wavesel;

	wavesel = pot_data[2] >> 6;
	
	// Wave table are 128 entries long, 
	// phasor is unsigned 16 bits
	// so need 7 msbs of phasor as index
	// and next 4 bits as interp value
	if(wavesel == 3)
	{
		// return top 8 of phasor as saw
		return(phasor>>8);
	}
	else
	{
		l = wavetable[wavesel][phasor >> 9];
		r = wavetable[wavesel][((phasor >> 9)+1)];
		dist = (phasor & 0x01e0) >> 5;
	
		return linterp16(l,r,dist);	
	}
}

ISR(TIMER0_COMPA_vect)
{
	// Source signal is Cleared automatically when vector called.
		
	// Pin wiggle for debug...
	// writing 1's to PIN reg toggles pin.
	PINB = 0x01;
	
	uint16_t math;
	int8_t  sample;
	
	// Increment phasor
//	phasor += 0x0080;
	phasor += (pot_data[0] << 3)+ 0x80;

	// Apply pitch envelope...
//	phasor += (env >> 6);
	uint16_t pitch_mod;
	pitch_mod = ((env >> 8) * pot_data[4])>> 4;
	phasor+= (pitch_mod);
	
	//lookup value
	sample = waveLookup();
	
	// Calc next envelope
	// 16-bit fixed point multiply...
	// 16 times 16 makes 32 bits...with 16 bits in the MSBs.
	// IE: 0xffff * 0xfff0 = 0xffef0010
	uint32_t res;
	res = env * (uint32_t)(0xff00UL + pot_data[3]);
	env = res >> 16;

	// Apply envelope to latest sample.
	math = ( sample * (env >> 8));		
	// write the value to the output
#if 1
	writeDAC((math >> 8) + 128);
#else
	
	writeDAC(sample + 128);
#endif

	PINB = 0x01;
}

void setupTimer0()
{
	// 20 KHZ = 16MHZ / 800;
	//  but with timer prescale. we can reduce that.
	// 16MHZ / 8 = 2 MHZ.
	// 2MHz / 100 = 20KHz.
	
	// Pins disconnected, 
	TCCR0A = (1 << WGM01); // Clear on match
	TCCR0B = (1 << CS01);  // Clk sel as div by 8.
	
	TCNT0 = 0; // start at zero
	OCR0A = 99; // count to 100.
	
	TIMSK0 = 0x02; // interrupt on match A
	
}

void setupADC()
{
	
}

uint8_t readADC(uint8_t channel)
{
	uint16_t value;
	
	// only allow the pins we've selected to read...
	if(channel >= 6)
	{
		// catch for attempt to read invalid channel.
		while(1);
	}
	
	// turn on adc
	// TODO: perhaps turn on and leave on?
	ADCSRA = 0x86; // power bit, prescale of 64
	
	ADCSRB |= 0x00; // not option to left justify
	
	// set digital input disable
	DIDR0 = 0x3f;
	
	// input mux, vref selection
	// Left justified, AVcc reference
	ADMUX = channel | 0x60;

	// pause a moment...
//	for(volatile uint8_t i = 0xff; i != 0; i--);
	
	// bit 6 starts conversion
	// bit 4 clears IRQ flag
	ADCSRA |= 0x50;
	
	// start bit clears when complete
	// watching the interrupt flag
	while(ADCSRA & 0x40);

	value = ADCH;
	
    return (value);
	
}

int main(void)
{
	
	volatile uint32_t pause;
	
	setupDebugPins();
	
	setupClocks();
	
	setupDAC();
	
	setupADC();

	setupTimer0();
		
	// initialize panel data by reading all controls one time
	for(uint8_t idx = 0; idx < 6; idx++)
	{
		pot_data[idx] = readADC(idx);
	}
		
		
	// enable interrupts
	sei();
		
    while(1)
    {
        //count++;
		//writeDAC(count);

		// canary in coalmine idle wiggle
		PINB = 0x02;
		
//		for(pause = 0; pause < 0x1fffff; pause++)
		for(pause = 0; pause < 0x01fff/*f*/; pause++)
		{
			PINB = 0x02;
			
			uint8_t channel = pause%6;
			pot_data[channel] = readADC(channel);
			
			
			// We can't disable interrupts from ISR, because the ISR exit
			// re-enables them regardless.  So we'll watch the env, and when it's done, 
			// stop them.
			//
			// TODO: try it with a "naked" isr?
			if(env == 0x0)
			{
				cli();
			}
		}
			
		// for now, auto-trigger the envelope
		// By setting the peak level, and enabling ISRs
		env = 0xffff;
		phasor = 0 ;
		sei();
    }
}