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
static const int8_t wavetable [1][65] = 
{
	// The first entry repeats in the last so that "phasor + 1" lookup is valid.
#if 0	
	// sine:
	{127,139,152,164,176,187,198,208,217,225,233,239,244,249,252,253,254,253,252,249,244,239,233,225,217,208,198,187,176,164,152,139,
	 127,115,102, 90, 78, 67, 56, 46, 37, 29, 21, 15, 10,  5,  2,  1,  0,  1,  2,  5, 10, 15, 21, 29, 37, 46, 56, 67, 78, 90,102,115,127},
#else
	{0, 12, 25, 37, 49, 60, 71, 81, 90,	98, 106, 112, 117, 122, 125, 126, 127, 126, 125, 122, 117, 112, 106, 98, 90, 81, 71, 60, 49, 37, 25, 12, 0,
	-12,-25,-37,-49,-60,-71,-81,-90,-98,-106,-112,-117,-122,-125,-126,-127,-126,-125,-122,-117,-112,-106,-98,-90,-81,-71,-60,-49,-37,-25,-12, 0	
	},
#endif	 
	 
};


// module-level variables
static uint16_t phasor = 0;
static uint16_t env = 0;



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
	
	// Wave table are 64 entries long, 
	// phasor is unsigned 16 bits
	// so need 6 msbs of phasor as index
	// and next 4 bits as interp value
	l = wavetable[0][phasor >> 10];
	r = wavetable[0][((phasor >> 10)+1)];
	dist = (phasor & 0x03c0) >> 6;
	
	return linterp16(l,r,dist);	
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
	phasor += 0x0100;
	//phasor += 0x0200;
	//phasor += (env >> 5);
	phasor += (env >> 6);
	
	//lookup value
	sample = waveLookup();
	
	// Calc next envelope
	// 16-bit fixed point multiply...
	// 16 times 16 makes 32 bits...with 16 bits in the MSBs.
	// IE: 0xffff * 0xfff0 = 0xffef0010
	uint32_t res;
	res = env * (uint32_t)0xfffeUL;
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

int main(void)
{
	
	volatile uint32_t pause;
	
	setupDebugPins();
	
	setupClocks();
	
	setupDAC();

	setupTimer0();
		
	// enable interrupts
	sei();
		
    while(1)
    {
        //count++;
		//writeDAC(count);

		// canary in coalmine idle wiggle
		PINB = 0x02;
		
//		for(pause = 0; pause < 0x1fffff; pause++)
		for(pause = 0; pause < 0x07ffff; pause++)
		{
			PINB = 0x02;
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