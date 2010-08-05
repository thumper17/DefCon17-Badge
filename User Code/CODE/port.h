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

/* File: port.h */

#include "PE_Types.h"

#ifndef __PORT_H
#define __PORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "PE_Types.h"
#include "IO_Map.h"

/*******************************************************
* Target designation
*******************************************************/

/* MetroWerks defines __m56800E__ */


/*******************************************************
* C Constructs
*******************************************************/

#define EXPORT extern
#define ITU_INTRINSICS


/*******************************************************
* Basic Types
*******************************************************/

/* 8-bit integer types */
//typedef unsigned char  UInt8;
//typedef signed char    Int8;

/* Generic word types for ITU compatibility */
//typedef short          Word16;
//typedef unsigned short UWord16;
//typedef long           Word32;
//typedef unsigned long  UWord32;

/* 16-bit integer types */
//typedef int            Int16;
//typedef unsigned int   UInt16;
//typedef long           Int32;
//typedef unsigned long  UInt32;

/* Fractional data types for portability */
//typedef short          Frac16;
//typedef long           Frac32;

typedef struct {
   Frac16     real;
   Frac16     imag;
} CFrac16;

typedef struct {
   Frac32     real;
   Frac32     imag;
} CFrac32;

/* Useful definitions */

/* Convert int/float to Frac16; constant x generates compile time constant */
#define FRAC16(x) ((Frac16)((x) < 1 ? ((x) >= -1 ? ((Frac16)((x)*0x8000)) : ((Frac16)0x8000)) : ((Frac16)0x7FFF)))
/* Convert int/float to Frac32; constant x generates compile time constant */
#define FRAC32(x) ((Frac32)((x) < 1 ? ((x) >= -1 ? ((Frac32)((x)*0x80000000)) : ((Frac32)0x80000000)) : ((Frac32)0x7FFFFFFF)))

/* Miscellaneous types */
typedef int            Flag;

typedef int            Result;

//#ifndef COMPILER_HAS_BOOL
//typedef int            bool;
//#endif


/*******************************************************
* Conditional assembly
*******************************************************/

/*
  PORT_ASSERT_ON_INVALID_PARAMETER conditionally compiles
  code to check the validity of function parameters
*/
#define PORT_ASSERT_ON_INVALID_PARAMETER

/*
   These defines permit the same C source code to be
   used for Large and Small Memory Models
*/

#undef PORT_LARGE_MEMORY_MODEL


/*******************************************************
* Constants
*******************************************************/

/* Function Result Values */
#define PASS           0
#define FAIL           -1

#ifndef COMPILER_HAS_BOOL
#define true           1
#define false          0
#endif


/*******************************************************
* Implementation Limits
*******************************************************/

#define PORT_MAX_VECTOR_LEN  65535

#include "types.h"
#include "arch.h"

#ifdef __cplusplus
}
#endif

#endif

