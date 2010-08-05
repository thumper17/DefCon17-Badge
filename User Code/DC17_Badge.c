/******************************************************************************
*
* DEFCON 17 BADGE
*
* Filename:		  DC17_Badge.c
* Author:		    Joe Grand (Kingpin)
* Revision:		  1.0
* Last Updated: April 27, 2009
*
* Description:	Main File for the DEFCON 17 Badge (Freescale MC56F8006)
* Notes:
*
* See DC17_Badge.h for more inf0z...		
*******************************************************************************/

/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "PWMRed.h"
#include "PWMGreen.h"
#include "PWMBlue.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

#include "DC17_Badge.h"


/****************************************************************************
 ************************** Global variables ********************************
 ***************************************************************************/

// Badge 
badge_state_type badge_state = I2C; // begin in I2C (multi-badge) mode on power-up
badge_type_struct badge_type;
struct packed_flags flags;

// Microphone
unsigned int freqcount = 0;
word	micval = 0;
word  micval_offset;

// FFT
dfr16_tRFFTStruct RFFT;
dfr16_tRFFTStruct *pRFFT = &RFFT;
CFrac16 FFTInplaceBuf[MAX_RFFT_LEN/2];
unsigned long FFTVal[3];

// A/D
unsigned int AdcBuffer[ADC_MAX_BUFFER_SIZE];
unsigned int AdcWriteIndex = 0;
unsigned int AdcReadIndex = 0;

// LED
led_state_type LEDstate = RED;
unsigned int LEDptr = 0;

// RNG
//unsigned int randtime = 0;
//int randval; 

// Sleep
byte threshold_failure;
word threshold_count;

// Modes
unsigned int soscount = 0;
unsigned int dancetmr = 0; 


/****************************************************************************
 ************************** Functions ***************************************
 ***************************************************************************/

void dc17_badge(void)
{ 
	dc17_init();
  
	// check to see if badges are connected to each other and, if so, enter badge-to-badge mode (master)
  // this happens only once on power-up
  // every slave badge that is connected to the bus *must* be powered on or the bus will fail like a whale
	if (badge_type == HUMAN) dc17_i2c_check(); 
  
  // begin normal operation
  badge_state = FFT; // enable FFT-related functions in the interrupts
  while(1)
  {  	
  	if (badge_type != HUMAN) if (flags.i2c_rx) dc17_i2c_slave(); // if i2c data packet received, enter badge-to-badge mode (slave)
  
    dc17_fft();
    dc17_process_fft();
    dc17_sleep();
  }
}


/**************************************************************/

void dc17_fft(void) // do tha' FFT
{
  unsigned int  i;
 	Result 				res;
 	
  unsigned long   Temp1, Temp2;    

 	if (flags.PerformFFT == true)
  {      
   	// perform the FFT (N = 8)
    res = dfr16RFFT (pRFFT, (Frac16 *)&AdcBuffer[AdcReadIndex], (dfr16_sInplaceCRFFT *)&FFTInplaceBuf[0]);

    for (i = 1; i <= ((MAX_RFFT_LEN/2) - 1); i++) // ignore bin 0
    {
      // calculate the power of the ith element (bin) of the FFT
      // M = sqrt(real^2 + imag^2)
    	// P = M^2
 	   	// since the power of a frequency element of the FFT is the square of the magnitude, we can just skip
		  // the square root while calculating the magnitude and we're left with the power. 
			// we ignore the phase component of the FFT.     	
      Temp1 = abs_s(FFTInplaceBuf[i].real);
      Temp1 = Temp1*Temp1;
        
      Temp2 = abs_s(FFTInplaceBuf[i].imag);
      FFTVal[i-1] = (Temp2*Temp2) + Temp1;
		}	
		
		/*Term1_SendStr("\n\r0: ");
		Term1_SendNum((long)FFTVal[0]);
		Term1_SendStr("\t1: ");
		Term1_SendNum((long)FFTVal[1]);
		Term1_SendStr("\t2: ");
		Term1_SendNum((long)FFTVal[2]);*/

    AdcReadIndex += MAX_RFFT_LEN;

    if (AdcReadIndex >= ADC_MAX_BUFFER_SIZE)
    {
     	AdcReadIndex = 0;
    }

    flags.PerformFFT = false;
  }
}


void dc17_process_fft(void) // do stuff based on the calculated FFT data
{
	unsigned char i;

	// if sound energy is greater than our sleep threshold...
	if ((FFTVal[0] > SLEEP_THRESH) || (FFTVal[1] > SLEEP_THRESH) || (FFTVal[2] > SLEEP_THRESH))
	{
		threshold_failure++; // increment counter
 		flags.sleep = 0;		 // stay awake
		TI1_Enable();				 // make sure TMR1 is only enabled when the device is completely awake (to avoid flickering LED while sleeping)
	}
	
	if (!flags.sleep) // if we're awake
	{
		// super secret frequency detection code
		if ((FFTVal[0] > (FFTVal[1] << 4)) && (FFTVal[0] > (FFTVal[2] << 4))) 	// if bin 0 is an order of magnitude larger than the other bins
		{
			++freqcount;
			if (freqcount == 42)
			{
				morse((char *)sec);
				freqcount = 0;
			}
		}
		else
			freqcount = 0;
				
		// choose our operating mode	
		// if we are in a loud environment (like a disco!), enter dance mode
		if (((FFTVal[0] > DANCE_THRESH) && (FFTVal[1] > DANCE_THRESH) && (FFTVal[2] > DANCE_THRESH)) || dancetmr) // all bins must be above the threshold to enter the mode
		{
		  // hysteresis to stay in dance mode for some amount of time after we go below the threshold
		  if (dancetmr) 
		    dancetmr--;
		  else 							// dancetmr = 0, so we've just entered this mode
		  	dancetmr = DANCE_TMR;	// set our timer

			if ((FFTVal[0] > (DANCE_THRESH >> 2)) || (FFTVal[1] > (DANCE_THRESH >> 2)) || (FFTVal[2] > (DANCE_THRESH >> 2)))
		  	dancetmr = DANCE_TMR; // reset the timer if any one bin has met grester than 1/2 threshold	 
		  	    
			for (i = 0; i < 3; ++i)
			{
			  FFTVal[i] >>= 2; // scale FFT value down to something more reasonable (we lose resolution at lower volumes, but that's OK, we're at the disco!)
				if (FFTVal[i] > 0xB000) FFTVal[i] = 0xB000; // and limit the maximum brightness
			}	
		
			// set LED colors and brightness based on input frequency			
			PWMRed_SetRatio16(~(unsigned int)FFTVal[0]); 
			PWMGreen_SetRatio16(~(unsigned int)FFTVal[1]); 
			PWMBlue_SetRatio16(~(unsigned int)FFTVal[2]); 
		
		  // if we've been in this mode for some amount of time, put out a mayday call (per DT!)
		  if (soscount >= SOS_TMR)
		  {
				morse("SOS");
				morse("SOS");
				morse("SOS");
				soscount = 0;				  
		  }	  
		}
		else 	// do cool RGB blending...
		{
			PWMRed_SetRatio16(~(LEDR[LEDptr>>2]));
		  PWMGreen_SetRatio16(~(LEDG[LEDptr>>2]));
		  PWMBlue_SetRatio16(~(LEDB[LEDptr>>2]));

			dancetmr = 0;
		  soscount = 0;
		}			
  }
}


void dc17_sleep(void) // sleep, if required
{
  unsigned long   Temp1, Temp2;    

	if (flags.sleep) // if it's time to go to sleep or continue to sleep...
	{
    // disable peripherals
   	PWMRed_Disable();
		PWMGreen_Disable();
		PWMBlue_Disable();
		MICOUT_Disable();
		TI0_Disable();
		clrRegBit(GPIO_A_PER, PE0);
		clrRegBit(GPIO_A_PER, PE1);
    clrRegBit(GPIO_A_PER, PE2);
		TI1_Disable();
		BADGEI2C_Disable();
				
		// save original PIT timing values
		Temp1 = getReg(PIT_MOD);
		Temp2 = getReg(PIT_CTRL);
				
		// set PIT to interrupt in 1 second to check for sound
		setReg(PIT_MOD, 62500); 
		setRegBitGroup(PIT_CTRL, PRESCALER, 6); 

		Cpu_SetStopMode(); // Zzzzz...
						
		// restore original PIT timing values	    
	  setReg(PIT_MOD, Temp1);
	  setReg(PIT_CTRL, Temp2);
	    
 		threshold_count = 0;
 		threshold_failure = 0;
	 	PWMRed_Enable(); 		
		PWMGreen_Enable();
		PWMBlue_Enable();	 			
		MICOUT_Enable();
		TI0_Enable();
		BADGEI2C_Enable();				
	}

	Cpu_SetWaitMode();  // snooze in reduced-power wait mode until the next interrupt
}

/**************************************************************/

void dc17_i2c_check(void)
{
	Result 					res;
	word						i2cret; // return value from i2c function call	
  byte						tmp[6]; // temporary buffer for i2c tx/rx
  
  // send a test broadcast packet to see if we receive a response back from any badges
	flags.i2c_nack = 0;
     
	tmp[0] = 'D';
	tmp[1] = 'E'; 
	tmp[2] = 'F';
	tmp[3] = 'C'; 
	tmp[4] = 'O';
	tmp[5] = 'N';
	BADGEI2C_SelectSlave(0);	// set slave address to broadcast (0)	
	res = BADGEI2C_SendBlock(&tmp, 6, &i2cret);		  
	delay_ms(1);  
	if (!flags.i2c_nack) // if NACK flag not set, then other badges must be connected
	{
	  //Term1_SendStr("Multi-Badge Master.\n\r");

 		// let's SEND data...
		while(1) // ...forever!
		{
		  while (!flags.ptr_update){}; // wait until the RGB blending pointer has been updated...
			
			// do a neat LED animation to led people know we're in multi-badge mode
   		switch (LEDptr % 8)
  		{		
  			case 0:
				 	PWMRed_SetRatio16(OFF);
				 	PWMGreen_SetRatio16(OFF);
				 	PWMBlue_SetRatio16(OFF);
				 	tmp[0] = tmp[1] = tmp[2] = tmp[3] = tmp[4] = tmp[5] = 0xFF; // simple way to set the bytes to transfer
					break;
  			case 1:
				 	PWMRed_SetRatio16(OFF);
				 	PWMGreen_SetRatio16(OFF);
				 	PWMBlue_SetRatio16(ON);
				 	tmp[0] = tmp[1] = tmp[2] = tmp[3] = tmp[5] = 0xFF;
					tmp[4] = 0x4F;
				 	break;
  			case 2:
				 	PWMRed_SetRatio16(OFF);
				 	PWMGreen_SetRatio16(ON);
				 	PWMBlue_SetRatio16(ON);
				 	tmp[0] = tmp[1] = tmp[3] = tmp[5] = 0xFF;
					tmp[2] = tmp[4] = 0x4F;
				 	break;
  			case 3:
				 	PWMRed_SetRatio16(OFF);
				 	PWMGreen_SetRatio16(ON);
				 	PWMBlue_SetRatio16(OFF); 
				 	tmp[0] = tmp[1] = tmp[3] = tmp[4] = tmp[5] = 0xFF;
					tmp[2] = 0x4F;
  				break;
  			case 4:
				 	PWMRed_SetRatio16(ON);
				 	PWMGreen_SetRatio16(ON);
				 	PWMBlue_SetRatio16(OFF);  			
				 	tmp[1] = tmp[3] = tmp[4] = tmp[5] = 0xFF;
					tmp[0] = tmp[2] = 0x4F;
  				break;
  			case 5:
				 	PWMRed_SetRatio16(ON);
				 	PWMGreen_SetRatio16(OFF);
				 	PWMBlue_SetRatio16(OFF);  			
				 	tmp[1] = tmp[2] = tmp[3] = tmp[4] = tmp[5] = 0xFF;
					tmp[0] = 0x4F;
  				break;
  			case 6:
				 	PWMRed_SetRatio16(ON);
				 	PWMGreen_SetRatio16(OFF);
				 	PWMBlue_SetRatio16(ON);  			
				 	tmp[1] = tmp[2] = tmp[3] = tmp[5] = 0xFF;
					tmp[0] = tmp[4] = 0x4F;
  				break; 				
  			case 7:
				 	PWMRed_SetRatio16(ON);
				 	PWMGreen_SetRatio16(ON);
				 	PWMBlue_SetRatio16(ON);  			
				 	tmp[1] = tmp[3] = tmp[5] = 0xFF;
					tmp[0] = tmp[2] = tmp[4] = 0x4F;
  				break; 
 			}      
			
			//BADGEI2C_SelectSlave(UBER); // use this function to direct all subsequent calls to a particular slave device (instead of broadcasting)
		  res = BADGEI2C_SendBlock(&tmp, 6, &i2cret); // broadcast PWM values to all the other badges	
		  if (res != ERR_OK) // if transmission is not successful, restart I2C
		  {
				dc17_i2c_init();
		  }
		  
		  delay_ms(100);
  	  flags.ptr_update = 0;			
		}
	}
}


void dc17_i2c_slave(void)
{
	Result 					res;
	word						i2cret; // return value from i2c function call	
  byte						tmp[6]; // temporary buffer for i2c tx/rx
	unsigned int		tmpw;

  badge_state = I2C;
	res = BADGEI2C_RecvBlock(&tmp, 6, &i2cret); // first data packet can be ignored
	flags.i2c_rx = 0;
	
	//Term1_SendStr("Multi-Badge Slave.\n\r");		
	
	// let's RECEIVE data...
	while(1) // ...forever!
	{
		while (!flags.i2c_rx){};

	  res = BADGEI2C_RecvBlock(&tmp, 6, &i2cret); // receive the bytes out of the buffer
	  if (res == ERR_OK)
	  {
			// convert into 16-bit values and set the PWM registers
		  tmpw = tmp[0];
		  tmpw <<= 8;
		  tmpw |= tmp[1];
		 	PWMRed_SetRatio16((unsigned int)tmpw);

		  tmpw = tmp[2];
		  tmpw <<= 8;
		  tmpw |= tmp[3];
			PWMGreen_SetRatio16((unsigned int)tmpw);

		  tmpw = tmp[4];
		  tmpw <<= 8;
		  tmpw |= tmp[5];
		 	PWMBlue_SetRatio16((unsigned int)tmpw);
	 	}
	  
	  flags.i2c_rx = 0;	
	}
}


void dc17_i2c_init(void) // initialize the I2C system for multi-badge communication
{
  unsigned char i;
	
	BADGEI2C_Disable(); // disable I2C module during configuration
	
	// initialize global flags
	flags.i2c_rx = 0;
	flags.i2c_nack = 0;
	
  // determine what type of badge I am (based on A2-A0 input values)
  i = 0;
  if (A2_GetVal()) i |= 1;
  i <<= 1;
  if (A1_GetVal()) i |= 1;
  i <<= 1;
  if (A0_GetVal()) i |= 1;
  i++;
  
  badge_type = (badge_type_struct)i;
  setReg(I2C_ADDR, (i << 1) & I2C_ADDR_AD_MASK); // set I2C slave address (upper 7 bits of register), must be non-zero  

	if (badge_type == HUMAN)
	{
	 	BADGEI2C_SetMode(TRUE); // human is always the master...
	 	
		// Set General Call Address - all subsequent calls of SendChar, SendBlock will be broadcast on the bus
		BADGEI2C_SelectSpecialCommand(0);
	}
  else
  {
   	BADGEI2C_SetMode(FALSE); // ...otherwise, we are a slave
  }

  /*Term1_SendStr("I am ");
  switch (badge_type)
  {
  	case UBER:
  		Term1_SendStr("Uber");
  		break;
  	case GOON:
  		Term1_SendStr("Goon");	
  		break;
  	case CONTEST:
  		Term1_SendStr("Contest");  	
  		break;
  	case SPEAKER:
  		Term1_SendStr("Speaker");  	
  		break;
  	case VENDOR:
  		Term1_SendStr("Vendor");  	
  		break;
  	case PRESS:
  		Term1_SendStr("Press");  	
  		break;
  	case HUMAN:
  		Term1_SendStr("Human");  	
  		break;
  	default:
  	  Term1_SendStr("Unused");
  	  break;
  }
	Term1_SendStr(".\n\r");*/

	BADGEI2C_Enable(); // enable I2C module
}


/**************************************************************/

void dc17_init(void) // badge start-up/initialization
{
  unsigned int 		options = FFT_SCALE_RESULTS_BY_N; // unconditionally scale down the results by n

	Cpu_DisableInt(); // disable global interrupts 	
	PWMRed_Enable();  // while we enable all the modules
	PWMGreen_Enable();
	PWMBlue_Enable();
	MICOUT_Enable();

	Term1_SendStr("Welcome to the DEFCON 17 Badge.\n\n\r");

  // take a listen at the current ambient noise
	/*MICOUT_Measure(TRUE); // number of conversions specified in ADC bean, wait for conversion to complete
	MICOUT_GetValue16(&micval); // get a/d sample  
	srand((unsigned int)micval); // ...seed the PRNG with the value */
  		
	TI0_Enable();
	TI1_Enable();
	Cpu_EnableInt();  // re-enable global interrupts when we're done and ready for action
 		
	// initialize global flags
	flags.PerformFFT = FALSE;
	flags.ptr_update = 0;
	flags.sleep = 0;
	  
  threshold_failure = 0; // counters for auto-power-off
  threshold_count = 0;
	
  // initialize the FFT algorithm
  dfr16RFFTInit (pRFFT, 8, options); // MAX_RFFT_LEN

	// initialize the I2C system for multi-badge communication
	dc17_i2c_init();
}



/**************************************************************
/* INTERRUPT SERVICE ROUTINES
/**************************************************************/

void dc17_pit_isr(void) // Programmable Interval Timer (PIT): every 8ms, set to operate during Stop Mode
{  	   	
  if (badge_state == FFT)
  {
			// update the randomizer...  
			/*randtime++;
			if (randtime >= RAND_TMR) 
			{
				randval = rand() % 3; 
				randtime = 0;
			}*/
			
			// if noise energy is below our threshold for some amount of time, then go to sleep
 			if (threshold_failure == 0)
 			{
 				threshold_count++;
 			}
 			else
 			{
 				threshold_count = 0;
 				threshold_failure = 0;
 			}
 		
 			if (threshold_count > SLEEP_TMR)
 				flags.sleep = 1;
 				
 			// increment counter for dance mode
 			soscount++; 
 	}
 	
  // LEDptr used for RGB pulsing and blending tables
	LEDptr++; // increment the pointer
	flags.ptr_update = 1;

	if (LEDptr >= LED_TabLength)  // if we've reached the end of the table, start again
		LEDptr = 0;  	
}


void dc17_t0_isr(void) // TMR0: A/D sampling, 8kHz (every 0.125ms) (Fmax = highest frequency that can be analyzed = Fs / 2 = 4kHz)
{
  if (badge_state == FFT)
  {  
		// get a/d sample and add it to our circular buffer
		MICOUT_Measure(TRUE); // number of conversions specified in ADC bean, wait for conversion to complete
		MICOUT_GetValue16(&micval);  	   
	  AdcBuffer[AdcWriteIndex] = micval;
	  AdcWriteIndex++;
	  
	  if (AdcWriteIndex >= ADC_MAX_BUFFER_SIZE) 
	  	AdcWriteIndex = 0; // wrap around to beginning of buffer
	  		
 		// check to see if it's time to do an FFT (e.g., we have enough new A/D samples)
    if (AdcReadIndex > AdcWriteIndex)
  	{
  		flags.PerformFFT = true;
  	}
  	else if (AdcReadIndex <= (int)(AdcWriteIndex - MAX_RFFT_LEN))
  	{
    	flags.PerformFFT = true;
  	}
  }
}


void dc17_t1_isr(void) // TMR1: RGB multiplexing, every 1ms
{
		switch (LEDstate)
		{
				case RED:
					setRegBit(GPIO_A_PER, PE0);
  	  		clrRegBit(GPIO_A_PER, PE1);
  	  		clrRegBit(GPIO_A_PER, PE2);
  				LEDstate = GREEN;
					break;
				case GREEN:
					clrRegBit(GPIO_A_PER, PE0);
  	  		setRegBit(GPIO_A_PER, PE1);
  	  		clrRegBit(GPIO_A_PER, PE2);
  				LEDstate = BLUE;					
  				break;
				case BLUE:
					clrRegBit(GPIO_A_PER, PE0);
  	  		clrRegBit(GPIO_A_PER, PE1);
  	  		setRegBit(GPIO_A_PER, PE2);
  				LEDstate = RED;				
					break;
		}
}

/**************************************************************
*	Function: 	morse
*	Parameters: 16-bit delay value
*	Return:		none
*
*	Transmit a string in Morse code
*
* This function transmits the string pointed to by the cp 
* argument in Morse code via the RGB LED. 
* 
* The transmission speed is set by the UNIT_TIME constant. 
* 
* A static table translates from ASCII to Morse code. Each 
* entry is an unsigned integer, where a zero represents a dot 
* and a one represents a dash. No more than 14 bits may be used.
* Setting bit 15 produces a word space, regardless of any other 
* pattern.
* 
* The Morse code pattern is taken from bit 0, and is shifted 
* right each time an element is sent. A special "marker bit" 
* follows the complete Morse pattern. This marker bit is tested 
* before transmitting each bit; if there are no 1's in bits 
* 1..15, the complete character has been sent.
* 
* For example, an "L" (dit-dah-dit-dit) would be:
* 0000000000010010, with bit zero containing the first dot, 
* bit one the dash, etc.  The marker bit is in bit 4.
*
* Based on code written by Michael M. Dodd, N4CF
*
**************************************************************/

void morse(char *cp)
{  
	unsigned int c;

	TIPIT_Disable(); 	// disable PIT to prevent blending
  TI0_Disable(); 		// disable A/D/microphone input
  	
  PWMRed_SetRatio16(OFF);
  PWMGreen_SetRatio16(OFF);
  PWMBlue_SetRatio16(OFF);
  	
  while ((c = *cp++) != '\0')	 // transmit string
  {  
  	//c = (unsigned int)toupper((int)c); // convert all text to uppercase - there are no lower-case Morse characters
    c -= ' ';                  // adjust for zero-based table

    if (c < 0 || c > 58)       // if character out of range, ignore it
    	continue;

    c = codes[c];              // look up Morse pattern from table

    if (c & SPACE_MASK)        // if the space bit is set...
    {    
    	word_space(); 					 //...then send a word space and continue
      continue;
    }

    while (c & BIT_MASK)       // transmit a character...
    {
    	// ...one bit at a time
     	if (c & 1)
       	send_dash();
      else  
       	send_dot();

      c >>= 1;
    }

    ltr_space();               // send a letter space

  } 
  
  TIPIT_Enable(); 	// re-enable
  TI0_Enable();
}

/*
** These functions transmit a dot, a dash, a letter space, and a
** word space.
** 
** Note that a single unit space is automatically transmitted after
** each dot or dash, so the ltr_space() function produces only a
** two-unit pause.
** 
** Also, the word_space() function produces only a four-unit pause
** because the three-unit letter space has already occurred following
** the previous letter.
*/

void send_dot(void)				// dot and space
{
  	PWMRed_SetRatio16(ON);
   	PWMGreen_SetRatio16(ON);
  	PWMBlue_SetRatio16(ON);
  
    delay_ms(UNIT_TIME);
    
  	PWMRed_SetRatio16(OFF);
   	PWMGreen_SetRatio16(OFF);
  	PWMBlue_SetRatio16(OFF);
  
    delay_ms(UNIT_TIME);
}

void send_dash(void)      // dash and space
{
  	PWMRed_SetRatio16(ON);
   	PWMGreen_SetRatio16(ON);
  	PWMBlue_SetRatio16(ON);
  	
  	delay_ms(UNIT_TIME * 3);
    
  	PWMRed_SetRatio16(OFF);
   	PWMGreen_SetRatio16(OFF);
  	PWMBlue_SetRatio16(OFF);
  	
  	delay_ms(UNIT_TIME);
}

void ltr_space(void)      // letter space
{
      delay_ms(UNIT_TIME * 3);
}

void word_space(void)     // word space
{
      delay_ms(UNIT_TIME * 7);
}


/**************************************************************
*	Function: 	delay
*	Parameters: 16-bit delay value
*	Return:		none
*
*	Simple delay loops
* These are not very accurate because they could get interrupted
**************************************************************/
 
void delay_ms(unsigned int ms)  // delay the specified number of milliseconds
{
  unsigned int i;

  for (i = 0; i < ms; ++i)
  	Cpu_Delay100US(10);
}

/****************************** END OF FILE **********************************/

