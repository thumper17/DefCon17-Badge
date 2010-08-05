/** ###################################################################
**
**     (c) Freescale Semiconductor
**     2004 All Rights Reserved
**
**
**     (c) Copyright UNIS, a.s. 1997-2008
**     UNIS, a.s.
**     Jundrovska 33
**     624 00 Brno
**     Czech Republic
**     http      : www.processorexpert.com
**     mail      : info@processorexpert.com
** ###################################################################*/

/* File: periph.h */

#ifndef __PERIPH_H
#define __PERIPH_H

#include "port.h"


#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************
* Routines for Peripheral Memory Access
********************************************************
*  Parameter usage:
*   Addr - architecture structure field
*   Mask - bit to manipulate
*   Data - 16 bit word to assign or access
********************************************************/

/*
inline void periphBitSet(register UWord16 Mask, register volatile UWord16 * Addr)
{
  register unsigned int Value;

  asm(move.w  x:(Addr+$1ffc00),Value);
  asm(or.w    Mask,Value);
  asm(move.w  Value,x:(Addr+$1ffc00));
}
*/

/*
inline void periphBitClear(register UWord16 Mask, register volatile UWord16 * Addr)
{
  register unsigned int Value;

  asm(move.w  x:(Addr+$1ffc00),Value);
  asm(not.w   Mask);
  asm(and.w   Mask,Value);
  asm(move.w  Value,x:(Addr+$1ffc00));
}
*/

/*
inline void periphBitChange(register UWord16 Mask, register volatile UWord16 * Addr)
{
  register unsigned int Value;

  asm(move.w  x:(Addr+$1ffc00),Value);
  asm(eor.w   Mask,Value);
  asm(move.w  Value,x:(Addr+$1ffc00));
}
*/

/*
inline bool periphBitTest(register UWord16 Mask, register volatile UWord16 * Addr)
{
  register unsigned int Value;

  asm(move.w  x:(Addr+$1ffc00),Value);

  if ((Value & Mask) == 0)
  {
    return false;
  }

  return true;
}
*/

/*
inline void periphMemWrite(register UWord16 Data, register volatile UWord16 * Addr)
{
  asm(move.w  Data,X:(Addr+$1ffc00));
}
*/

/*
inline UWord16 periphMemRead(register volatile UWord16 * Addr)
{
  register unsigned int Value;

  asm(move.w  X:(Addr+$1ffc00),Value);

  return Value;
}
*/

#ifdef __cplusplus
}
#endif

#endif
