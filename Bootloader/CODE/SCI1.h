/** ###################################################################
**     THIS BEAN MODULE IS GENERATED BY THE TOOL WITH OPTION "Preserve User Changes".
**     IT'S NOT RECOMMENDED TO MODIFY IT, BUT THE TOOL ACCEPTS CHANGES IN THIS MODE.
**     Filename  : SCI1.H
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

#ifndef __SCI1
#define __SCI1

/* MODULE SCI1. */

#include "Cpu.ch"
/*Include shared modules, which are used for whole project*/
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* Include inherited beans */
#include "Cpu.h"

/* PESL device mapping  */
#define SCI1_DEVICE SCI

extern void fisr_sci_rx_full(void);


void SCI1_Init(void);
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

/* END SCI1. */

#endif /* ifndef __SCI1 */
/*
** ###################################################################
**
**     This file was created by UNIS Processor Expert 2.99 [04.17]
**     for the Freescale 56800 series of microcontrollers.
**
** ###################################################################
*/