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

/* File: arch.h */

#ifndef __ARCH_H
#define __ARCH_H

#include "port.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************
* Architecture Dependent Routines
*******************************************************/
/* Get limit bit */
EXPORT Flag archGetLimitBit (void);

/* void archResetLimitBit (void); */
#define archResetLimitBit() asm(bfclr  #0x40,SR)

/* void archSetNoSat (void); */
#define archSetNoSat() asm(bfclr #0x10,OMR)

/* void archSetSat32 (void); */
#define archSetSat32() asm(bfset #0x10,OMR)

/* Get, then set saturation mode */
EXPORT bool archGetSetSaturationMode (bool bSatMode);

/* void archSet2CompRound (void); */
#define archSet2CompRound() asm(bfset #0x20,OMR)

/* void archSetConvRound (void); */
#define archSetConvRound() asm(bfclr #0x20,OMR)

/* void archStop (void); */
#define archStop() asm(stop)

/* void archTrap (void); */
#define archTrap() asm(swi)

/* void archWait (void); */
#define archWait() asm(wait)

/* void archEnableInt (void); */
#define archEnableInt() asm(bfclr #0x0300,SR)

/* void archDisableInt (void); */
#define archDisableInt() asm(bfset #0x0300,SR)


#define archMemRead(Local, Remote, Bytes) *(Local) = *(Remote)
#define archMemWrite(Remote, Local, Bytes) *(Remote) = *(Local)

#define archCoreRegisterBitSet(Mask, Reg)     asm(bfset   Mask,Reg)
#define archCoreRegisterBitClear(Mask, Reg)    asm(bfclr   Mask,Reg)
#define archCoreRegisterBitChange(Mask, Reg)   asm(bfchg   Mask,Reg)
#define archCoreRegisterBitTestHigh(Mask, Reg) asm(bftsth  Mask,Reg)
#define archCoreRegisterBitTestLow(Mask, Reg)  asm(bftstl  Mask,Reg)

#define archMemBitSet(Mask, Addr)          asm(bfset   Mask,Addr)
#define archMemBitClear(Mask, Addr)         asm(bfclr   Mask,Addr)
#define archMemBitChange(Mask, Addr)        asm(bfchg   Mask,Addr)
#define archMemBitTestHigh(Mask, Addr)       asm(bftsth  Mask,Addr)
#define archMemBitTestLow(Mask, Addr)        asm(bftstl  Mask,Addr)

#ifdef __cplusplus
}
#endif

#endif
