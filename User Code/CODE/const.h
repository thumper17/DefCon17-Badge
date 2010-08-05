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

/* const.h */

#include "port.h"
#include "arch.h"

/* General constants */
extern const char constNewline[];


/*
  DSP FUNCTION Library constants
*/
#if defined(SDK_LIBRARY) || defined(INCLUDE_DSPFUNC)

    EXPORT const CFrac16 dfr16CFFTTwiddleFactorTable8[8];
    EXPORT const CFrac16 dfr16CFFTTwiddleFactorTable16[16];
    EXPORT const CFrac16 dfr16CFFTTwiddleFactorTable32[32];
    EXPORT const CFrac16 dfr16CFFTTwiddleFactorTable64[64];
    EXPORT const CFrac16 dfr16CFFTTwiddleFactorTable128[128];
    EXPORT const CFrac16 dfr16CFFTTwiddleFactorTable256[256];
    EXPORT const CFrac16 dfr16CFFTTwiddleFactorTable512[512];
    EXPORT const CFrac16 dfr16CFFTTwiddleFactorTable1024[1024];
    EXPORT const CFrac16 dfr16CFFTTwiddleFactorTable2048[2048];

    EXPORT const CFrac16 dfr16RFFTTwiddleTable8[];
    EXPORT const CFrac16 dfr16RFFTTwiddleTable8br[];
    EXPORT const CFrac16 dfr16RFFTTwiddleTable16[];
    EXPORT const CFrac16 dfr16RFFTTwiddleTable16br[];
    EXPORT const CFrac16 dfr16RFFTTwiddleTable32[];
    EXPORT const CFrac16 dfr16RFFTTwiddleTable32br[];
    EXPORT const CFrac16 dfr16RFFTTwiddleTable64[];
    EXPORT const CFrac16 dfr16RFFTTwiddleTable64br[];
    EXPORT const CFrac16 dfr16RFFTTwiddleTable128[];
    EXPORT const CFrac16 dfr16RFFTTwiddleTable128br[];
    EXPORT const CFrac16 dfr16RFFTTwiddleTable256[];
    EXPORT const CFrac16 dfr16RFFTTwiddleTable256br[];
    EXPORT const CFrac16 dfr16RFFTTwiddleTable512[];
    EXPORT const CFrac16 dfr16RFFTTwiddleTable512br[];
    EXPORT const CFrac16 dfr16RFFTTwiddleTable1024[];
    EXPORT const CFrac16 dfr16RFFTTwiddleTable1024br[];
    EXPORT const CFrac16 dfr16RFFTTwiddleTable2048[];
    EXPORT const CFrac16 dfr16RFFTTwiddleTable2048br[];

#endif
