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

/* File: dfr16priv.h */

#ifndef __DFR16PRIV_H
#define __DFR16PRIV_H

#include "port.h"

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************
* 16-bit Fractional FIR Filter private data structure
*******************************************************/
typedef struct dfr16_sFirStructPriv
{
  Frac16 * pC;                 /* Coefficients for the filter */
  Frac16 * pHistory;           /* Memory for the filter history buffer */
  Frac16 * pNextHistoryAddr;   /* Next memory element to use */
  UInt16   n;                  /* Length of the vector of FIR filter coefficients */
  ibool    bCanUseModAddr;     /* Can use modulo addressing on history buffer */
  ibool    bCanUseDualMAC;     /* Set if FIR can use dual parallel move MAC */
  UInt16   Factor;             /* Interpolation or decimation factor */
  UInt16   Count;              /* Count used in decimation/interpolation */
} dfr16_tFirStructPriv;


/*******************************************************
* 16-bit Fractional CFFT private data structure
*******************************************************/

/* Added: MIEL_PRASAD */
typedef struct {
    UInt16 options;
    UInt16 n;
    CFrac16 *Twiddle;
    Int16 No_of_Stages;
} dfr16_tCFFTStruct;


/*******************************************************
* 16-bit Fractional IIR Filter private data structure
*******************************************************/
typedef struct dfr16_sIirStruct
{
  Frac16 * pC;                 /* Coefficients for the filter */
  Frac16 * pHistory;           /* Memory for the filter history buffer */
  Frac16 * pNextHistoryAddr;   /* Next memory element to use */
  UInt16   nbiq;               /* No. of biquads */
  ibool    bCanUseModAddr;     /* Can use modulo addressing on history buffer */
  ibool    bCanUseDualMAC;     /* Set if IIR can use dual parallel move MAC */
} dfr16_tIirStruct;

/***********************************************************/
/* 16-bit Fractional RFFT private Data structure           */
/***********************************************************/

typedef struct
{
    UInt16 options;
    UInt16 n;
    CFrac16 *Twiddle;
    CFrac16 *Twiddle_br;
    UInt16 No_of_Stages;
} dfr16_tRFFTStruct;

#ifdef __cplusplus
}
#endif

#endif

