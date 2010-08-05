/** ###################################################################
**     THIS BEAN MODULE IS GENERATED BY THE TOOL WITH OPTION "Preserve User Changes".
**     IT'S NOT RECOMMENDED TO MODIFY IT, BUT THE TOOL ACCEPTS CHANGES IN THIS MODE.
**     Filename  : SCI1.C
**     Project   : static_bootloader
**     Processor : MC56F8006_32_LQFP
**     Beantype  : Init_SCI
**     Version   : Bean 01.077, Driver 01.17, CPU db: 3.00.169
**     Compiler  : Metrowerks DSP C Compiler
**     Date/Time : 4/6/2009, 11:56 AM
**     Abstract  :
**         This "Init_SCI" Peripheral Inspector implements the
**         Serial Communications Interface module (SCI), basic
**         initialization and settings.
**     Settings  :
**         Serial channel              : SCI
**
**         Communication setting
**             Divisor value           : 13
**             Init baud rate          : 19230.769231 Bd
**             Data format             : 8 bits
**             Wakeup                  : by idle line
**             Polarity                : normal
**             Parity                  : none
**             SCI run in Wait mode    : Enabled
**
**         Interrupts
**           Receiver Full             : Enabled
**             ISR name                : fisr_sci_rx_full
**             Interrupt name          : INT_SCI_Rx_Full_Over_Error
**             Priority                : 2 (USER6)
**
**           Receiver Error            : Disabled
**
**           Transmitter Complete      : Disabled
**
**           Transmitter Ready         : Disabled
**
**     Contents  :
**         Init - void SCI1_Init(void);
**
**     (c) Copyright UNIS, a.s. 1997-2008
**     UNIS, a.s.
**     Jundrovska 33
**     624 00 Brno
**     Czech Republic
**     http      : www.processorexpert.com
**     mail      : info@processorexpert.com
** ###################################################################*/

/* MODULE SCI1. */

#include "SCI1.h"

/*
** ###################################################################
**
**  The following method(s) must be implemented by the user in one
**  of the following modules:
**
**    #pragma interrupt
**    void fisr_sci_rx_full(void)
**
**  Modules:
**       static_bootloader
**       Events
**
** ###################################################################
*/

/*
** ===================================================================
**     Method      :  SCI1_Init (bean Init_SCI)
**
**     Description :
**         This method initializes registers of the SCI module
**         according to this Peripheral Initialization Bean settings.
**         Call this method in the user code to initialize the
**         module. By default, the method is called by PE
**         automatically; see "Call Init method" property of the
**         bean for more details.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void SCI1_Init(void)
{
  getReg(SCI_STAT);                    /* Clear interrupt request flags */
  /* SCI_RATE: SBR=13,FRAC_SBR=0 */
  setReg16(SCI_RATE,C_SCI1_reg_SCI_RATE); /* Set prescaler bits */ 
  /* SCI_CTRL1: LOOP=0,SWAI=0,RSRC=0,M=0,WAKE=0,POL=0,PE=0,PT=0,TEIE=0,TIIE=0,RFIE=1,REIE=0,TE=1,RE=1,RWU=0,SBK=0 */
  setReg16(SCI_CTRL1,C_SCI1_reg_SCI_CTRL1); /* Set the SCI control register */ 
  /* SCI_CTRL2: TFCNT=0,TFWM=0,RFCNT=0,RFWM=0,FIFO_EN=0,??=0,LIN_MODE=0,??=0,??=0,??=0 */
  setReg16(SCI_CTRL2,C_SCI1_reg_SCI_CTRL2); /* Set the SCI control register */ 
}

/* END SCI1. */

/*
** ###################################################################
**
**     This file was created by UNIS Processor Expert 2.99 [04.17]
**     for the Freescale 56800 series of microcontrollers.
**
** ###################################################################
*/
