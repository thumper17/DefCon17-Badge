/** ###################################################################
**     Filename  : Events.H
**     Project   : DC17
**     Processor : MC56F8006_32_LQFP
**     Beantype  : Events
**     Version   : Driver 01.03
**     Compiler  : Metrowerks DSP C Compiler
**     Date/Time : 1/18/2009, 1:37 PM
**     Abstract  :
**         This is user's event module.
**         Put your event handler code here.
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

#ifndef __Events_H
#define __Events_H
/* MODULE Events */

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
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


void TIPIT_OnInterrupt(void);
/*
** ===================================================================
**     Event       :  TIPIT_OnInterrupt (module Events)
**
**     From bean   :  TIPIT [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the bean is enabled - <Enable> and the events are
**         enabled - <EnableEvent>). This event is enabled only if a
**         <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void TI1_OnInterrupt(void);
/*
** ===================================================================
**     Event       :  TI1_OnInterrupt (module Events)
**
**     From bean   :  TI1 [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the bean is enabled - <Enable> and the events are
**         enabled - <EnableEvent>). This event is enabled only if a
**         <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void TI0_OnInterrupt(void);
/*
** ===================================================================
**     Event       :  TI0_OnInterrupt (module Events)
**
**     From bean   :  TI0 [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the bean is enabled - <Enable> and the events are
**         enabled - <EnableEvent>). This event is enabled only if a
**         <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void BADGEI2C_OnFullRxBuf(void);
/*
** ===================================================================
**     Event       :  BADGEI2C_OnFullRxBuf (module Events)
**
**     From bean   :  BADGEI2C [InternalI2C]
**     Description :
**         Called when the input buffer is full. This event is not
**         available for the MASTER mode. 
**         If OnRxChar event also generated, this event is called
**         after OnRxChar.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void BADGEI2C_OnNACK(void);
/*
** ===================================================================
**     Event       :  BADGEI2C_OnNACK (module Events)
**
**     From bean   :  BADGEI2C [InternalI2C]
**     Description :
**         Called when a no slave acknowledge (NAK) occurs during
**         communication. This event is not available for the SLAVE
**         mode. This event is enabled only if interrupts/events are
**         enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void BADGEI2C_OnGeneralCall(void);
/*
** ===================================================================
**     Event       :  BADGEI2C_OnGeneralCall (module Events)
**
**     From bean   :  BADGEI2C [InternalI2C]
**     Description :
**         This event is called when a general call address is
**         received and it is acknowledged. This event is not
**         available for the MASTER mode and if the General call
**         address is disabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

/* END Events */
#endif /* __Events_H*/

/*
** ###################################################################
**
**     This file was created by UNIS Processor Expert 2.99 [04.17]
**     for the Freescale 56800 series of microcontrollers.
**
** ###################################################################
*/
