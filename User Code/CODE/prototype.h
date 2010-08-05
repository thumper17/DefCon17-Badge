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

/* File: prototype.h */

#ifndef __PROTOTYPE_H
#define __PROTOTYPE_H

#include "port.h"
#include "arch.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************/
/* Name mappings for DSP code portability             */
/******************************************************/

/* void setnostat (void); */
#define setnosat   archSetNoSat

/* void setstat32 (void); */
#define setsat32   archSetSat32

/* void Stop (void); */
#define Stop        archStop

/* void Trap (void); */
#define Trap        archTrap

/* void Wait (void); */
#define Wait        archWait

/* void EnableInt (void); */
#define EnableInt   archEnableInt

/* void DisableInt (void); */
#define DisableInt  archDisableInt

#define MAX_32 (Word32)0x7fffffffL
#define MIN_32 (Word32)0x80000000L

#define MAX_16 (Word16)0x7fff
#define MIN_16 (Word16)0x8000


#ifdef ITU_INTRINSICS

  /*******************************************************

     Predefined basic intrinsics.

     The intrinsic functions are defined in the compiler
     defined functions name space. They are redefined here
     according to the ETSI naming convention.

   ******************************************************/

  #include "intrinsics_56800E.h"

#else

  Word16 div_ls  (Word32 L_var1, Word16 L_var2);
  Word32 L_mult_ls (Word32 L_var1, Word16 L_var2);

  /* Use ITU C code */
  #include "basic_op.h"

#endif


#ifdef __cplusplus
}
#endif

#endif
