/** ###################################################################
**     THIS BEAN MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : MICPWR.C
**     Project   : DC17
**     Processor : MC56F8006_32_LQFP
**     Beantype  : BitIO
**     Version   : Bean 02.072, Driver 01.17, CPU db: 3.00.169
**     Compiler  : Metrowerks DSP C Compiler
**     Date/Time : 3/21/2009, 1:50 PM
**     Abstract  :
**         This bean "BitIO" implements an one-bit input/output.
**         It uses one bit/pin of a port.
**         Note: This bean is set to work in Output direction only.
**         Methods of this bean are mostly implemented as a macros
**         (if supported by target language and compiler).
**     Settings  :
**         Used pin                    :
**             ----------------------------------------------------
**                Number (on package)  |    Name
**             ----------------------------------------------------
**                       6             |  GPIOC5_PGA1INV_ANB6_CMP0_P4
**             ----------------------------------------------------
**
**         Port name                   : GPIOC
**
**         Bit number (in port)        : 5
**         Bit mask of the port        : 32
**
**         Initial direction           : Output (direction cannot be changed)
**         Initial output value        : 1
**         Initial pull option         : off
**
**         Port data register          : GPIO_C_DR [61889]
**         Port control register       : GPIO_C_DDR [61890]
**         Port function register      : GPIO_C_PER [61891]
**
**         Optimization for            : code size
**     Contents  :
**         ClrVal - void MICPWR_ClrVal(void);
**         SetVal - void MICPWR_SetVal(void);
**
**     (c) Copyright UNIS, a.s. 1997-2008
**     UNIS, a.s.
**     Jundrovska 33
**     624 00 Brno
**     Czech Republic
**     http      : www.processorexpert.com
**     mail      : info@processorexpert.com
** ###################################################################*/

/* MODULE MICPWR. */

#include "MICPWR.h"
/*Include shared modules, which are used for whole project*/
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

#include "Cpu.h"


/*
** ===================================================================
**     Method      :  BitIO__PutVal (bean BitIO)
**
**     Description :
**         Sets pin value. The method is called automatically as a part 
**         of several methods if optimization for code size is required.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void BitIO__PutVal(byte port, register word mask, bitAction value, register byte safe, word *Shadow)
{
  tBitIO_portDsc *Item=&BitIO_portDsc[port];
  register word *TempReg=Item->portReg; /* Temporary variable - data port address */

  switch (value) {                     /* Which operation should be performed? */
  case CLR:                            /* Set output value to zero */
    if (safe)                          /* Is Safe mode active? */
      *Shadow &= ~mask;                /* Clear appropriate bit in shadow variable */
    *TempReg &= ~mask;                 /* Clear appropriate bit in appropriate port */
    break;
  case SET:                            /* Set output value to one */
    if (safe)                          /* Is Safe mode active? */
      *Shadow |= mask;                 /* Set appropriate bit in shadow variable */
    *TempReg |= mask;                  /* Set appropriate bit in appropriate port */
    break;
  case NEG:                            /* Negate output value */
    if (safe)                          /* Is Safe mode active? */
      *Shadow ^= mask;                 /* Negate appropriate bit in shadow variable */
    *TempReg ^= mask;                  /* Negate appropriate bit in appropriate port */
    break;
  }
}

/*
** ===================================================================
**     Method      :  MICPWR_SetVal (bean BitIO)
**
**     Description :
**         This method sets (sets to one) the output value.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
/*
void MICPWR_SetVal(void)

**  This method is implemented as a macro. See MICPWR.h file.  **
*/

/*
** ===================================================================
**     Method      :  MICPWR_ClrVal (bean BitIO)
**
**     Description :
**         This method clears (sets to zero) the output value.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
/*
void MICPWR_ClrVal(void)

**  This method is implemented as a macro. See MICPWR.h file.  **
*/

/* END MICPWR. */

/*
** ###################################################################
**
**     This file was created by UNIS Processor Expert 2.99 [04.17]
**     for the Freescale 56800 series of microcontrollers.
**
** ###################################################################
*/
