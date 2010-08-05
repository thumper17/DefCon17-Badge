/** ###################################################################
**     Filename  : DC17.C
**     Project   : DC17
**     Processor : MC56F8006_32_LQFP
**     Version   : Driver 01.13
**     Compiler  : Metrowerks DSP C Compiler
**     Date/Time : 1/18/2009, 1:37 PM
**     Abstract  :
**         Main module.
**         This module contains user's application code.
**     Settings  :
**     Contents  :
**         No public methods
**
**     (c) Copyright UNIS, a.s. 1997-2008
**     UNIS, a.s.
**     Jundrovska 33
**     624 00 Brno
**     Czech Republic
**     http      : www.processorexpert.com
**     mail      : info@processorexpert.com
** ###################################################################*/
/* MODULE DC17 */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "PWMRed.h"
#include "PWMGreen.h"
#include "PWMBlue.h"
#include "MICOUT.h"
#include "BADGEI2C.h"
#include "MEM1.h"
#include "A0.h"
#include "A1.h"
#include "A2.h"
#include "Term1.h"
#include "Inhr6.h"
#include "TIPIT.h"
#include "TI1.h"
#include "PORTAOUT.h"
#include "PORTB1OUT.h"
#include "PORTB5OUT.h"
#include "PORTCIN.h"
#include "PORTCOUT.h"
#include "MICPWR.h"
#include "TI0.h"
#include "PORTFOUT.h"
#include "DFR1.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "DC17_Badge.h"

void main(void)
{
  extern word _vba;
    
  INTC_VBA = ((word)&_vba) >> 7;	// restore VBA register to point to user defined vector table
  
  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  dc17_badge(); // all this main.c file used to do was set up the CPU and then call this function
  while(1){};
}


/*
** ###################################################################
**
**     This file was created by UNIS Processor Expert 2.99 [04.17]
**     for the Freescale 56800 series of microcontrollers.
**
** ###################################################################
*/
