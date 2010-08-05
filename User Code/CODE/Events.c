/** ###################################################################
**     Filename  : Events.C
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
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"

#include "DC17_Badge.h"
extern struct packed_flags flags;


/*
** ===================================================================
**     Event       :  BADGEI2C_OnReceiveData (module Events)
**
**     From bean   :  BADGEI2C [InternalI2C]
**     Description :
**         This event is invoked when BADGEI2C finishes the reception of
**         the data successfully. This event is not available for
**         the SLAVE mode and if both RecvChar and RecvBlock are
**         disabled. This event is enabled only if interrupts/events
**         are enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
#pragma interrupt called /* Comment this line if the appropriate 'Interrupt preserve registers' property */
/*
** ===================================================================
**     Event       :  BADGEI2C_OnTransmitData (module Events)
**
**     From bean   :  BADGEI2C [InternalI2C]
**     Description :
**         This event is invoked when BADGEI2C finishes the transmission
**         of the data successfully. This event is not available for
**         the SLAVE mode and if both SendChar and SendBlock are
**         disabled. This event is enabled only if interrupts/events
**         are enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
#pragma interrupt called /* Comment this line if the appropriate 'Interrupt preserve registers' property */
/*
** ===================================================================
**     Event       :  BADGEI2C_OnRxChar (module Events)
**
**     From bean   :  BADGEI2C [InternalI2C]
**     Description :
**         Called when a correct character is received. This event
**         is not available for the MASTER mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
#pragma interrupt called /* Comment this line if the appropriate 'Interrupt preserve registers' property */
/*
** ===================================================================
**     Event       :  BADGEI2C_OnTxChar (module Events)
**
**     From bean   :  BADGEI2C [InternalI2C]
**     Description :
**         Called when a correct character is sent (placed to the
**         transmitter). This event is not available for the MASTER
**         mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
#pragma interrupt called /* Comment this line if the appropriate 'Interrupt preserve registers' property */
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
#pragma interrupt called /* Comment this line if the appropriate 'Interrupt preserve registers' property */
                         /* is set to 'yes' (#pragma interrupt saveall is generated before the ISR)      */
void TIPIT_OnInterrupt(void)
{
  dc17_pit_isr();
}

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
#pragma interrupt called /* Comment this line if the appropriate 'Interrupt preserve registers' property */
                         /* is set to 'yes' (#pragma interrupt saveall is generated before the ISR)      */
void TI1_OnInterrupt(void)
{
	dc17_t1_isr();
}

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
#pragma interrupt called /* Comment this line if the appropriate 'Interrupt preserve registers' property */
                         /* is set to 'yes' (#pragma interrupt saveall is generated before the ISR)      */
void TI0_OnInterrupt(void)
{
  /* Write your code here ... */
  	dc17_t0_isr();
}

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
#pragma interrupt called /* Comment this line if the appropriate 'Interrupt preserve registers' property */
                         /* is set to 'yes' (#pragma interrupt saveall is generated before the ISR)      */
void BADGEI2C_OnNACK(void)
{
  /* Write your code here ... */
  flags.i2c_nack = 1;
}


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
#pragma interrupt called /* Comment this line if the appropriate 'Interrupt preserve registers' property */
                         /* is set to 'yes' (#pragma interrupt saveall is generated before the ISR)      */
void BADGEI2C_OnFullRxBuf(void)
{
  /* Write your code here ... */
  flags.i2c_rx = 1;  
}


