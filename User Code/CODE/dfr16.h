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

/* File: dfr16.h */

#ifndef __DFR16_H
#define __DFR16_H

#include "port.h"
#include "dfr16priv.h"   /* MIEL_PRASAD */

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************
* To switch between C and assembly implementations
*       #if 0  => assembly
*       #if 1  => C
*******************************************************/




/*********************************************************
* Flags for Fast Fourier Transform (FFT) Functions
**********************************************************/

#ifndef FFT_DEFAULT_OPTIONS

#define FFT_DEFAULT_OPTIONS               0  /* Default all options bits to 0     */
#define FFT_SCALE_RESULTS_BY_N            1  /* Unconditionally scale by N        */
#define FFT_SCALE_RESULTS_BY_DATA_SIZE    2  /* Scale according to data sizes     */
#define FFT_INPUT_IS_BITREVERSED          4  /* Default to normal (linear) input  */
#define FFT_OUTPUT_IS_BITREVERSED         8  /* Default to normal (linear) output */

#endif

/********************************************************
* Data Sructure for Real FFT and Real Inverse FFT
*********************************************************/

typedef struct {
        Frac16 z0;
        Frac16 zNDiv2;
        CFrac16 cz[1];
        } dfr16_sInplaceCRFFT;


/*********************************************************
* 16-bit Complex Fractional Forward Fast Fourier Transform
**********************************************************/
#define dfr16CFFTCreate(n,o) dfr16CFFTCreateFFT_SIZE_##n(o)
#define dfr16CFFTInit(p,n,o)   dfr16CFFTInitFFT_SIZE_##n(p,o)

EXPORT dfr16_tCFFTStruct *dfr16CFFTCreate   (8,    UInt16 options);
EXPORT dfr16_tCFFTStruct *dfr16CFFTCreate   (16,   UInt16 options);
EXPORT dfr16_tCFFTStruct *dfr16CFFTCreate   (32,   UInt16 options);
EXPORT dfr16_tCFFTStruct *dfr16CFFTCreate   (64,   UInt16 options);
EXPORT dfr16_tCFFTStruct *dfr16CFFTCreate   (128,  UInt16 options);
EXPORT dfr16_tCFFTStruct *dfr16CFFTCreate   (256,  UInt16 options);
EXPORT dfr16_tCFFTStruct *dfr16CFFTCreate   (512,  UInt16 options);
EXPORT dfr16_tCFFTStruct *dfr16CFFTCreate   (1024, UInt16 options);
EXPORT dfr16_tCFFTStruct *dfr16CFFTCreate   (2048, UInt16 options);

EXPORT void  dfr16CFFTInit   (dfr16_tCFFTStruct * pCFFT, 8,    UInt16 options);
EXPORT void  dfr16CFFTInit   (dfr16_tCFFTStruct * pCFFT, 16,   UInt16 options);
EXPORT void  dfr16CFFTInit   (dfr16_tCFFTStruct * pCFFT, 32,   UInt16 options);
EXPORT void  dfr16CFFTInit   (dfr16_tCFFTStruct * pCFFT, 64,   UInt16 options);
EXPORT void  dfr16CFFTInit   (dfr16_tCFFTStruct * pCFFT, 128,  UInt16 options);
EXPORT void  dfr16CFFTInit   (dfr16_tCFFTStruct * pCFFT, 256,  UInt16 options);
EXPORT void  dfr16CFFTInit   (dfr16_tCFFTStruct * pCFFT, 512,  UInt16 options);
EXPORT void  dfr16CFFTInit   (dfr16_tCFFTStruct * pCFFT, 1024, UInt16 options);
EXPORT void  dfr16CFFTInit   (dfr16_tCFFTStruct * pCFFT, 2048, UInt16 options);

EXPORT void dfr16CFFTDestroy (dfr16_tCFFTStruct *pCFFT);

EXPORT Result dfr16CFFT (dfr16_tCFFTStruct *pCFFT, CFrac16 *pX, CFrac16 *pZ);


/*********************************************************
* 16-bit Complex Fractional Inverse Fast Fourier Transform
**********************************************************/
#define dfr16CIFFTCreate(n,o) dfr16CFFTCreateFFT_SIZE_##n(o)
#define dfr16CIFFTInit(p,n,o) dfr16CFFTInitFFT_SIZE_##n(p,o)

EXPORT void dfr16CIFFTDestroy (dfr16_tCFFTStruct * pCIFFT);

EXPORT Result dfr16CIFFT (dfr16_tCFFTStruct *pCIFFT, CFrac16 *pX, CFrac16 *pZ);


/**********************************************************
* 16-bit In-Place Fractional Forward Fast Fourier Transform
***********************************************************/
#define dfr16RFFTCreate(n,o) dfr16RFFTCreateFFT_SIZE_##n(o)
#define dfr16RFFTInit(p,n,o) dfr16RFFTInitFFT_SIZE_##n(p,o)

EXPORT dfr16_tRFFTStruct *dfr16RFFTCreate (8, UInt16 options);
EXPORT dfr16_tRFFTStruct *dfr16RFFTCreate (16, UInt16 options);
EXPORT dfr16_tRFFTStruct *dfr16RFFTCreate (32, UInt16 options);
EXPORT dfr16_tRFFTStruct *dfr16RFFTCreate (64, UInt16 options);
EXPORT dfr16_tRFFTStruct *dfr16RFFTCreate (128, UInt16 options);
EXPORT dfr16_tRFFTStruct *dfr16RFFTCreate (256, UInt16 options);
EXPORT dfr16_tRFFTStruct *dfr16RFFTCreate (512, UInt16 options);
EXPORT dfr16_tRFFTStruct *dfr16RFFTCreate (1024, UInt16 options);
EXPORT dfr16_tRFFTStruct *dfr16RFFTCreate (2048, UInt16 options);

EXPORT void  dfr16RFFTInit   (dfr16_tRFFTStruct * pRFFT, 8,    UInt16 options);
EXPORT void  dfr16RFFTInit   (dfr16_tRFFTStruct * pRFFT, 16,   UInt16 options);
EXPORT void  dfr16RFFTInit   (dfr16_tRFFTStruct * pRFFT, 32,   UInt16 options);
EXPORT void  dfr16RFFTInit   (dfr16_tRFFTStruct * pRFFT, 64,   UInt16 options);
EXPORT void  dfr16RFFTInit   (dfr16_tRFFTStruct * pRFFT, 128,  UInt16 options);
EXPORT void  dfr16RFFTInit   (dfr16_tRFFTStruct * pRFFT, 256,  UInt16 options);
EXPORT void  dfr16RFFTInit   (dfr16_tRFFTStruct * pRFFT, 512,  UInt16 options);
EXPORT void  dfr16RFFTInit   (dfr16_tRFFTStruct * pRFFT, 1024, UInt16 options);
EXPORT void  dfr16RFFTInit   (dfr16_tRFFTStruct * pRFFT, 2048, UInt16 options);

EXPORT void    dfr16RFFTDestroy  (dfr16_tRFFTStruct * pRFFT);

EXPORT Result  dfr16RFFT (dfr16_tRFFTStruct * pRFFT, Frac16 *pX, dfr16_sInplaceCRFFT *pZ);


/**********************************************************
* 16-bit In-Place Inverse Fractional Fast Fourier Transform
***********************************************************/
#define dfr16RIFFTCreate(n,o) dfr16RFFTCreateFFT_SIZE_##n(o)
#define dfr16RIFFTInit(p,n,o) dfr16RFFTInitFFT_SIZE_##n(p,o)

EXPORT void    dfr16RIFFTDestroy  (dfr16_tRFFTStruct * pRIFFT);

EXPORT Result  dfr16RIFFT         (dfr16_tRFFTStruct * pRIFFT, dfr16_sInplaceCRFFT *pX, Frac16 *pZ);


/*******************************************************
* 16-bit Fractional Fast Fourier Transform Utilities
*******************************************************/

EXPORT Result  dfr16Cbitrev_       (CFrac16 *pX, CFrac16 *pZ, UInt16 n);


/*******************************************************
* 16-bit Fractional FIR Filters
*******************************************************/
typedef struct dfr16_sFirStruct {
 Frac16   * pC;                 /* Coefficients for the filter */
 Frac16   * pHistory;           /* Memory for the filter history buffer */
 UWord16    Private[6];
} dfr16_tFirStruct;

EXPORT dfr16_tFirStruct * dfr16FIRCreate     (Frac16 *pC, UInt16 n);
EXPORT void               dfr16FIRDestroy    (dfr16_tFirStruct *pFIR);

EXPORT void               dfr16FIRInit       (dfr16_tFirStruct *pFIR, Frac16 *pC, UInt16 n);

EXPORT void               dfr16FIRHistory    (dfr16_tFirStruct *pFIR, Frac16 *pX);

EXPORT void               dfr16FIR           (dfr16_tFirStruct *pFIR, Frac16 *pX, Frac16 *pZ, UInt16 n);
EXPORT Frac16             dfr16FIRs          (dfr16_tFirStruct *pFIR, Frac16 x);


#define dfr16_tFirDecStruct dfr16_tFirStruct

EXPORT dfr16_tFirDecStruct * dfr16FIRDecCreate  (Frac16 *pC, UInt16 n, UInt16 f);
/* EXPORT void               dfr16FIRDecDestroy (dfr16_tFirDecStruct *pFIRDec); */
#define dfr16FIRDecDestroy dfr16FIRDestroy

EXPORT void                dfr16FIRDecInit    (dfr16_tFirDecStruct *pFIRDec, Frac16 *pC, UInt16 n,   UInt16 f);

EXPORT UInt16              dfr16FIRDec        (dfr16_tFirDecStruct *pFIRDec, Frac16 *pX, Frac16 *pZ, UInt16 nx);


#define dfr16_tFirIntStruct dfr16_tFirStruct

EXPORT dfr16_tFirIntStruct * dfr16FIRIntCreate  (Frac16 *pC, UInt16 n, UInt16 f);
/* EXPORT void               dfr16FIRIntDestroy (dfr16_tFirIntStruct *pFIRInt); */
#define dfr16FIRIntDestroy dfr16FIRDestroy

EXPORT void                  dfr16FIRIntInit    (dfr16_tFirIntStruct *pFIRInt, Frac16 *pC, UInt16 n,   UInt16 f);

EXPORT UInt16                dfr16FIRInt        (dfr16_tFirIntStruct *pFIRInt, Frac16 *pX, Frac16 *pZ, UInt16 n);


/*******************************************************
* 16-bit Fractional IIR Filters
*******************************************************/
#define FILT_STATES_PER_BIQ 2
#define FILT_COEF_PER_BIQ   5
//typedef struct dfr16_sIirStruct dfr16_tIirStruct;

EXPORT dfr16_tIirStruct * dfr16IIRCreate  (Frac16 *pC, UInt16 nbiq);
EXPORT void               dfr16IIRDestroy (dfr16_tIirStruct *pIIR);

EXPORT void dfr16IIRInit (dfr16_tIirStruct * pIIR, Frac16 *pC, UInt16 n);

EXPORT Result             dfr16IIR        (dfr16_tIirStruct *pIIR, Frac16 *pX, Frac16 *pZ, UInt16 n);


/*******************************************************
* 16-bit Fractional Correlations
*******************************************************/

#ifndef CORR_RAW

#define CORR_RAW      0  /* Select Raw correlation     */
#define CORR_BIAS     1  /* Select Bias correlation    */
#define CORR_UNBIAS   2  /* Select Unbias correlation  */

#endif

EXPORT Result  dfr16AutoCorr (UInt16 options, Frac16 *pX, Frac16 *pZ, UInt16 nx, UInt16 nz);

EXPORT Result  dfr16Corr     (UInt16 options, Frac16 *pX, Frac16 *pY, Frac16 *pZ, UInt16 nx, UInt16 ny);


#ifdef __cplusplus
}
#endif

#endif
