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

/* File: dspfunc.h */

#ifndef __DSPFUNC_H
#define __DSPFUNC_H

#ifndef SDK_LIBRARY
 #include "configdefines.h"

 #ifndef INCLUDE_DSPFUNC
  #error INCLUDE_DSPFUNC must be defined in AppConfig.h to initialize the DSP Function Library
 #endif
#endif

/*
   This include file is the master include file for the
   DSP568xx Digital Signal Processing Function Library -
   Fractional Algorithms for C and Assembly Programmers.
*/

/***************************
 Foundational Include Files
****************************/
#include "port.h"
#include "arch.h"
#include "prototype.h"
#include "dfr16priv.h"   /* MIEL_PRASAD */


/***************************
 Basic Fractional Math
****************************/
//#include "mfr16.h"
//#include "mfr32.h"

/***************************
 Single Dimension Array Functions
****************************/
//#include "afr16.h"
//#include "afr32.h"

/***************************
 Vector Functions
****************************/
//#include "vfr16.h"

/***************************
 Matrix Functions
****************************/
//#include "xfr16.h"

/***************************
 Trigonometric Functions
****************************/
#include "tfr16.h"

/***************************
 Signal Processing Functions
****************************/
#include "dfr16.h"

/***************************
 Functions to Workaround CW problems
****************************/

unsigned long impyuu (unsigned short unsigA, unsigned short unsigB);
long          impysu (short          sig,    unsigned short unsig);


void dspfuncInitialize(void);

#endif
