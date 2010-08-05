/********************************************************************
*
* (c) Freescale Semiconductor
* 2004 All Rights Reserved
*
*********************************************************************/
/* File: dfr16.c */

#include "port.h"
#include "prototype.h"
#include "dfr16.h"
#include "dfr16priv.h"
#include <stdlib.h>
#include "mem.h"
#include "assert.h"
#include "const.h"

#define MAX_VECTOR_LEN 8192

/* Declare C function prototypes */

Result  dfr16CFFTC     (dfr16_tCFFTStruct *pCFFT, CFrac16 *pX, CFrac16 *pZ);
Result  dfr16RFFTC     (dfr16_tRFFTStruct *pRFFT, Frac16 *pX, dfr16_sInplaceCRFFT *pZ);
Result  dfr16CIFFTC    (dfr16_tCFFTStruct *pCIFFT, CFrac16 *pX, CFrac16 *pZ);
Result  dfr16RIFFTC    (dfr16_tRFFTStruct * pRIFFT, dfr16_sInplaceCRFFT *pX, Frac16 *pZ);
void    dfr16FIRC      (dfr16_tFirStruct *pFIR, Frac16 *pX, Frac16 *pZ, UInt16 n);
UInt16  dfr16FIRIntC (dfr16_tFirIntStruct *pFIRINT, Frac16 *pX, Frac16 *pZ, UInt16 n);
Result  dfr16IIRC      (dfr16_tIirStruct *pIIR, Frac16 *pX, Frac16 *pZ, UInt16 n);
Result  dfr16AutoCorrC (UInt16 options, Frac16 *pX, Frac16 *pZ, UInt16 nx, UInt16 nz);
Result  dfr16CorrC     (UInt16 options,Frac16 *px, Frac16 *py, Frac16 *pz, UInt16 nx, UInt16 ny);
Result  dfr16CbitrevC  (CFrac16 *pX, CFrac16 *pZ, UInt16 n);

Word32 AddAndDivBy2(Word32, Word32);

//Word32 AddAndDivBy2(register Word32 I, register Word32 J)
//{
//    asm(add b,a);
//    asm(asr a);
//}

asm Word32 AddAndDivBy2(register Word32 I, register Word32 J)
{
    add b,a;
    asr a;
    rts;
}


/*******************************************************
* 16-bit Complex Fractional Forward Fast Fourier Transforms
*******************************************************/

/*--------------------------------------------------------------
 * Revision History:
 *
 * VERSION    CREATED BY    MODIFIED BY      DATE       COMMENTS
 * -------    ----------    -----------      -----      --------
 *   0.1      Sandeep S        -          31-01-2000   For review.
 *   1.0          -            -          16-02-2000   Reviewed and
 *                                                     baselined.
 *
 *-------------------------------------------------------------*/

/*--------------------------------------------------------------
 * FILE:        dfr16.c
 *
 * FUNCTION:    dfr16_tCFFTStruct * dfr16CFFTCreateFFT_SIZE_8 (UInt16 options)
 *
 * DESCRIPTION: Initialization for N = 8 CFFT.
 *
 * ARGUMENTS:   options - 0 - FFT_DEFAULT_OPTIONS
 *                        1 - FFT_SCALE_RESULTS_BY_N
 *                        2 - FFT_SCALE_RESULTS_BY_DATA_SIZE
 *                        4 - FFT_INPUT_IS_BITREVERSED
 *                        8 - FFT_OUTPUT_IS_BITREVERSED
 *
 * RETURNS:     Pointer to private data structure pCFFT.
 *
 * GLOBAL
 *  VARIABLES:  None
 *--------------------------------------------------------------*/

dfr16_tCFFTStruct * dfr16CFFTCreateFFT_SIZE_8 (UInt16 options)
{

    dfr16_tCFFTStruct * pCFFT;

    pCFFT = (dfr16_tCFFTStruct *) memMallocIM(sizeof(dfr16_tCFFTStruct));

    dfr16CFFTInitFFT_SIZE_8 (pCFFT, options);

    return pCFFT;
}


void dfr16CFFTInitFFT_SIZE_8 (dfr16_tCFFTStruct * pCFFT, UInt16 options)
{
    pCFFT->n            = 8;
    pCFFT->options      = options;
    pCFFT->Twiddle      = (CFrac16 *)dfr16CFFTTwiddleFactorTable8;
    pCFFT->No_of_Stages = 3;
}


/*--------------------------------------------------------------
 * FILE:        dfr16.c
 *
 * FUNCTION:    dfr16_tCFFTStruct * dfr16CFFTCreateFFT_SIZE_16 (UInt16 options)
 *
 * DESCRIPTION: Initialization for N = 16 CFFT.
 *
 * ARGUMENTS:   options - 0 - FFT_DEFAULT_OPTIONS
 *                        1 - FFT_SCALE_RESULTS_BY_N
 *                        2 - FFT_SCALE_RESULTS_BY_DATA_SIZE
 *                        4 - FFT_INPUT_IS_BITREVERSED
 *                        8 - FFT_OUTPUT_IS_BITREVERSED
 *
 * RETURNS:     Pointer to private data structure pCFFT.
 *
 * GLOBAL
 *  VARIABLES:  None
 *--------------------------------------------------------------*/

dfr16_tCFFTStruct * dfr16CFFTCreateFFT_SIZE_16 (UInt16 options)
{
    dfr16_tCFFTStruct * pCFFT;

    pCFFT = (dfr16_tCFFTStruct *)memMallocIM(sizeof(dfr16_tCFFTStruct));

    dfr16CFFTInitFFT_SIZE_16 (pCFFT, options);

    return pCFFT;
}

void dfr16CFFTInitFFT_SIZE_16 (dfr16_tCFFTStruct * pCFFT, UInt16 options)
{
    pCFFT->n            = 16;
    pCFFT->options      = options;
    pCFFT->Twiddle      = (CFrac16 *)dfr16CFFTTwiddleFactorTable16;
    pCFFT->No_of_Stages = 4;
}


/*--------------------------------------------------------------
 * FILE:        dfr16.c
 *
 * FUNCTION:    dfr16_tCFFTStruct * dfr16CFFTCreateFFT_SIZE_32 (UInt16 options)
 *
 * DESCRIPTION: Initialization for N = 32 CFFT.
 *
 * ARGUMENTS:   options - 0 - FFT_DEFAULT_OPTIONS
 *                        1 - FFT_SCALE_RESULTS_BY_N
 *                        2 - FFT_SCALE_RESULTS_BY_DATA_SIZE
 *                        4 - FFT_INPUT_IS_BITREVERSED
 *                        8 - FFT_OUTPUT_IS_BITREVERSED
 *
 * RETURNS:     Pointer to private data structure pCFFT.
 *
 * GLOBAL
 *  VARIABLES:  None
 *--------------------------------------------------------------*/

dfr16_tCFFTStruct * dfr16CFFTCreateFFT_SIZE_32 (UInt16 options)
{

    dfr16_tCFFTStruct * pCFFT;
    pCFFT = (dfr16_tCFFTStruct *)memMallocIM(sizeof(dfr16_tCFFTStruct));

    dfr16CFFTInitFFT_SIZE_32 (pCFFT, options);

    return pCFFT;
}


void dfr16CFFTInitFFT_SIZE_32 (dfr16_tCFFTStruct * pCFFT, UInt16 options)
{
    pCFFT->n            = 32;
    pCFFT->options      = options;
    pCFFT->Twiddle      = (CFrac16 *)dfr16CFFTTwiddleFactorTable32;
    pCFFT->No_of_Stages = 5;
}



/*--------------------------------------------------------------
 * FILE:        dfr16.c
 *
 * FUNCTION:    dfr16_tCFFTStruct * dfr16CFFTCreateFFT_SIZE_64 (UInt16 options)
 *
 * DESCRIPTION: Initialization for N = 64 CFFT.
 *
 * ARGUMENTS:   options - 0 - FFT_DEFAULT_OPTIONS
 *                        1 - FFT_SCALE_RESULTS_BY_N
 *                        2 - FFT_SCALE_RESULTS_BY_DATA_SIZE
 *                        4 - FFT_INPUT_IS_BITREVERSED
 *                        8 - FFT_OUTPUT_IS_BITREVERSED
 *
 * RETURNS:     Pointer to private data structure pCFFT.
 *
 * GLOBAL
 *  VARIABLES:  None
 *--------------------------------------------------------------*/

dfr16_tCFFTStruct * dfr16CFFTCreateFFT_SIZE_64 (UInt16 options)
{

    dfr16_tCFFTStruct * pCFFT;

    pCFFT = (dfr16_tCFFTStruct *)memMallocIM(sizeof(dfr16_tCFFTStruct));

    dfr16CFFTInitFFT_SIZE_64 (pCFFT, options);

    return pCFFT;
}


void dfr16CFFTInitFFT_SIZE_64 (dfr16_tCFFTStruct * pCFFT, UInt16 options)
{
    pCFFT->n            = 64;
    pCFFT->options      = options;
    pCFFT->Twiddle      = (CFrac16 *)dfr16CFFTTwiddleFactorTable64;
    pCFFT->No_of_Stages = 6;
}



/*--------------------------------------------------------------
 * FILE:        dfr16.c
 *
 * FUNCTION:    dfr16_tCFFTStruct * dfr16CFFTCreateFFT_SIZE_128 (UInt16 options)
 *
 * DESCRIPTION: Initialization for N = 128 CFFT.
 *
 * ARGUMENTS:   options - 0 - FFT_DEFAULT_OPTIONS
 *                        1 - FFT_SCALE_RESULTS_BY_N
 *                        2 - FFT_SCALE_RESULTS_BY_DATA_SIZE
 *                        4 - FFT_INPUT_IS_BITREVERSED
 *                        8 - FFT_OUTPUT_IS_BITREVERSED
 *
 * RETURNS:     Pointer to private data structure pCFFT.
 *
 * GLOBAL
 *  VARIABLES:  None
 *--------------------------------------------------------------*/

dfr16_tCFFTStruct * dfr16CFFTCreateFFT_SIZE_128 (UInt16 options)
{

    dfr16_tCFFTStruct * pCFFT;

    pCFFT = (dfr16_tCFFTStruct *)memMallocIM(sizeof(dfr16_tCFFTStruct));

    dfr16CFFTInitFFT_SIZE_128 (pCFFT, options);

    return pCFFT;
}


void dfr16CFFTInitFFT_SIZE_128 (dfr16_tCFFTStruct * pCFFT, UInt16 options)
{
    pCFFT->n            = 128;
    pCFFT->options      = options;
    pCFFT->Twiddle      = (CFrac16 *)dfr16CFFTTwiddleFactorTable128;
    pCFFT->No_of_Stages = 7;
}


/*--------------------------------------------------------------
 * FILE:        dfr16.c
 *
 * FUNCTION:    dfr16_tCFFTStruct * dfr16CFFTCreateFFT_SIZE_256 (UInt16 options)
 *
 * DESCRIPTION: Initialization for N = 256 CFFT.
 *
 * ARGUMENTS:   options - 0 - FFT_DEFAULT_OPTIONS
 *                        1 - FFT_SCALE_RESULTS_BY_N
 *                        2 - FFT_SCALE_RESULTS_BY_DATA_SIZE
 *                        4 - FFT_INPUT_IS_BITREVERSED
 *                        8 - FFT_OUTPUT_IS_BITREVERSED
 *
 * RETURNS:     Pointer to private data structure pCFFT.
 *
 * GLOBAL
 *  VARIABLES:  None
 *--------------------------------------------------------------*/

dfr16_tCFFTStruct * dfr16CFFTCreateFFT_SIZE_256 (UInt16 options)
{
    dfr16_tCFFTStruct * pCFFT;

    pCFFT = (dfr16_tCFFTStruct *)memMallocIM(sizeof(dfr16_tCFFTStruct));

    dfr16CFFTInitFFT_SIZE_256 (pCFFT, options);

    return pCFFT;
}


void dfr16CFFTInitFFT_SIZE_256 (dfr16_tCFFTStruct * pCFFT, UInt16 options)
{
    pCFFT->n            = 256;
    pCFFT->options      = options;
    pCFFT->Twiddle      = (CFrac16 *)dfr16CFFTTwiddleFactorTable256;
    pCFFT->No_of_Stages = 8;
}


/*--------------------------------------------------------------
 * FILE:        dfr16.c
 *
 * FUNCTION:    dfr16_tCFFTStruct * dfr16CFFTCreateFFT_SIZE_512 (UInt16 options)
 *
 * DESCRIPTION: Initialization for N = 512 CFFT.
 *
 * ARGUMENTS:   options - 0 - FFT_DEFAULT_OPTIONS
 *                        1 - FFT_SCALE_RESULTS_BY_N
 *                        2 - FFT_SCALE_RESULTS_BY_DATA_SIZE
 *                        4 - FFT_INPUT_IS_BITREVERSED
 *                        8 - FFT_OUTPUT_IS_BITREVERSED
 *
 * RETURNS:     Pointer to private data structure pCFFT.
 *
 * GLOBAL
 *  VARIABLES:  None
 *--------------------------------------------------------------*/

dfr16_tCFFTStruct * dfr16CFFTCreateFFT_SIZE_512 (UInt16 options)
{

    dfr16_tCFFTStruct * pCFFT;

  pCFFT = (dfr16_tCFFTStruct *)memMallocIM(sizeof(dfr16_tCFFTStruct));

    dfr16CFFTInitFFT_SIZE_512 (pCFFT, options);

    return pCFFT;
}


void dfr16CFFTInitFFT_SIZE_512 (dfr16_tCFFTStruct * pCFFT, UInt16 options)
{
    pCFFT->n            = 512;
    pCFFT->options      = options;
    pCFFT->Twiddle      = (CFrac16 *)dfr16CFFTTwiddleFactorTable512;
    pCFFT->No_of_Stages = 9;
}


/*--------------------------------------------------------------
 * FILE:        dfr16.c
 *
 * FUNCTION:    dfr16_tCFFTStruct * dfr16CFFTCreateFFT_SIZE_1024 (UInt16 options)
 *
 * DESCRIPTION: Initialization for N = 1024 CFFT.
 *
 * ARGUMENTS:   options - 0 - FFT_DEFAULT_OPTIONS
 *                        1 - FFT_SCALE_RESULTS_BY_N
 *                        2 - FFT_SCALE_RESULTS_BY_DATA_SIZE
 *                        4 - FFT_INPUT_IS_BITREVERSED
 *                        8 - FFT_OUTPUT_IS_BITREVERSED
 *
 * RETURNS:     Pointer to private data structure pCFFT.
 *
 * GLOBAL
 *  VARIABLES:  None
 *--------------------------------------------------------------*/

dfr16_tCFFTStruct * dfr16CFFTCreateFFT_SIZE_1024 (UInt16 options)
{

    dfr16_tCFFTStruct * pCFFT;

  pCFFT = (dfr16_tCFFTStruct *)memMallocIM(sizeof(dfr16_tCFFTStruct));

    dfr16CFFTInitFFT_SIZE_1024 (pCFFT, options);

    return pCFFT;
}


void dfr16CFFTInitFFT_SIZE_1024 (dfr16_tCFFTStruct * pCFFT, UInt16 options)
{
    pCFFT->n            = 1024;
    pCFFT->options      = options;
    pCFFT->Twiddle      = (CFrac16 *)dfr16CFFTTwiddleFactorTable1024;
    pCFFT->No_of_Stages = 10;
}



/*--------------------------------------------------------------
 * FILE:        dfr16.c
 *
 * FUNCTION:    dfr16_tCFFTStruct * dfr16CFFTCreateFFT_SIZE_2048 (UInt16 options)
 *
 * DESCRIPTION: Initialization for N = 2048 CFFT.
 *
 * ARGUMENTS:   options - 0 - FFT_DEFAULT_OPTIONS
 *                        1 - FFT_SCALE_RESULTS_BY_N
 *                        2 - FFT_SCALE_RESULTS_BY_DATA_SIZE
 *                        4 - FFT_INPUT_IS_BITREVERSED
 *                        8 - FFT_OUTPUT_IS_BITREVERSED
 *
 * RETURNS:     Pointer to private data structure pCFFT.
 *
 * GLOBAL
 *  VARIABLES:  None
 *--------------------------------------------------------------*/

dfr16_tCFFTStruct * dfr16CFFTCreateFFT_SIZE_2048 (UInt16 options)
{


    dfr16_tCFFTStruct * pCFFT;

  pCFFT = (dfr16_tCFFTStruct *)memMallocIM(sizeof(dfr16_tCFFTStruct));

    dfr16CFFTInitFFT_SIZE_2048 (pCFFT, options);

    return pCFFT;
}


void dfr16CFFTInitFFT_SIZE_2048 (dfr16_tCFFTStruct * pCFFT, UInt16 options)
{
    pCFFT->n            = 2048;
    pCFFT->options      = options;
    pCFFT->Twiddle      = (CFrac16 *)dfr16CFFTTwiddleFactorTable2048;
    pCFFT->No_of_Stages = 11;
}


/*--------------------------------------------------------------
 * Revision History:
 *
 * VERSION    CREATED BY    MODIFIED BY      DATE       COMMENTS
 * -------    ----------    -----------      -----      --------
 *   0.1      Sandeep S        -          31-01-2000   For review.
 *   1.0          -            -          16-02-2000   Reviewed and
 *                                                     baselined.
 *
 *-------------------------------------------------------------*/
/*--------------------------------------------------------------
 * FILE:        dfr16.c
 *
 * FUNCTION:    void dfr16CFFTDestroy (dfr16_tCFFTStruct *pCFFT)
 *
 * DESCRIPTION: Destroys the memory allocated for private data
 *              structure.
 *
 * ARGUMENTS:   pCFFT - Pointer to private data structure pCFFT.
 *
 * RETURNS:     None
 *
 * GLOBAL
 *  VARIABLES:  None
 *--------------------------------------------------------------*/

void dfr16CFFTDestroy (dfr16_tCFFTStruct *pCFFT)
{

    if (pCFFT != NULL)
    {

    memFreeIM (pCFFT);
  }

    return;
}



/*--------------------------------------------------------------
 * Revision History:
 *
 * VERSION    CREATED BY    MODIFIED BY      DATE       COMMENTS
 * -------    ----------    -----------      -----      --------
 *   0.1      N R Prasad        -          27-01-2000   For review.
 *   1.0          -             -          16-02-2000   Reviewed and
 *                                                      baselined.
 *
 *-------------------------------------------------------------*/
/*--------------------------------------------------------------
 * FILE:        dfr16.c
 *
 * FUNCTION:    dfr16CFFT (dfr16_tCFFTStruct *pCFFT, CFrac16 *pX,
 *                         CFrac16 *pZ)
 *
 * DESCRIPTION: Computes N-point radix-2 decimation in time (DIT)
 *              complex FFT.
 *
 * ARGUMENTS:   pCFFT - Pointer to private data structure containing
 *                      options, length of fft N, pointer to twiddle
 *                      factor table, and no. of stages of FFT for a
 *                      given N.
 *              pX - Pointer to input data whose FFT is to be
 *                   calculated.
 *              pZ - Pointer to output containing FFT values of input.
 *                   If pZ = pX, inplace computation is done.
 *
 * RETURNS:     -1 - Indicates FAIL (due to FAIL in bit reverse)
 *              >=0 - Indicates the amount of scaling done in terms of
 *                    number of shifts.
 *
 * GLOBAL
 *  VARIABLES:  None
 *--------------------------------------------------------------*/
Result dfr16CFFTC (dfr16_tCFFTStruct *pCFFT, CFrac16 *pX,
                  CFrac16 *pZ)
{
    Int16 res;           /* Result indicating PASS or FAIL */
    Int16 FFT_Stage;     /* FFT stage-counter */
    Int16 Bfs;           /* Similar-butterfly counter */
    Int16 But_Top;       /* Offset for b-fly top */
    Int16 But_Bot;       /* Offset for b-fly bottom */
//    Int16 Bf_Size;       /* Size of butterfly */
    Int16 Gfs;           /* Counter to count the groups in first stage alone */
    Frac32 temp1, temp2; /* Temporary variables */
    Frac16 temp16, temp;
    CFrac16 *pZZ;        /* Temporrary pointer */
    CFrac16 *Twiddle;    /* Local variable to store Twiddle factor
                            table pointer */
    bool Sticky = false; /* Flag to store the sticky bit for Block floating
                            point method od scaling */
    Int16 ScaleBF = 0;   /* Variable to store the information on amount
                            of scaling done in block floating point method. */
    Int16 ii, N, Passes; /* Loop variable */
    bool SatBit = false; /* Variable to remember old saturation bit */
    UWord32 TempReg;



    SatBit = archGetSetSaturationMode (SatBit); /* Reset the saturation mode bit */

    Twiddle = pCFFT->Twiddle; /* Assign the twiddle-factor pointer */

    /*--------------------------------------------------------------*/
    /* Rearrange the input array in bit-reversed order, if required */
    /*--------------------------------------------------------------*/

    temp1 = (pCFFT->options) & FFT_INPUT_IS_BITREVERSED;

    pZZ = pX;
    if (temp1)
    {
        res = dfr16CbitrevC (pX, pZ, pCFFT->n); /* Invoke bit-rev with
                                                 inplace computation. */
        pZZ = pZ;
        if (res == FAIL)
        {
            /* Put back the original saturation bit */
            archGetSetSaturationMode (SatBit);
            return FAIL;
        }
    }


    /*-----------------*/
    /* FFT computation */
    /*-----------------*/

    N = (Int16) ((pCFFT->n)/2);

    if ((pCFFT->options) & FFT_SCALE_RESULTS_BY_DATA_SIZE)
    {
        /* Block Floating Point scaling */
        Sticky = false;
        for (ii = 0; ii < pCFFT->n; ii++)
        {
            if ((abs (pZZ[ii].real) >= 0x2000) ||
                (abs (pZZ[ii].imag) >= 0x2000))
            {
                Sticky = true;
                ScaleBF++;
                break;
            }
        }
    }


    /* Take out the first loop as it is just a plus-minus butterfly loop */
    if (((pCFFT->options) & FFT_SCALE_RESULTS_BY_N) || Sticky)
    {
        /* Look for scaling options */

        /* Auto Scaling */
        for (Gfs = 0; Gfs < N; Gfs++)
        {
            But_Top = Gfs;
            But_Bot = (Int16) (pCFFT->n/2 + Gfs);

            temp1 = L_deposit_h(pZZ[But_Top].real);
            TempReg = (UWord32) AddAndDivBy2(L_deposit_h(pZZ[But_Bot].real),temp1);
            pZ[But_Top].real = round((Word32) TempReg);
            pZ[But_Bot].real = round(L_sub(temp1,L_deposit_h(pZ[But_Top].real)));
            temp1 = L_deposit_h(pZZ[But_Top].imag);
            TempReg = (UWord32) AddAndDivBy2(temp1,L_deposit_h(pZZ[But_Bot].imag));
            pZ[But_Top].imag = round((Word32) TempReg);
            pZ[But_Bot].imag = round(L_sub(temp1,L_deposit_h(pZ[But_Top].imag)));

        }
    }
    else
    {
        /* No scaling */
        for (Gfs = 0; Gfs < N; Gfs++)
        {
            But_Top = Gfs;
            But_Bot = (Int16) (pCFFT->n/2 + Gfs);

            temp1 = L_deposit_h(pZZ[But_Top].real);
            pZ[But_Top].real = round(L_add(L_deposit_h(pZZ[But_Bot].real),temp1));
            pZ[But_Bot].real = round(L_sub(L_shl(temp1,1),L_deposit_h(pZ[But_Top].real)));
            temp1 = L_deposit_h(pZZ[But_Top].imag);
            pZ[But_Top].imag = round(L_add(temp1,L_deposit_h(pZZ[But_Bot].imag)));
            pZ[But_Bot].imag = round(L_sub(L_shl(temp1,1),L_deposit_h(pZ[But_Top].imag)));
        }
    }


    /* The remaining loops .... */
    temp16 = (Frac16) ((pCFFT->options) & FFT_SCALE_RESULTS_BY_N);

    for(FFT_Stage = 0; FFT_Stage < pCFFT->No_of_Stages-2; FFT_Stage++)
                                    /* Stage counter */
    {
         /* Look for scaling options */
        Sticky = false;
        if ((pCFFT->options) & FFT_SCALE_RESULTS_BY_DATA_SIZE)
        {
            for (ii = 0; ii < pCFFT->n; ii++)
            {
                if ((abs (pZ[ii].real) >= 0x2000) ||
                    (abs (pZ[ii].imag) >= 0x2000))
                {
                    Sticky = true;
                    ScaleBF++;
                    break;
                }
            }
        }

        /*--------------------*/
        /* Size of butterfly */
        /*--------------------*/
        But_Top = 0;
        N = N/2;
        But_Bot = N;

        Passes = shl(2, FFT_Stage);
        /*--------------------------------------------------*/
        /* Loop for counting butterflies in a stage */
        /*--------------------------------------------------*/
        for(Gfs = 0; Gfs < Passes; Gfs++)
        {
            for(Bfs = 0; Bfs < N; Bfs++)
            {

                if (temp16 || Sticky)
                {
                    temp  = pZ[But_Bot].real;
                    temp1 = L_mult(pCFFT->Twiddle[Gfs].real,pZ[But_Bot].real);            /* WrBr */
                    temp2 = L_deposit_h(pZ[But_Top].real);                                /* Ar */
                    temp1 = L_mac(temp1,pCFFT->Twiddle[Gfs].imag,pZ[But_Bot].imag);       /* WrBr+WiBi */
                    TempReg = (UWord32) AddAndDivBy2(temp1,temp2);
                    pZ[But_Top].real = round((Word32) TempReg);
                    pZ[But_Bot].real = round(L_sub(temp2,L_deposit_h(pZ[But_Top].real)));

                    temp1 = L_negate(L_mult(pCFFT->Twiddle[Gfs].imag,temp));    /*-WiBr*/
                    temp2 = L_deposit_h(pZ[But_Top].imag);            /*Ai*/
                    temp1 = L_mac(temp1,pCFFT->Twiddle[Gfs].real,pZ[But_Bot].imag);/*WiBr+WrBi*/
                    TempReg = (UWord32) AddAndDivBy2(temp2,temp1);
                    pZ[But_Top].imag = round((Word32) TempReg);
                    pZ[But_Bot].imag = round(L_sub(temp2,L_deposit_h(pZ[But_Top].imag)));
                }
                else
                {
                    /* No scaling */
                    temp  = pZ[But_Bot].real;
                    temp1 = L_mult(pCFFT->Twiddle[Gfs].real,pZ[But_Bot].real);            /* WrBr */
                    temp2 = L_deposit_h(pZ[But_Top].real);                                /* Ar */
                    temp1 = L_mac(temp1,pCFFT->Twiddle[Gfs].imag,pZ[But_Bot].imag);       /* WrBr+WiBi */
                    pZ[But_Top].real = round(L_add(temp1,temp2));                       /* WrBr+WiBi+Ar */
                    pZ[But_Bot].real = round(L_sub(L_shl(temp2,1),L_deposit_h(pZ[But_Top].real)));

                    temp1 = L_negate(L_mult(pCFFT->Twiddle[Gfs].imag,temp));    /*-WiBr*/
                    temp2 = L_deposit_h(pZ[But_Top].imag);            /*Ai*/
                    temp1 = L_mac(temp1,pCFFT->Twiddle[Gfs].real,pZ[But_Bot].imag);/*WiBr+WrBi*/
                    pZ[But_Top].imag = round(L_add(temp2,temp1));
                    pZ[But_Bot].imag = round(L_sub(L_shl(temp2,1),L_deposit_h(pZ[But_Top].imag)));
                } /*- End of B-fly computation -*/

                But_Top++;
                But_Bot++;

            } /* End of "But_Top" loop */

            But_Top = But_Bot;
            But_Bot = But_Bot + N;

        } /* End of "Bfs" loop */

    } /* End of "FFT_Stage" loop */

    /*-------------------------------*/
    /* Last Pass                     */
    /*-------------------------------*/

    Sticky = false;
    if ((pCFFT->options) & FFT_SCALE_RESULTS_BY_DATA_SIZE)
    {
        for (ii = 0; ii < pCFFT->n; ii++)
        {
            if ((abs (pZ[ii].real) >= 0x2000) ||
                (abs (pZ[ii].imag) >= 0x2000))
            {
                Sticky = true;
                ScaleBF++;
                break;
            }
        }
    }

    But_Top = 0;
    But_Bot = 1;
    N = (Int16) ((pCFFT->n)/2);

    for(Gfs = 0; Gfs < N; Gfs++)
    {

        if (temp16 || Sticky)
        {
            temp  = pZ[But_Bot].real;
            temp1 = L_mult(pCFFT->Twiddle[Gfs].real,pZ[But_Bot].real);            /* WrBr */
            temp2 = L_deposit_h(pZ[But_Top].real);                                /* Ar */
            temp1 = L_mac(temp1,pCFFT->Twiddle[Gfs].imag,pZ[But_Bot].imag);       /* WrBr+WiBi */
            TempReg = (UWord32) AddAndDivBy2(temp1,temp2);
            pZ[But_Top].real = round((Word32) TempReg);
            pZ[But_Bot].real = round(L_sub(temp2,L_deposit_h(pZ[But_Top].real)));

            temp1 = L_negate(L_mult(pCFFT->Twiddle[Gfs].imag,temp));    /*-WiBr*/
            temp2 = L_deposit_h(pZ[But_Top].imag);            /*Ai*/
            temp1 = L_mac(temp1,pCFFT->Twiddle[Gfs].real,pZ[But_Bot].imag);/*WiBr+WrBi*/
            TempReg = (UWord32) AddAndDivBy2(temp2,temp1);
            pZ[But_Top].imag = round((Word32) TempReg);
            pZ[But_Bot].imag = round(L_sub(temp2,L_deposit_h(pZ[But_Top].imag)));
        }
        else
        {
             /* No scaling */
            temp  = pZ[But_Bot].real;
            temp1 = L_mult(pCFFT->Twiddle[Gfs].real,pZ[But_Bot].real);            /* WrBr */
            temp2 = L_deposit_h(pZ[But_Top].real);                                /* Ar */
            temp1 = L_mac(temp1,pCFFT->Twiddle[Gfs].imag,pZ[But_Bot].imag);       /* WrBr+WiBi */
            pZ[But_Top].real = round(L_add(temp1,temp2));                       /* WrBr+WiBi+Ar */
            pZ[But_Bot].real = round(L_sub(L_shl(temp2,1),L_deposit_h(pZ[But_Top].real)));

            temp1 = L_negate(L_mult(pCFFT->Twiddle[Gfs].imag,temp));    /*-WiBr*/
            temp2 = L_deposit_h(pZ[But_Top].imag);            /*Ai*/
            temp1 = L_mac(temp1,pCFFT->Twiddle[Gfs].real,pZ[But_Bot].imag);/*WiBr+WrBi*/
            pZ[But_Top].imag = round(L_add(temp2,temp1));
            pZ[But_Bot].imag = round(L_sub(L_shl(temp2,1),L_deposit_h(pZ[But_Top].imag)));
        }

        But_Top += 2;
        But_Bot += 2;
    }


    /*--------------------------------------------------------------*/
    /* Rearrange the output array in bit-reversed order, if required */
    /*--------------------------------------------------------------*/
    temp16 = (Frac16) ((pCFFT->options) & FFT_OUTPUT_IS_BITREVERSED);
    if (!temp16)
    {
        res = dfr16CbitrevC (pZ, pZ, pCFFT->n); /* Invoke bit-rev with
                                                    inplace computation. */
        if (res == FAIL)
        {
            /* Put back the original saturation bit */
            archGetSetSaturationMode (SatBit);
            return FAIL;
        }
    }

    /*----------------------------------------------------------------*/
    /* Return the amount of scaling done in terms of number of shifts */
    /*----------------------------------------------------------------*/
    archGetSetSaturationMode (SatBit);
    temp16 = (Frac16) ((pCFFT->options) & FFT_SCALE_RESULTS_BY_N);
    if (temp16)
    {
        /* Scaling factor = N. Thus, no. of shifts = No_of_Stages */

        /* Put back the original saturation bit */
        return ((Result) (pCFFT->No_of_Stages));
    }
    else if ((pCFFT->options) & FFT_SCALE_RESULTS_BY_DATA_SIZE)
    {
        /* Put back the original saturation bit */
        return ((Result) ScaleBF);  /* Return amount of scaling done */
    }
    else
    {
        /* Put back the original saturation bit */
        return 0; /* No scaling done. Thus, no. of shifts = 0 */
    }

} /* End of CFFT */



/*******************************************************
* 16-bit Fractional Forward Fast Fourier Transforms
*******************************************************/

/*--------------------------------------------------------------
* Revision History:
*
* VERSION    CREATED BY    MODIFIED BY      DATE       COMMENTS
* -------    ----------    -----------      -----      --------
*   0.1      Sandeep Sehgal    -          10-02-2000   For review.
*
*-------------------------------------------------------------*/
/*--------------------------------------------------------------
* FILE:        dfr16.c
*
* FUNCTION:    dfr16RFFTCreate (UInt16 n, UInt16 options)
*
*
* DESCRIPTION: Filling the private data structure, to be accessed
* by Real FFT routine for computation.
*
*
* ARGUMENTS:   n - length of fft N,
*
*              options - options - The options are input bitreversed, o/p
*              bitreversed, scale the o/p.
*
*
*
* RETURNS:     Pointer to the structure dfr16_tRFFTStruct
*
*
* GLOBAL
*  VARIABLES:  None
*--------------------------------------------------------------*/

dfr16_tRFFTStruct * dfr16RFFTCreateFFT_SIZE_8 (UInt16 options)
{
    dfr16_tRFFTStruct *pRFFT;

    pRFFT=(dfr16_tRFFTStruct *) memMallocIM(sizeof(dfr16_tRFFTStruct));

  dfr16RFFTInitFFT_SIZE_8 (pRFFT, options);

    return pRFFT;
}


void dfr16RFFTInitFFT_SIZE_8 (dfr16_tRFFTStruct * pRFFT, UInt16 options)
{
    pRFFT->n            = 4;
    pRFFT->options      = options;
    pRFFT->Twiddle      = (CFrac16 *)dfr16RFFTTwiddleTable8;
    pRFFT->Twiddle_br   = (CFrac16 *)dfr16RFFTTwiddleTable8br;
    pRFFT->No_of_Stages = 2;
}


dfr16_tRFFTStruct * dfr16RFFTCreateFFT_SIZE_16 (UInt16 options)
{
    dfr16_tRFFTStruct *pRFFT;

    pRFFT=(dfr16_tRFFTStruct *) memMallocIM(sizeof(dfr16_tRFFTStruct));

  dfr16RFFTInitFFT_SIZE_16 (pRFFT, options);

    return pRFFT;
}


void dfr16RFFTInitFFT_SIZE_16 (dfr16_tRFFTStruct * pRFFT, UInt16 options)
{
    pRFFT->n            = 8;
    pRFFT->options      = options;
    pRFFT->Twiddle      = (CFrac16 *)dfr16RFFTTwiddleTable16;
    pRFFT->Twiddle_br   = (CFrac16 *)dfr16RFFTTwiddleTable16br;
    pRFFT->No_of_Stages = 3;
}



dfr16_tRFFTStruct * dfr16RFFTCreateFFT_SIZE_32 (UInt16 options)
{
    dfr16_tRFFTStruct *pRFFT;

    pRFFT=(dfr16_tRFFTStruct *) memMallocIM(sizeof(dfr16_tRFFTStruct));

  dfr16RFFTInitFFT_SIZE_32 (pRFFT, options);

    return pRFFT;
}


void dfr16RFFTInitFFT_SIZE_32 (dfr16_tRFFTStruct * pRFFT, UInt16 options)
{
    pRFFT->n            = 16;
    pRFFT->options      = options;
    pRFFT->Twiddle      = (CFrac16 *)dfr16RFFTTwiddleTable32;
    pRFFT->Twiddle_br   = (CFrac16 *)dfr16RFFTTwiddleTable32br;
    pRFFT->No_of_Stages = 4;
}


dfr16_tRFFTStruct * dfr16RFFTCreateFFT_SIZE_64 (UInt16 options)
{
    dfr16_tRFFTStruct *pRFFT;

    pRFFT=(dfr16_tRFFTStruct *) memMallocIM(sizeof(dfr16_tRFFTStruct));

  dfr16RFFTInitFFT_SIZE_64 (pRFFT, options);

    return pRFFT;
}


void dfr16RFFTInitFFT_SIZE_64 (dfr16_tRFFTStruct * pRFFT, UInt16 options)
{
    pRFFT->n            = 32;
    pRFFT->options      = options;
    pRFFT->Twiddle      = (CFrac16 *)dfr16RFFTTwiddleTable64;
    pRFFT->Twiddle_br   = (CFrac16 *)dfr16RFFTTwiddleTable64br;
    pRFFT->No_of_Stages = 5;
}



dfr16_tRFFTStruct * dfr16RFFTCreateFFT_SIZE_128 (UInt16 options)
{
    dfr16_tRFFTStruct *pRFFT;

    pRFFT=(dfr16_tRFFTStruct *) memMallocIM(sizeof(dfr16_tRFFTStruct));

  dfr16RFFTInitFFT_SIZE_128 (pRFFT, options);

    return pRFFT;
}


void dfr16RFFTInitFFT_SIZE_128 (dfr16_tRFFTStruct * pRFFT, UInt16 options)
{
    pRFFT->n            = 64;
    pRFFT->options      = options;
    pRFFT->Twiddle      = (CFrac16 *)dfr16RFFTTwiddleTable128;
    pRFFT->Twiddle_br   = (CFrac16 *)dfr16RFFTTwiddleTable128br;
    pRFFT->No_of_Stages = 6;
}


dfr16_tRFFTStruct * dfr16RFFTCreateFFT_SIZE_256 (UInt16 options)
{
    dfr16_tRFFTStruct *pRFFT;

    pRFFT=(dfr16_tRFFTStruct *) memMallocIM(sizeof(dfr16_tRFFTStruct));

  dfr16RFFTInitFFT_SIZE_256 (pRFFT, options);

    return pRFFT;
}


void dfr16RFFTInitFFT_SIZE_256 (dfr16_tRFFTStruct * pRFFT, UInt16 options)
{
    pRFFT->n            = 128;
    pRFFT->options      = options;
    pRFFT->Twiddle      = (CFrac16 *)dfr16RFFTTwiddleTable256;
    pRFFT->Twiddle_br   = (CFrac16 *)dfr16RFFTTwiddleTable256br;
    pRFFT->No_of_Stages = 7;
}


dfr16_tRFFTStruct * dfr16RFFTCreateFFT_SIZE_512 (UInt16 options)
{
    dfr16_tRFFTStruct *pRFFT;

    pRFFT=(dfr16_tRFFTStruct *) memMallocIM(sizeof(dfr16_tRFFTStruct));

  dfr16RFFTInitFFT_SIZE_512 (pRFFT, options);

    return pRFFT;
}


void dfr16RFFTInitFFT_SIZE_512 (dfr16_tRFFTStruct * pRFFT, UInt16 options)
{
    pRFFT->n            = 256;
    pRFFT->options      = options;
    pRFFT->Twiddle      = (CFrac16 *)dfr16RFFTTwiddleTable512;
    pRFFT->Twiddle_br   = (CFrac16 *)dfr16RFFTTwiddleTable512br;
    pRFFT->No_of_Stages = 8;
}


dfr16_tRFFTStruct * dfr16RFFTCreateFFT_SIZE_1024 (UInt16 options)
{
    dfr16_tRFFTStruct *pRFFT;

    pRFFT=(dfr16_tRFFTStruct *) memMallocIM(sizeof(dfr16_tRFFTStruct));

  dfr16RFFTInitFFT_SIZE_1024 (pRFFT, options);

    return pRFFT;
}


void dfr16RFFTInitFFT_SIZE_1024 (dfr16_tRFFTStruct * pRFFT, UInt16 options)
{
    pRFFT->n            = 512;
    pRFFT->options      = options;
    pRFFT->Twiddle      = (CFrac16 *)dfr16RFFTTwiddleTable1024;
    pRFFT->Twiddle_br   = (CFrac16 *)dfr16RFFTTwiddleTable1024br;
    pRFFT->No_of_Stages = 9;
}


dfr16_tRFFTStruct * dfr16RFFTCreateFFT_SIZE_2048 (UInt16 options)
{
    dfr16_tRFFTStruct *pRFFT;

    pRFFT=(dfr16_tRFFTStruct *) memMallocIM(sizeof(dfr16_tRFFTStruct));

  dfr16RFFTInitFFT_SIZE_2048 (pRFFT, options);

    return pRFFT;
}

void dfr16RFFTInitFFT_SIZE_2048 (dfr16_tRFFTStruct * pRFFT, UInt16 options)
{
    pRFFT->n            = 1024;
    pRFFT->options      = options;
    pRFFT->Twiddle      = (CFrac16 *)dfr16RFFTTwiddleTable2048;
    pRFFT->Twiddle_br   = (CFrac16 *)dfr16RFFTTwiddleTable2048br;
    pRFFT->No_of_Stages = 10;
}


/*--------------------------------------------------------------
* Revision History:
*
* VERSION    CREATED BY    MODIFIED BY      DATE       COMMENTS
* -------    ----------    -----------      -----      --------
*   0.1      Sandeep Sehgal    -          10-02-2000   For review.
*
*-------------------------------------------------------------*/
/*--------------------------------------------------------------
* FILE:        dfr16.c
*
* FUNCTION:    dfr16RFFTDestroy (dfr16_tRFFTStruct * pRFFT)
*
*
* DESCRIPTION: Free the memory allocated to the private data structure
*
*
* ARGUMENTS:   Pointer to the data structure dfr16_tRFFTStruct
*
*
*
* RETURNS:     void
*
*
* GLOBAL
*  VARIABLES:  None
*--------------------------------------------------------------*/
void dfr16RFFTDestroy ( dfr16_tRFFTStruct *pRFFT)
{
    if ( pRFFT != NULL)
        memFreeIM(pRFFT);

}


/*--------------------------------------------------------------
* VERSION    CREATED BY    MODIFIED BY      DATE       COMMENTS
* -------    ----------    -----------      -----      --------
*   0.1      N R Prasad         -          02-02-2000   For review.
*   0.2         -          Sandeep S       11-02-2000   converted to
*                            real FFT
*
*-------------------------------------------------------------*/
/*--------------------------------------------------------------
* FILE:        dfr16.c
*
* FUNCTION:    dfr16RFFT (dfr16_tRFFTStruct *pRFFT, Frac16 *pX,
*                         dfr16_sInplaceCRFFT *pZ)
*
* DESCRIPTION: Computes N-point radix-2 decimation in
* time (DIT)
*              real FFT.
*
* ARGUMENTS:   pRFFT - Pointer to private data
*                      structure containing
*                      options, length of fft N,
*                      pointer to twiddle
*                      factor table, and no. of
*                      stages of FFT for a
*                      given N.
*              pX - Pointer to input data whose
*              FFT is to be
*                   calculated.
*              pZ - Pointer to output data structure
*              containing FFT values of input.
*
* RETURNS:     -1 - Indicates FAIL (due to FAIL in bit reverse)
*              >=0 - Indicates the amount
*                    of scaling done in terms of number of shifts.
*
* GLOBAL
*  VARIABLES:  None
*--------------------------------------------------------------*/


Result dfr16RFFTC (dfr16_tRFFTStruct *pRFFT, Frac16 *pX, dfr16_sInplaceCRFFT *pZ)
{
    Int16 res=0;         /* Result indicating PASS or FAIL */
    Int16 FFT_Stage;     /* FFT stage-counter */
    Int16 Bfs;           /* Similar-butterfly counter */
    Int16 But_Top;       /* Offset for b-fly top */
    Int16 But_Bot;       /* Offset for b-fly bottom */
//    Int16 Bf_Size;      /* Size of butterfly */
    Int16 Passes;
    Int16 Gfs;           /* Offset to top of next b-fly */
    Frac32 temp1, temp2; /* Temporary variables */
    Frac16 temp16, temp;
    CFrac16 *pY, *pW, *Twiddle, *Twiddle_br;
    Int16 N, i, ii;
//    Int16 Index;
    bool Sticky = false; /* Flag to store the sticky bit for Block floating
                            point method od scaling */
    Int16 ScaleBF = 0;   /* Variable to store the information on amount*/
    bool SatBit = false; /* Variable to store original Saturation bit */

    UWord32  TempReg;

    Frac16 A,B,C,D;
    pY = (CFrac16 *) pX;  /* Type cast the pointer type */
    pW = (CFrac16 *) pZ;

    N =  (Int16) pRFFT->n;
    Twiddle = pRFFT->Twiddle;

    SatBit = archGetSetSaturationMode (SatBit);

    Twiddle    = pRFFT->Twiddle; /* Assign the twiddle-factor pointer */
    Twiddle_br = pRFFT->Twiddle_br;

    N = (Int16) ((pRFFT->n)/2);

    /* Take out the first loop as it is just a plus-minus butterfly loop */
    if ((pRFFT->options) & FFT_SCALE_RESULTS_BY_DATA_SIZE)
    {
        /* Block Floating Point scaling */
        Sticky = false;
        for (ii = 0; ii < pRFFT->n; ii++)
        {
            if ((abs (pY[ii].real) >= 0x2000) ||
                (abs (pY[ii].imag) >= 0x2000))
            {
                Sticky = true;
                ScaleBF++;
                break;
            }
        }
    }

    if (((pRFFT->options) & FFT_SCALE_RESULTS_BY_N) || Sticky)
    {
        /* Look for scaling options */

        /* Auto Scaling */
        for (Gfs = 0; Gfs < N; Gfs++)
        {
            But_Top = Gfs;
            But_Bot = (Int16) (pRFFT->n/2 + Gfs);

            temp1 = L_deposit_h(pY[But_Top].real);
            TempReg = (UWord32) AddAndDivBy2(L_deposit_h(pY[But_Bot].real),temp1);
            pW[But_Top].real = round((Word32) TempReg);
            pW[But_Bot].real = round(L_sub(temp1,L_deposit_h(pW[But_Top].real)));
            temp1 = L_deposit_h(pY[But_Top].imag);
            TempReg = (UWord32) AddAndDivBy2(temp1,L_deposit_h(pY[But_Bot].imag));
            pW[But_Top].imag = round((Word32) TempReg);
            pW[But_Bot].imag = round(L_sub(temp1,L_deposit_h(pW[But_Top].imag)));

        }
    }
    else
    {
        /* No scaling */
        for (Gfs = 0; Gfs < N; Gfs++)
        {
            But_Top = Gfs;
            But_Bot = (Int16) (pRFFT->n/2 + Gfs);

            temp1 = L_deposit_h(pY[But_Top].real);
            pW[But_Top].real = round(L_add(L_deposit_h(pY[But_Bot].real),temp1));
            pW[But_Bot].real = round(L_sub(L_shl(temp1,1),L_deposit_h(pW[But_Top].real)));
            temp1 = L_deposit_h(pY[But_Top].imag);
            pW[But_Top].imag = round(L_add(temp1,L_deposit_h(pY[But_Bot].imag)));
            pW[But_Bot].imag = round(L_sub(L_shl(temp1,1),L_deposit_h(pW[But_Top].imag)));
        }
    }


    /* The remaining loops .... */
    temp16 = (Frac16) ((pRFFT->options) & FFT_SCALE_RESULTS_BY_N); /* Look for scaling options */

    for(FFT_Stage = 0; FFT_Stage < pRFFT->No_of_Stages-2; FFT_Stage++)
                                    /* Stage counter */
    {
        Sticky = false;
        if ((pRFFT->options) & FFT_SCALE_RESULTS_BY_DATA_SIZE)
        {
            for (ii = 0; ii < pRFFT->n; ii++)
            {
                if ((abs (pW[ii].real) >= 0x2000) ||
                    (abs (pW[ii].imag) >= 0x2000))
                {
                    Sticky = true;
                    ScaleBF++;
                    break;
                }
            }
        }

        /*--------------------*/
        /* Size of butterfly */
        /*--------------------*/
        But_Top = 0;
        N = N/2;
        But_Bot = N;

        Passes = shl(2, FFT_Stage);
        /*--------------------------------------------------*/
        /* Loop for counting butterflies in a stage */
        /*--------------------------------------------------*/
        for(Gfs = 0; Gfs < Passes; Gfs++)
        {
            for(Bfs = 0; Bfs < N; Bfs++)
            {

                if (temp16 || Sticky)
                {
                    temp  = pW[But_Bot].real;
                    temp1 = L_mult(pRFFT->Twiddle_br[Gfs].real,pW[But_Bot].real);            /* WrBr */
                    temp2 = L_deposit_h(pW[But_Top].real);                                /* Ar */
                    temp1 = L_mac(temp1,pRFFT->Twiddle_br[Gfs].imag,pW[But_Bot].imag);       /* WrBr+WiBi */
                    TempReg = (UWord32) AddAndDivBy2(temp1,temp2);
                    pW[But_Top].real = round((Word32) TempReg);
                    pW[But_Bot].real = round(L_sub(temp2,L_deposit_h(pW[But_Top].real)));

                    temp1 = L_negate(L_mult(pRFFT->Twiddle_br[Gfs].imag,temp));    /*-WiBr*/
                    temp2 = L_deposit_h(pW[But_Top].imag);            /*Ai*/
                    temp1 = L_mac(temp1,pRFFT->Twiddle_br[Gfs].real,pW[But_Bot].imag);/*WiBr+WrBi*/
                    TempReg = (UWord32) AddAndDivBy2(temp2,temp1);
                    pW[But_Top].imag = round((Word32) TempReg);
                    pW[But_Bot].imag = round(L_sub(temp2,L_deposit_h(pW[But_Top].imag)));
                }
                else
                {
                    /* No scaling */
                    temp  = pW[But_Bot].real;
                    temp1 = L_mult(pRFFT->Twiddle_br[Gfs].real,pW[But_Bot].real);            /* WrBr */
                    temp2 = L_deposit_h(pW[But_Top].real);                                /* Ar */
                    temp1 = L_mac(temp1,pRFFT->Twiddle_br[Gfs].imag,pW[But_Bot].imag);       /* WrBr+WiBi */
                    pW[But_Top].real = round(L_add(temp1,temp2));                       /* WrBr+WiBi+Ar */
                    pW[But_Bot].real = round(L_sub(L_shl(temp2,1),L_deposit_h(pW[But_Top].real)));

                    temp1 = L_negate(L_mult(pRFFT->Twiddle_br[Gfs].imag,temp));    /*-WiBr*/
                    temp2 = L_deposit_h(pW[But_Top].imag);            /*Ai*/
                    temp1 = L_mac(temp1,pRFFT->Twiddle_br[Gfs].real,pW[But_Bot].imag);/*WiBr+WrBi*/
                    pW[But_Top].imag = round(L_add(temp2,temp1));
                    pW[But_Bot].imag = round(L_sub(L_shl(temp2,1),L_deposit_h(pW[But_Top].imag)));
                } /*- End of B-fly computation -*/

                But_Top++;
                But_Bot++;


            } /* End of "But_Top" loop */

            But_Top = But_Bot;
            But_Bot = But_Bot + N;

        } /* End of "Bfs" loop */

    } /* End of "FFT_Stage" loop */

    /*-------------------------------*/
    /* Last Pass                     */
    /*-------------------------------*/

    Sticky = false;
    if ((pRFFT->options) & FFT_SCALE_RESULTS_BY_DATA_SIZE)
    {
        for (ii = 0; ii < pRFFT->n; ii++)
        {
            if ((abs (pW[ii].real) >= 0x2000) ||
                (abs (pW[ii].imag) >= 0x2000))
            {
                Sticky = true;
                ScaleBF++;
                break;
            }
        }
    }

    But_Top = 0;
    But_Bot = 1;
    N = (Int16) (pRFFT ->n/2);
    for(Gfs = 0; Gfs < N; Gfs++)
    {

        if (temp16 || Sticky)
        {
            temp  = pW[But_Bot].real;
            temp1 = L_mult(pRFFT->Twiddle_br[Gfs].real,pW[But_Bot].real);            /* WrBr */
            temp2 = L_deposit_h(pW[But_Top].real);                                /* Ar */
            temp1 = L_mac(temp1,pRFFT->Twiddle_br[Gfs].imag,pW[But_Bot].imag);       /* WrBr+WiBi */
            TempReg = (UWord32) AddAndDivBy2(temp1,temp2);
            pW[But_Top].real = round((Word32) TempReg);
            pW[But_Bot].real = round(L_sub(temp2,L_deposit_h(pW[But_Top].real)));

            temp1 = L_negate(L_mult(pRFFT->Twiddle_br[Gfs].imag,temp));    /*-WiBr*/
            temp2 = L_deposit_h(pW[But_Top].imag);            /*Ai*/
            temp1 = L_mac(temp1,pRFFT->Twiddle_br[Gfs].real,pW[But_Bot].imag);/*WiBr+WrBi*/
            TempReg = (UWord32) AddAndDivBy2(temp2,temp1);
            pW[But_Top].imag = round((Word32) TempReg);
            pW[But_Bot].imag = round(L_sub(temp2,L_deposit_h(pW[But_Top].imag)));
        }
        else
        {
             /* No scaling */
            temp  = pW[But_Bot].real;
            temp1 = L_mult(pRFFT->Twiddle_br[Gfs].real,pW[But_Bot].real);            /* WrBr */
            temp2 = L_deposit_h(pW[But_Top].real);                                /* Ar */
            temp1 = L_mac(temp1,pRFFT->Twiddle_br[Gfs].imag,pW[But_Bot].imag);       /* WrBr+WiBi */
            pW[But_Top].real = round(L_add(temp1,temp2));                       /* WrBr+WiBi+Ar */
            pW[But_Bot].real = round(L_sub(L_shl(temp2,1),L_deposit_h(pW[But_Top].real)));

            temp1 = L_negate(L_mult(pRFFT->Twiddle_br[Gfs].imag,temp));    /*-WiBr*/
            temp2 = L_deposit_h(pW[But_Top].imag);            /*Ai*/
            temp1 = L_mac(temp1,pRFFT->Twiddle_br[Gfs].real,pW[But_Bot].imag);/*WiBr+WrBi*/
            pW[But_Top].imag = round(L_add(temp2,temp1));
            pW[But_Bot].imag = round(L_sub(L_shl(temp2,1),L_deposit_h(pW[But_Top].imag)));
        }

        But_Top += 2;
        But_Bot += 2;
    }


    /*--------------------------------------------------------------*/
    /* Rearrange the output array in bit-reversed order             */
    /*--------------------------------------------------------------*/
    res = dfr16CbitrevC (pW, pW, pRFFT->n); /* Invoke bit-rev with
                                              inplace computation. */
    if (res == FAIL)
    {
        /* Put back the original saturation bit */
        archGetSetSaturationMode (SatBit);
        return FAIL;
    }


    /*-------------------*/
    /* Post Processing   */
    /*-------------------*/

    Sticky = false;
    if ((pRFFT->options) & FFT_SCALE_RESULTS_BY_DATA_SIZE)
    {
        for (ii = 0; ii < pRFFT->n; ii++)
        {
            if ((abs (pW[ii].real) >= 0x2000) ||
                (abs (pW[ii].imag) >= 0x2000))
            {
                Sticky = true;
                ScaleBF++;
                break;
            }
        }
    }

    temp16 = (Frac16) ((pRFFT -> options) & FFT_SCALE_RESULTS_BY_N); /* Look for scaling options */
    N = (Int16) pRFFT ->n;

    A = pW[0].real;  /* add(pW[0].imag/2,pW[N].imag/2) */
    C = pW[0].imag;  /* add(pW[N].real/2,pW[0].real/2) */
    B = 0;           /* sub(pW[N].real,A) */

    temp1 = 0;       /* L_mult(B, Twiddle[0].imag) */
    temp2 = L_deposit_h(C); /* L_mac(temp1, C, Twiddle[0].real) */

    if (temp16 || Sticky)
    {
        TempReg = (UWord32) AddAndDivBy2(L_deposit_h(A), temp2);
        pW[0].real = round((Word32) TempReg);
        pW[0].imag = round(L_sub(L_deposit_h(A), temp2)/2);
    }
    else
    {
        pW[0].real = round(L_add(L_deposit_h(A), temp2));
        pW[0].imag = round(L_sub(L_deposit_h(A), temp2));
    }


    for(i = 1; i <= N/2; i++)
    {
        C = add(pW[i].imag/2,pW[N-i].imag/2);
        D = sub(pW[i].imag,C);
        A = add(pW[N-i].real/2,pW[i].real/2);
        B = sub(pW[N-i].real,A);

        temp1 = L_mult(B,Twiddle[i].imag);
        temp2 = L_mac(temp1,C,Twiddle[i].real);
        if (temp16 || Sticky)
        {
            TempReg = (UWord32) AddAndDivBy2(L_deposit_h(A), temp2);
            pW[i].real = round((Word32) TempReg);
            pW[N-i].real = round(L_sub(L_deposit_h(A),temp2)/2);
        }
        else
        {
            pW[i].real = round(L_add(L_deposit_h(A),temp2));
            pW[N-i].real = round(L_sub(L_deposit_h(A),temp2));
        }

        temp1 = L_negate(L_mult(C,Twiddle[i].imag));
        temp2 = L_mac(temp1,B,Twiddle[i].real);
        if (temp16 || Sticky)
        {
            TempReg = (UWord32) AddAndDivBy2(L_deposit_h(D), temp2);
            pW[i].imag = round((Word32) TempReg);
            pW[N-i].imag = round(L_sub(temp2,L_deposit_h(D))/2);
        }
        else
        {
            pW[i].imag = round(L_add(L_deposit_h(D),temp2));
            pW[N-i].imag = round(L_sub(temp2,L_deposit_h(D)));
        }

    }

    archGetSetSaturationMode (SatBit);    /* Set the saturation mode bit */
    temp16 = (Frac16) ((pRFFT->options) & FFT_SCALE_RESULTS_BY_N);

    if (temp16)
    {
        /* Scaling factor = N. Thus, no. of shifts = No_of_Stages */

        return ((Result) ((pRFFT->No_of_Stages) + 1));
    }
    else if ((pRFFT->options) & FFT_SCALE_RESULTS_BY_DATA_SIZE)
    {
        return ((Result) ScaleBF);  /* Return amount of scaling done */
    }
    else
    {
        return 0; /* No scaling done. Thus, no. of shifts = 0 */
    }

} /* End of RFFT */



/*******************************************************
* 16-bit Complex Fractional Inverse Fast Fourier Transforms
*******************************************************/

/*--------------------------------------------------------------
 * Revision History:
 *
 * VERSION    CREATED BY    MODIFIED BY      DATE       COMMENTS
 * -------    ----------    -----------      -----      --------
 *   0.1      Sandeep S        -          11-02-2000   For review.
 *   1.0          -            -          16-02-2000   Reviewed and
 *                                                     baselined.
 *
 *-------------------------------------------------------------*/
/*--------------------------------------------------------------
 * FILE:        dfr16.c
 *
 * FUNCTION:    void dfr16CIFFTDestroy (dfr16_tCFFTStruct *pCIFFT)
 *
 * DESCRIPTION: Destroys the memory of private data structure.
 *
 * ARGUMENTS:   pCIFFT - Pointer to private data structure pCIFFT.
 *
 * RETURNS:     None
 *
 * GLOBAL
 *  VARIABLES:  None
 *--------------------------------------------------------------*/

void dfr16CIFFTDestroy (dfr16_tCFFTStruct *pCIFFT)
{
   if (pCIFFT != NULL)
    {

    memFreeIM (pCIFFT);
  }

    return;
}



/*--------------------------------------------------------------
 * Revision History:
 *
 * VERSION    CREATED BY    MODIFIED BY      DATE       COMMENTS
 * -------    ----------    -----------      -----      --------
 *   0.1      N R Prasad        -          14-02-2000   For review.
 *   1.0          -             -          16-02-2000   Reviewed and
 *                                                      baselined.
 *
 *-------------------------------------------------------------*/
/*--------------------------------------------------------------
 * FILE:        dfr16.c
 *
 * FUNCTION:    dfr16CIFFT (dfr16_tCFFTStruct *pCIFFT, CFrac16 *pX,
 *                         CFrac16 *pZ)
 *
 * DESCRIPTION: Computes N-point radix-2 decimation in time (DIT)
 *              complex inverse FFT.
 *
 * ARGUMENTS:   pCIFFT - Pointer to private data structure containing
 *                       options, length of ifft N, pointer to twiddle
 *                       factor table, and no. of stages of IFFT for a
 *                       given N.
 *              pX - Pointer to input data whose CIFFT is to be
 *                   calculated.
 *              pZ - Pointer to output containing CIFFT values of input.
 *                   If pZ = pX, inplace computation is done.
 *
 * RETURNS:     -1 - Indicates FAIL (due to FAIL in bit reverse)
 *              >=0 - Indicates the amount of scaling done in terms of
 *                    number of shifts.
 *
 * GLOBAL
 *  VARIABLES:  None
 *--------------------------------------------------------------*/

Result dfr16CIFFTC (dfr16_tCFFTStruct *pCIFFT, CFrac16 *pX,
                  CFrac16 *pZ)
{
    Int16 res;           /* Result indicatinf PASS or FAIL */
    Int16 FFT_Stage;     /* FFT stage-counter */
    Int16 Bfs;           /* Similar-butterfly counter */
    Int16 But_Top;       /* Offset for b-fly top */
    Int16 But_Bot;       /* Offset for b-fly bottom */
//    Int16 Bf_Size;       /* Size of butterfly */
    Int16 Gfs;           /* Counter to count the groups in first stage alone */
    Frac32 temp1, temp2; /* Temporary variables */
    Frac16 temp16,temp;
    CFrac16 *pZZ;        /* Temporary pointer */
    CFrac16 *Twiddle;    /* Local variable to store Twiddle factor
                            table pointer */
    bool Sticky = false; /* Flag to store the sticky bit for Block floating
                            point method od scaling */
    Int16 ScaleBF = 0;   /* Variable to store the information on amount
                            of scaling done in block floating point method. */
    Int16 ii,N,Passes;            /* Loop variable */
    bool SatBit = false; /* Variable to remember old saturation bit */



    SatBit = archGetSetSaturationMode (SatBit); /* Reset the saturation mode bit */

    Twiddle = pCIFFT->Twiddle; /* Assign the twiddle-factor pointer */

    /*--------------------------------------------------------------*/
    /* Rearrange the input array in bit-reversed order, if required */
    /*--------------------------------------------------------------*/

    pZZ = pX;
    temp1 = (pCIFFT->options) & FFT_INPUT_IS_BITREVERSED;

    if (temp1)
    {
        res = dfr16CbitrevC (pX, pZ, pCIFFT->n); /* Invoke bit-rev with
                                                 inplace computation. */
        pZZ = pZ;
        if (res == FAIL)
        {
            /* Put back the original saturation bit */
            archGetSetSaturationMode (SatBit);
            return FAIL;
        }
    }


    /*-----------------*/
    /* FFT computation */
    /*-----------------*/

    N = (Int16) ((pCIFFT->n)/2);

    if ((pCIFFT->options) & FFT_SCALE_RESULTS_BY_DATA_SIZE)
    {
        /* Block Floating Point scaling */
        Sticky = false;
        for (ii = 0; ii < N; ii++)
        {
            if ((abs (pZZ[ii].real) >= 0x2000) ||
                (abs (pZZ[ii].imag) >= 0x2000))
            {
                Sticky = true;
                ScaleBF++;
                break;
            }
        }
    }

    /* Take out the first loop as it is just a plus-minus butterfly loop */
    if (((pCIFFT->options) & FFT_SCALE_RESULTS_BY_N) || Sticky)
    {
        /* Look for scaling options */

        /* Auto Scaling */
        for (Gfs = 0; Gfs < N; Gfs++)
        {
            But_Top = Gfs;
            But_Bot = (Int16) (pCIFFT->n/2 + Gfs);

            temp1 = L_deposit_h(pZZ[But_Top].real);
            pZ[But_Top].real = round(L_add(L_deposit_h(pZZ[But_Bot].real),temp1)/2);
            pZ[But_Bot].real = round(L_sub(temp1,L_deposit_h(pZ[But_Top].real)));
            temp1 = L_deposit_h(pZZ[But_Top].imag);
            pZ[But_Top].imag = round(L_add(temp1,L_deposit_h(pZZ[But_Bot].imag))/2);
            pZ[But_Bot].imag = round(L_sub(temp1,L_deposit_h(pZ[But_Top].imag)));

        }
    }
    else
    {
        /* No scaling */
        for (Gfs = 0; Gfs < N; Gfs++)
        {
            But_Top = Gfs;
            But_Bot = (Int16) (pCIFFT->n/2 + Gfs);

            temp1 = L_deposit_h(pZZ[But_Top].real);
            pZ[But_Top].real = round(L_add(L_deposit_h(pZZ[But_Bot].real),temp1));
            pZ[But_Bot].real = round(L_sub(L_shl(temp1,1),L_deposit_h(pZ[But_Top].real)));
            temp1 = L_deposit_h(pZZ[But_Top].imag);
            pZ[But_Top].imag = round(L_add(temp1,L_deposit_h(pZZ[But_Bot].imag)));
            pZ[But_Bot].imag = round(L_sub(L_shl(temp1,1),L_deposit_h(pZ[But_Top].imag)));
        }
    }


    /* The remaining loops .... */
    temp16 = (Frac16) ((pCIFFT->options) & FFT_SCALE_RESULTS_BY_N); /* Look for scaling options */

    for(FFT_Stage = 0; FFT_Stage < pCIFFT->No_of_Stages-2; FFT_Stage++)
                                    /* Stage counter */
    {
        Sticky = false;
        if ((pCIFFT->options) & FFT_SCALE_RESULTS_BY_DATA_SIZE)
        {
            for (ii = 0; ii < pCIFFT->n; ii++)
            {
                if ((abs (pZ[ii].real) >= 0x2000) ||
                    (abs (pZ[ii].imag) >= 0x2000))
                {
                    Sticky = true;
                    ScaleBF++;
                    break;
                }
            }
        }

        /*--------------------*/
        /* Size of butterfly */
        /*--------------------*/
        But_Top = 0;
        N = N/2;
        But_Bot = N;

        Passes = 2;
        Passes = shl(Passes,FFT_Stage);
        /*--------------------------------------------------*/
        /* Loop for counting butterflies in a stage */
        /*--------------------------------------------------*/
        for(Gfs = 0; Gfs < Passes; Gfs++)
        {
            for(Bfs = 0; Bfs < N; Bfs++)
            {

                if (temp16 || Sticky)
                {
                    temp  = pZ[But_Bot].real;
                    temp1 = L_mult(pCIFFT->Twiddle[Gfs].real,pZ[But_Bot].real);            /* WrBr */
                    temp2 = L_deposit_h(pZ[But_Top].real);                                /* Ar */
                    temp1 = L_mac(temp1,negate(pCIFFT->Twiddle[Gfs].imag),pZ[But_Bot].imag);       /* WrBr+WiBi */
                    pZ[But_Top].real = round(L_add(temp1,temp2)/2);                       /* WrBr+WiBi+Ar */
                    pZ[But_Bot].real = round(L_sub(temp2,L_deposit_h(pZ[But_Top].real)));

                    temp1 = L_negate(L_mult(negate(pCIFFT->Twiddle[Gfs].imag),temp));    /*-WiBr*/
                    temp2 = L_deposit_h(pZ[But_Top].imag);            /*Ai*/
                    temp1 = L_mac(temp1,pCIFFT->Twiddle[Gfs].real,pZ[But_Bot].imag);/*WiBr+WrBi*/
                    pZ[But_Top].imag = round(L_add(temp2,temp1)/2);
                    pZ[But_Bot].imag = round(L_sub(temp2,L_deposit_h(pZ[But_Top].imag)));
                }
                else
                {
                    /* No scaling */
                    temp  = pZ[But_Bot].real;
                    temp1 = L_mult(pCIFFT->Twiddle[Gfs].real,pZ[But_Bot].real);            /* WrBr */
                    temp2 = L_deposit_h(pZ[But_Top].real);                                /* Ar */
                    temp1 = L_mac(temp1,negate(pCIFFT->Twiddle[Gfs].imag),pZ[But_Bot].imag);       /* WrBr+WiBi */
                    pZ[But_Top].real = round(L_add(temp1,temp2));                       /* WrBr+WiBi+Ar */
                    pZ[But_Bot].real = round(L_sub(L_shl(temp2,1),L_deposit_h(pZ[But_Top].real)));

                    temp1 = L_negate(L_mult(negate(pCIFFT->Twiddle[Gfs].imag),temp));    /*-WiBr*/
                    temp2 = L_deposit_h(pZ[But_Top].imag);            /*Ai*/
                    temp1 = L_mac(temp1,pCIFFT->Twiddle[Gfs].real,pZ[But_Bot].imag);/*WiBr+WrBi*/
                    pZ[But_Top].imag = round(L_add(temp2,temp1));
                    pZ[But_Bot].imag = round(L_sub(L_shl(temp2,1),L_deposit_h(pZ[But_Top].imag)));
                } /*- End of B-fly computation -*/

                But_Top++;
                But_Bot++;

            } /* End of "But_Top" loop */

            But_Top = But_Bot;
            But_Bot = But_Bot + N;

        } /* End of "Bfs" loop */

    } /* End of "FFT_Stage" loop */

    /*-------------------------------*/
    /* Last Pass                     */
    /*-------------------------------*/
    Sticky = false;
    if ((pCIFFT->options) & FFT_SCALE_RESULTS_BY_DATA_SIZE)
    {
        for (ii = 0; ii < pCIFFT->n; ii++)
        {
            if ((abs (pZ[ii].real) >= 0x2000) ||
                (abs (pZ[ii].imag) >= 0x2000))
            {
                Sticky = true;
                ScaleBF++;
                break;
            }
        }
    }

    But_Top = 0;
    But_Bot = 1;
    N = (Int16) (pCIFFT ->n/2);
    for(Gfs = 0; Gfs < N; Gfs++)
    {

        if (temp16 || Sticky)
        {
            temp  = pZ[But_Bot].real;
            temp1 = L_mult(pCIFFT->Twiddle[Gfs].real,pZ[But_Bot].real);            /* WrBr */
            temp2 = L_deposit_h(pZ[But_Top].real);                                /* Ar */
            temp1 = L_mac(temp1,negate(pCIFFT->Twiddle[Gfs].imag),pZ[But_Bot].imag);       /* WrBr+WiBi */
            pZ[But_Top].real = round(L_add(temp1,temp2)/2);                       /* WrBr+WiBi+Ar */
            pZ[But_Bot].real = round(L_sub(temp2,L_deposit_h(pZ[But_Top].real)));

            temp1 = L_negate(L_mult(negate(pCIFFT->Twiddle[Gfs].imag),temp));    /*-WiBr*/
            temp2 = L_deposit_h(pZ[But_Top].imag);            /*Ai*/
            temp1 = L_mac(temp1,pCIFFT->Twiddle[Gfs].real,pZ[But_Bot].imag);/*WiBr+WrBi*/
            pZ[But_Top].imag = round(L_add(temp2,temp1)/2);
            pZ[But_Bot].imag = round(L_sub(temp2,L_deposit_h(pZ[But_Top].imag)));
        }
        else
        {
             /* No scaling */
            temp  = pZ[But_Bot].real;
            temp1 = L_mult(pCIFFT->Twiddle[Gfs].real,pZ[But_Bot].real);            /* WrBr */
            temp2 = L_deposit_h(pZ[But_Top].real);                                /* Ar */
            temp1 = L_mac(temp1,negate(pCIFFT->Twiddle[Gfs].imag),pZ[But_Bot].imag);       /* WrBr+WiBi */
            pZ[But_Top].real = round(L_add(temp1,temp2));                       /* WrBr+WiBi+Ar */
            pZ[But_Bot].real = round(L_sub(L_shl(temp2,1),L_deposit_h(pZ[But_Top].real)));

            temp1 = L_negate(L_mult(negate(pCIFFT->Twiddle[Gfs].imag),temp));    /*-WiBr*/
            temp2 = L_deposit_h(pZ[But_Top].imag);            /*Ai*/
            temp1 = L_mac(temp1,pCIFFT->Twiddle[Gfs].real,pZ[But_Bot].imag);/*WiBr+WrBi*/
            pZ[But_Top].imag = round(L_add(temp2,temp1));
            pZ[But_Bot].imag = round(L_sub(L_shl(temp2,1),L_deposit_h(pZ[But_Top].imag)));
        }

        But_Top += 2;
        But_Bot += 2;
    }

    /*--------------------------------------------------------------*/
    /* Rearrange the output array in bit-reversed order, if required */
    /*--------------------------------------------------------------*/
    temp16 = (Frac16) ((pCIFFT->options) & FFT_OUTPUT_IS_BITREVERSED);
    if (!temp16)
    {
        res = dfr16CbitrevC (pZ, pZ, pCIFFT->n); /* Invoke bit-rev with
                                                    inplace computation. */
        if (res == FAIL)
        {
            /* Put back the original saturation bit */
            archGetSetSaturationMode (SatBit);
            return FAIL;
        }
    }


    /*----------------------------------------------------------------*/
    /* Return the amount of scaling done in terms of number of shifts */
    /*----------------------------------------------------------------*/
    archGetSetSaturationMode (SatBit);
    temp16 = (Frac16) ((pCIFFT->options) & FFT_SCALE_RESULTS_BY_N);
    if (temp16)
    {
        /* Scaling factor = N. Thus, no. of shifts = No_of_Stages */

        /* Put back the original saturation bit */
        return ((Result) (pCIFFT->No_of_Stages));
    }
    else if ((pCIFFT->options) & FFT_SCALE_RESULTS_BY_DATA_SIZE)
    {
        /* Put back the original saturation bit */
        return ((Result) ScaleBF);  /* Return amount of scaling done */
    }
    else
    {
        /* Put back the original saturation bit */
        return 0; /* No scaling done. Thus, no. of shifts = 0 */
    }

} /* End of CIFFT */



/*******************************************************
* 16-bit Fractional Inverse Fast Fourier Transforms
*******************************************************/

/*--------------------------------------------------------------
* Revision History:
*
* VERSION    CREATED BY    MODIFIED BY      DATE       COMMENTS
* -------    ----------    -----------      -----      --------
*   0.1      Sandeep Sehgal    -          10-02-2000   For review.
*
*-------------------------------------------------------------*/
/*--------------------------------------------------------------
* FILE:        dfr16.c
*
* FUNCTION:    dfr16RIFFTDestroy (dfr16_tRFFTStruct * pRIFFT)
*
*
* DESCRIPTION: Free the memory allocated to the private data structure
*
*
* ARGUMENTS:   Pointer to the data structure dfr16_tRFFTStruct
*
*
*
* RETURNS:     void
*
*
* GLOBAL
*  VARIABLES:  None
*--------------------------------------------------------------*/

void    dfr16RIFFTDestroy  (dfr16_tRFFTStruct * pRIFFT)
{
    if ( pRIFFT != NULL)
        memFreeIM(pRIFFT);
}



/*--------------------------------------------------------------
* Revision History:
*
* VERSION    CREATED BY    MODIFIED BY      DATE       COMMENTS
* -------    ----------    -----------      -----      --------
*   0.1      N R Prasad         -          02-02-2000   For review.
*   0.2         -          Sandeep S       11-02-2000   converted to
*                            real FFT
*
*-------------------------------------------------------------*/
/*--------------------------------------------------------------
* FILE:        dfr16.c
*
* FUNCTION:    dfr16RIFFT (dfr16_tRFFTStruct *pRIFFT, Frac16 *pX,
*                         dfr16_sInplaceCRFFT *pZ)
*
* DESCRIPTION: Computes N-point radix-2 decimation in
* time (DIT)
*              real Inverse FFT, with a function call to real FFT.
*
* ARGUMENTS:   pRIFFT - Pointer to private data
*                      structure containing
*                      options, length of inverse fft N,
*                      pointer to twiddle
*                      factor table, and no. of
*                      stages of inverse FFT for a
*                      given N.
*              pX - Pointer to input data whose inverse
*              FFT is to be
*                   calculated.
*              pZ - Pointer to output data structure
*              containing inverse FFT values of input.
*
* RETURNS:     -1 - Indicates FAIL (due to FAIL in bit reverse)
*              >=0 - Indicates the amount
*                    of scaling done in terms of number of shifts.
*
* GLOBAL
*  VARIABLES:  None
*--------------------------------------------------------------*/

Result  dfr16RIFFTC (dfr16_tRFFTStruct * pRIFFT, dfr16_sInplaceCRFFT *pX, Frac16 *pZ)
{
    Int16 res=0;         /* Result indicating PASS or FAIL */
    Int16 FFT_Stage;     /* FFT stage-counter */
    Int16 Bfs;           /* Similar-butterfly counter */
    Int16 But_Top;       /* Offset for b-fly top */
    Int16 But_Bot;       /* Offset for b-fly bottom */
    Int16 Gfs;
//    Int16 Bf_Size;       /* Size of butterfly */
    Frac32 temp, temp1, temp2; /* Temporary variables */
    Frac16 temp16;
    Int16 Passes;
    bool SatBit = false; /*Variable to Store the state of Saturation bit */


    CFrac16 *pY, *Twiddle, *Twiddle_br;
    Int16 N, i, ii, Sticky = 0;
//    Int16 Index;
    Int16 ai, aN;
    Int16 ScaleBF = 0;

    pY = (CFrac16 *) pZ;  /* Type cast the pointer type */


    N =  (Int16) pRIFFT->n;
    Twiddle = pRIFFT->Twiddle;
    Twiddle_br = pRIFFT->Twiddle;

    SatBit = archGetSetSaturationMode (SatBit);

    if ((pRIFFT->options) & FFT_SCALE_RESULTS_BY_DATA_SIZE)
    {
        /* Block Floating Point scaling */
        Sticky = false;
        if ((abs (pX->z0) >= 0x2000) ||
            (abs (pX->zNDiv2) >= 0x2000))
        for (ii = 0; ii < (pRIFFT->n); ii++)
        {
            if ((abs (pX->cz[ii].real) >= 0x2000) ||
                (abs (pX->cz[ii].imag) >= 0x2000))
            {
                Sticky = true;
                ScaleBF++;
                break;
            }
        }
    }

    if (((pRIFFT->options) & FFT_SCALE_RESULTS_BY_N) || Sticky)
    {
      /* Auto Scaling or Block Floating Point */

      temp16 = add((pX->z0)/2, (pX->zNDiv2)/2);
      pY[0].imag = sub((pX->z0)/2, (pX->zNDiv2)/2);
      pY[0].real = temp16;

      for (i=1; i < N/2; i++)
      {
          ai = i - 1;
          aN = N - i - 1;
          temp1 = L_sub(L_mult ((pX->cz[ai].real), -Twiddle[i].imag),
                        L_mult ((pX->cz[ai].imag), Twiddle[i].real));
          temp2 = L_add(L_mult ((pX->cz[aN].real), -Twiddle[i].imag),
                        L_mult ((pX->cz[aN].imag), Twiddle[i].real));
          temp1 = L_sub(temp1, temp2);
          temp16 = add(pX->cz[ai].real, pX->cz[aN].real);
          temp2 = L_deposit_h(temp16);
          pY[i].real = round(L_add(temp1, temp2)/2);

          temp1 = L_add(L_mult ((pX->cz[ai].imag), -Twiddle[i].imag),
                        L_mult ((pX->cz[ai].real), Twiddle[i].real));
          temp2 = L_sub(L_mult ((pX->cz[aN].imag), -Twiddle[i].imag),
                        L_mult ((pX->cz[aN].real), Twiddle[i].real));
          temp1 = L_add(temp1, temp2);
          temp16 = sub(pX->cz[ai].imag, pX->cz[aN].imag);
          temp2 = L_deposit_h(temp16);
          pY[i].imag = round( L_add(temp1, temp2)/2);

          temp1 = L_sub(L_mult ((pX->cz[ai].imag), Twiddle[i].real),
                        L_mult ((pX->cz[ai].real), -Twiddle[i].imag));
          temp2 = L_add(L_mult ((pX->cz[aN].real), -Twiddle[i].imag),
                        L_mult ((pX->cz[aN].imag), Twiddle[i].real));
          temp1 = L_add(temp1, temp2);
          temp16 = add(pX->cz[ai].real, pX->cz[aN].real);
          temp2 = L_deposit_h(temp16);
          pY[N-i].real = round(L_add(temp1, temp2)/2);

          temp1 = L_add( L_mult ((pX->cz[ai].imag), -Twiddle[i].imag),
                         L_mult ((pX->cz[ai].real), Twiddle[i].real));
          temp2 = L_sub( L_mult ((pX->cz[aN].imag), -Twiddle[i].imag),
                         L_mult ((pX->cz[aN].real), Twiddle[i].real));
          temp1 = L_add(temp1, temp2);
          temp16 = sub(pX->cz[aN].imag, pX->cz[ai].imag);
          temp2 = L_deposit_h(temp16);
          pY[N-i].imag = round(L_add(temp1, temp2)/2);

       }

       pY[N/2].real = pX->cz[N/2-1].real;
       pY[N/2].imag = -(pX->cz[N/2-1].imag);
    }
    else
    {
        /* No Scaling */

        temp16 = add((pX->z0), (pX->zNDiv2));
        pY[0].imag = sub((pX->z0), (pX->zNDiv2));
        pY[0].real = temp16;

        for (i=1; i < N/2; i++)
        {
          ai = i - 1;
          aN = N - i - 1;
          temp1 = L_sub(L_mult ((pX->cz[ai].real), -Twiddle[i].imag),
                        L_mult ((pX->cz[ai].imag), Twiddle[i].real));
          temp2 = L_add(L_mult ((pX->cz[aN].real), -Twiddle[i].imag),
                        L_mult ((pX->cz[aN].imag), Twiddle[i].real));
          temp1 = L_sub(temp1, temp2);
          temp16 = add(pX->cz[ai].real, pX->cz[aN].real);
          temp2 = L_deposit_h(temp16);
          pY[i].real = round(L_add(temp1, temp2));

          temp1 = L_add(L_mult ((pX->cz[ai].imag), -Twiddle[i].imag),
                        L_mult ((pX->cz[ai].real), Twiddle[i].real));
          temp2 = L_sub(L_mult ((pX->cz[aN].imag), -Twiddle[i].imag),
                        L_mult ((pX->cz[aN].real), Twiddle[i].real));
          temp1 = L_add(temp1, temp2);
          temp16 = sub(pX->cz[ai].imag, pX->cz[aN].imag);
          temp2 = L_deposit_h(temp16);
          pY[i].imag = round(L_add(temp1, temp2));

          temp1 = L_sub(L_mult ((pX->cz[ai].imag), Twiddle[i].real),
                        L_mult ((pX->cz[ai].real), -Twiddle[i].imag));
          temp2 = L_add(L_mult ((pX->cz[aN].real), -Twiddle[i].imag),
                        L_mult ((pX->cz[aN].imag), Twiddle[i].real));
          temp1 = L_add(temp1, temp2);
          temp16 = add(pX->cz[ai].real, pX->cz[aN].real);
          temp2 = L_deposit_h(temp16);
          pY[N-i].real = round(L_add(temp1, temp2));

          temp1 = L_add(L_mult ((pX->cz[ai].imag), -Twiddle[i].imag),
                        L_mult ((pX->cz[ai].real), Twiddle[i].real));
          temp2 = L_sub(L_mult ((pX->cz[aN].imag), -Twiddle[i].imag),
                        L_mult ((pX->cz[aN].real), Twiddle[i].real));
          temp1 = L_add(temp1, temp2);
          temp16 = sub(pX->cz[aN].imag, pX->cz[ai].imag);
          temp2 = L_deposit_h(temp16);
          pY[N-i].imag = round(L_add(temp1, temp2));

        }

        pY[N/2].real = 2*pX->cz[N/2-1].real;
        pY[N/2].imag = -2*(pX->cz[N/2-1].imag);
     }


    /************************************************************/
    /* Computation of n/2 length complex FFT                    */
    /************************************************************/


    /*-----------------*/
    /* FFT computation */
    /*-----------------*/
    N = (Int16) ((pRIFFT->n)/2);

    /* Take out the first loop as it is just a plus-minus butterfly loop */
    if ((pRIFFT->options) & FFT_SCALE_RESULTS_BY_DATA_SIZE)
    {
        /* Block Floating Point scaling */
        Sticky = false;
        for (ii = 0; ii < pRIFFT->n; ii++)
        {
            if ((abs (pY[ii].real) >= 0x2000) ||
                (abs (pY[ii].imag) >= 0x2000))
            {
                Sticky = true;
                ScaleBF++;
                break;
            }
        }
    }

    if (((pRIFFT->options) & FFT_SCALE_RESULTS_BY_N) || Sticky)
    {
        /* Look for scaling options */

        /* Auto Scaling */
        for (Gfs = 0; Gfs < N; Gfs++)
        {
            But_Top = Gfs;
            But_Bot = (Int16) (pRIFFT->n/2 + Gfs);

            temp1 = L_deposit_h(pY[But_Top].real);
            pY[But_Top].real = round(L_add(L_deposit_h(pY[But_Bot].real), temp1)/2);
            pY[But_Bot].real = round(L_sub(temp1, L_deposit_h(pY[But_Top].real)));
            temp1 = L_deposit_h(pY[But_Top].imag);
            pY[But_Top].imag = round(L_add(temp1, L_deposit_h(pY[But_Bot].imag))/2);
            pY[But_Bot].imag = round(L_sub(temp1, L_deposit_h(pY[But_Top].imag)));

        }
    }
    else
    {
        /* No scaling */
        for (Gfs = 0; Gfs < N; Gfs++)
        {
            But_Top = Gfs;
            But_Bot = (Int16) (pRIFFT->n/2 + Gfs);

            temp1 = L_deposit_h(pY[But_Top].real);
            pY[But_Top].real = round(L_add(L_deposit_h(pY[But_Bot].real), temp1));
            pY[But_Bot].real = round(L_sub(L_shl(temp1, 1), L_deposit_h(pY[But_Top].real)));
            temp1 = L_deposit_h(pY[But_Top].imag);
            pY[But_Top].imag = round(L_add(temp1, L_deposit_h(pY[But_Bot].imag)));
            pY[But_Bot].imag = round(L_sub(L_shl(temp1,1), L_deposit_h(pY[But_Top].imag)));
        }
    }


    /* The remaining loops .... */
    temp16 = (Frac16) ((pRIFFT->options) & FFT_SCALE_RESULTS_BY_N); /* Look for scaling options */

    for(FFT_Stage = 0; FFT_Stage < pRIFFT->No_of_Stages-2; FFT_Stage++)
                                    /* Stage counter */
    {
        Sticky = false;
        if ((pRIFFT->options) & FFT_SCALE_RESULTS_BY_DATA_SIZE)
        {
            for (ii = 0; ii < pRIFFT->n; ii++)
            {
                if ((abs (pY[ii].real) >= 0x2000) ||
                    (abs (pY[ii].imag) >= 0x2000))
                {
                    Sticky = true;
                    ScaleBF++;
                    break;
                }
            }
        }

        /*--------------------*/
        /* Size of butterfly */
        /*--------------------*/
        But_Top = 0;
        N = N/2;
        But_Bot = N;

        Passes = shl(2, FFT_Stage);
        /*--------------------------------------------------*/
        /* Loop for counting butterflies in a stage */
        /*--------------------------------------------------*/
        for(Gfs = 0; Gfs < Passes; Gfs++)
        {
            for(Bfs = 0; Bfs < N; Bfs++)
            {

                if (temp16 || Sticky)
                {
                    temp  = pY[But_Bot].real;
                    temp1 = L_mult(pRIFFT->Twiddle_br[Gfs].real,pY[But_Bot].real);            /* WrBr */
                    temp2 = L_deposit_h(pY[But_Top].real);                                /* Ar */
                    temp1 = L_mac(temp1,negate(pRIFFT->Twiddle_br[Gfs].imag),pY[But_Bot].imag);       /* WrBr+WiBi */
                    pY[But_Top].real = round(L_add(temp1,temp2)/2);                       /* WrBr+WiBi+Ar */
                    pY[But_Bot].real = round(L_sub(temp2,L_deposit_h(pY[But_Top].real)));

                    temp1 = L_negate(L_mult(negate(pRIFFT->Twiddle_br[Gfs].imag),(Frac16) temp));    /*-WiBr*/
                    temp2 = L_deposit_h(pY[But_Top].imag);            /*Ai*/
                    temp1 = L_mac(temp1,pRIFFT->Twiddle_br[Gfs].real,pY[But_Bot].imag);/*WiBr+WrBi*/
                    pY[But_Top].imag = round(L_add(temp2,temp1)/2);
                    pY[But_Bot].imag = round(L_sub(temp2,L_deposit_h(pY[But_Top].imag)));
                }
                else
                {
                    /* No scaling */
                    temp  = pY[But_Bot].real;
                    temp1 = L_mult(pRIFFT->Twiddle_br[Gfs].real,pY[But_Bot].real);            /* WrBr */
                    temp2 = L_deposit_h(pY[But_Top].real);                                /* Ar */
                    temp1 = L_mac(temp1,negate(pRIFFT->Twiddle_br[Gfs].imag),pY[But_Bot].imag);       /* WrBr+WiBi */
                    pY[But_Top].real = round(L_add(temp1,temp2));                       /* WrBr+WiBi+Ar */
                    pY[But_Bot].real = round(L_sub(L_shl(temp2,1),L_deposit_h(pY[But_Top].real)));

                    temp1 = L_negate(L_mult(negate(pRIFFT->Twiddle_br[Gfs].imag), (Frac16) temp));    /*-WiBr*/
                    temp2 = L_deposit_h(pY[But_Top].imag);            /*Ai*/
                    temp1 = L_mac(temp1,pRIFFT->Twiddle_br[Gfs].real,pY[But_Bot].imag);/*WiBr+WrBi*/
                    pY[But_Top].imag = round(L_add(temp2,temp1));
                    pY[But_Bot].imag = round(L_sub(L_shl(temp2,1),L_deposit_h(pY[But_Top].imag)));
                } /*- End of B-fly computation -*/

                But_Top++;
                But_Bot++;


            } /* End of "But_Top" loop */

            But_Top = But_Bot;
            But_Bot = But_Bot + N;

        } /* End of "Bfs" loop */

    } /* End of "FFT_Stage" loop */

    /*-------------------------------*/
    /* Last Pass                     */
    /*-------------------------------*/
    Sticky = false;
    if ((pRIFFT->options) & FFT_SCALE_RESULTS_BY_DATA_SIZE)
    {
        for (ii = 0; ii < pRIFFT->n; ii++)
        {
            if ((abs (pY[ii].real) >= 0x2000) ||
                (abs (pY[ii].imag) >= 0x2000))
            {
                Sticky = true;
                ScaleBF++;
                break;
            }
        }
    }

    But_Top = 0;
    But_Bot = 1;
    N = (Int16) pRIFFT ->n/2;
    for(Gfs = 0; Gfs < N; Gfs++)
    {

        if (temp16 || Sticky)
        {
            temp  = pY[But_Bot].real;
            temp1 = L_mult(pRIFFT->Twiddle_br[Gfs].real,pY[But_Bot].real);            /* WrBr */
            temp2 = L_deposit_h(pY[But_Top].real);                                /* Ar */
            temp1 = L_mac(temp1,negate(pRIFFT->Twiddle_br[Gfs].imag),pY[But_Bot].imag);       /* WrBr+WiBi */
            pY[But_Top].real = round(L_add(temp1,temp2)/2);                       /* WrBr+WiBi+Ar */
            pY[But_Bot].real = round(L_sub(temp2,L_deposit_h(pY[But_Top].real)));

            temp1 = L_negate(L_mult(negate(pRIFFT->Twiddle_br[Gfs].imag),(Frac16) temp));    /*-WiBr*/
            temp2 = L_deposit_h(pY[But_Top].imag);            /*Ai*/
            temp1 = L_mac(temp1,pRIFFT->Twiddle_br[Gfs].real,pY[But_Bot].imag);/*WiBr+WrBi*/
            pY[But_Top].imag = round(L_add(temp2,temp1)/2);
            pY[But_Bot].imag = round(L_sub(temp2,L_deposit_h(pY[But_Top].imag)));
        }
        else
        {
             /* No scaling */
            temp  = pY[But_Bot].real;
            temp1 = L_mult(pRIFFT->Twiddle_br[Gfs].real,pY[But_Bot].real);            /* WrBr */
            temp2 = L_deposit_h(pY[But_Top].real);                                /* Ar */
            temp1 = L_mac(temp1,negate(pRIFFT->Twiddle_br[Gfs].imag),pY[But_Bot].imag);       /* WrBr+WiBi */
            pY[But_Top].real = round(L_add(temp1,temp2));                       /* WrBr+WiBi+Ar */
            pY[But_Bot].real = round(L_sub(L_shl(temp2,1),L_deposit_h(pY[But_Top].real)));

            temp1 = L_negate(L_mult(negate(pRIFFT->Twiddle_br[Gfs].imag),(Frac16) temp));    /*-WiBr*/
            temp2 = L_deposit_h(pY[But_Top].imag);            /*Ai*/
            temp1 = L_mac(temp1,pRIFFT->Twiddle_br[Gfs].real,pY[But_Bot].imag);/*WiBr+WrBi*/
            pY[But_Top].imag = round(L_add(temp2,temp1));
            pY[But_Bot].imag = round(L_sub(L_shl(temp2,1),L_deposit_h(pY[But_Top].imag)));
        }

        But_Top += 2;
        But_Bot += 2;
    }

    res = dfr16CbitrevC (pY, pY, pRIFFT -> n); /* Invoke bit-rev with
                                                 inplace computation. */
    if (res == FAIL)
    {
        archGetSetSaturationMode (SatBit);
        return FAIL;
    }


    archGetSetSaturationMode (SatBit);    /* Set the saturation mode bit */
    temp16 = (Frac16) ((pRIFFT->options) & FFT_SCALE_RESULTS_BY_N);
    if (temp16)
    {
        /* Scaling factor = N. Thus, no. of shifts = No_of_Stages */

        return ((Result) ((pRIFFT->No_of_Stages)+1));
    }
    else if ((pRIFFT->options) & FFT_SCALE_RESULTS_BY_DATA_SIZE)
    {
        return ((Result) ScaleBF);  /* Return amount of scaling done */
    }
    else
    {
        return 0; /* No scaling done. Thus, no. of shifts = 0 */
    }

} /* End of RIFFT */




/******************************************************
* 16-bit Fractional Fast Fourier Transform Utilities
*******************************************************/
/*--------------------------------------------------------------
 * Revision History:
 *
 * VERSION    CREATED BY    MODIFIED BY      DATE       COMMENTS
 * -------    ----------    -----------      -----      --------
 *   0.1      Sandeep S         -          26-01-2000   For review.
 *   1.0          -             -          16-02-2000   Reviewed and
 *                                                      baselined.
 *
 *-------------------------------------------------------------*/
/*--------------------------------------------------------------
 * FILE:        dfr16.c
 *
 * FUNCTION:    dfr16CbitrevC (CFrac16 *pX, CFrac16 *pZ, UInt16 n)
 *
 * DESCRIPTION: Arranges the input array in the bit reversed order.
 *
 * ARGUMENTS:   pX - Pointer to input data which is to be arranged in
 *                   the bit reversed order.
 *              pZ - Pointer to output buffer containing the bit
 *                   reversed output.
 *              n  - Length of input buffer.
 *
 * RETURNS:     FAIL = -1 - Indicates FAIL (due to FAIL in bit reverse).
 *              PASS =  0 - Indicates PASS.
 *
 * GLOBAL
 *  VARIABLES:  None
 *--------------------------------------------------------------*/

Result  dfr16CbitrevC (CFrac16 *pX, CFrac16 *pZ, UInt16 n)
{
    Frac16 i,j,k,real_tmp,imag_tmp;

  if ((n <= 0) || (n > MAX_VECTOR_LEN))

    return FAIL;

    for (i=0,j=0;i<(n-1);i=add(i,1))
    {

      if (j>=i)
        {
        real_tmp= (*(pX+i)).real;
        (*(pZ+i)).real=(*(pX+j)).real;
         (*(pZ+j)).real=real_tmp;

        imag_tmp= (*(pX+i)).imag;
        (*(pZ+i)).imag=(*(pX+j)).imag;
           (*(pZ+j)).imag=imag_tmp;
        }

        k=shr((Word16) n,1);

        while (j>=k)
        {
            j=sub(j,k);

            k=shr(k,1);
        }
        j=add(j,k);

    }

    (*(pZ+n-1)).real=(*(pX+n-1)).real;
    (*(pZ+n-1)).imag=(*(pX+n-1)).imag;

    return PASS;

}



/*******************************************************
* 16-bit Fractional FIR Filters
*******************************************************/
dfr16_tFirStruct * dfr16FIRCreate (Frac16 *pC, UInt16 n)
{
  dfr16_tFirStruct * pFir;
//  UInt16             i;
//  Frac16           * pNewC;

  pFir = (dfr16_tFirStruct *)memMallocEM(sizeof(dfr16_tFirStruct));

  if(pFir != NULL)
  {
    pFir -> pC              = (Frac16 *)memMallocIM(n * sizeof(Frac16));

    pFir -> pHistory        = (Frac16 *)memMallocAlignedEM(n * sizeof(Frac16));

    if (pFir -> pC != NULL && pFir -> pHistory != NULL)
    {
      /* All memory allocations have succeeded, so initialize structure */

      dfr16FIRInit (pFir, pC, n);
    }
    else
    {
      if (pFir -> pC != NULL)
      {
        memFreeIM (pFir -> pC);
      }

      if (pFir -> pHistory != NULL)
      {
        memFreeEM (pFir -> pHistory);
      }

      memFreeEM (pFir);
      pFir = NULL;
    }
  }

  return pFir;
}


void dfr16FIRInit (dfr16_tFirStruct * pFIR, Frac16 *pC, UInt16 n)
{
  UInt16                 i;
  Frac16               * pNewC;
  dfr16_tFirStructPriv * pFIRPriv = (dfr16_tFirStructPriv *)pFIR;

  assert (pFIR != NULL);
  assert (pFIR -> pC != NULL);
  assert (pFIR -> pHistory != NULL);

  pFIRPriv -> n               = n;

  pFIRPriv -> bCanUseModAddr = memIsAligned(pFIRPriv -> pHistory, n*sizeof(Frac16));

  pFIRPriv -> bCanUseDualMAC = pFIRPriv -> bCanUseModAddr && memIsIM(pFIRPriv -> pC);

  pFIRPriv -> pNextHistoryAddr = pFIRPriv -> pHistory;

  pNewC = pFIRPriv -> pC + n-1;

  for (i=0; i < pFIRPriv -> n; i++)
  {
    /* Clear history buffer */
    *(pFIRPriv -> pNextHistoryAddr++) = 0;

    /* Reverse order of coefficients so that dual parallel MAC can be used */
    *(pNewC--) = *pC++;
  }

  pFIRPriv -> pNextHistoryAddr = pFIRPriv -> pHistory;
}


void    dfr16FIRDestroy    (dfr16_tFirStruct *pFIR)
{
  if (pFIR != NULL) {

    if (pFIR -> pC != NULL)
    {
      memFreeIM (pFIR -> pC);
    }

    if (pFIR -> pHistory != NULL)
    {
      memFreeEM (pFIR -> pHistory);
    }

    memFreeEM (pFIR);
  }
}


void  dfr16FIRHistory (dfr16_tFirStruct *pFIR, Frac16 *pX)
{
  UInt16    i;

  ((dfr16_tFirStructPriv *)pFIR) -> pNextHistoryAddr = (Frac16 *)(dfr16_tFirStructPriv *)pFIR -> pHistory;

  for (i=0; i < ((dfr16_tFirStructPriv *)pFIR) -> n; i++)
  {
    /* Preload history buffer */
    *(((dfr16_tFirStructPriv *)pFIR) -> pNextHistoryAddr++) = *pX++;
  }

  ((dfr16_tFirStructPriv *)pFIR) -> pNextHistoryAddr = ((dfr16_tFirStructPriv *)pFIR) -> pHistory;
}


void  dfr16FIRC (dfr16_tFirStruct *pFIR, Frac16 *pX, Frac16 *pZ, UInt16 n)
{
  UInt16                 i, j;
  Frac16               * pCoefs;
  Frac16               * pMem;
  Frac16               * pMemEnd;
//  Frac32                 temp1;
  Frac32                 total32;
  dfr16_tFirStructPriv * pFIRPriv = (dfr16_tFirStructPriv *)pFIR;

  pMem    = pFIRPriv -> pNextHistoryAddr;
  pMemEnd = pFIRPriv -> pHistory + pFIRPriv -> n - 1;

  for(i = 0; i < n; i++)
  {
    /* initialize total and setup pointer to walk through coefficients */
    total32  = 0;
    pCoefs   = pFIRPriv -> pC;

    /* replace oldest with newest input value */
    *pMem++ = *pX++;

    if (pMem > pMemEnd)
    {
      pMem = pFIRPriv->pHistory;
    }

    for (j=0; j<pFIRPriv->n; j++)
    {
      total32 = L_mac (total32, *pCoefs++, *pMem++);

      if (pMem > pMemEnd)
      {
        pMem = pFIRPriv->pHistory;
      }
    }

    *pZ++ = round(total32);
  }

  pFIRPriv->pNextHistoryAddr = pMem;
}


/* FIR for a single input sample */
Frac16 dfr16FIRs (dfr16_tFirStruct *pFIR, Frac16 x)
{
  Frac16 z;

  dfr16FIR (pFIR, &x, &z, 1);

  return z;
}


dfr16_tFirDecStruct * dfr16FIRDecCreate  (Frac16 *pC, UInt16 n, UInt16 f)
{
  dfr16_tFirDecStruct * pFirDec;

  pFirDec = dfr16FIRCreate (pC, n);

  ((dfr16_tFirStructPriv *)pFirDec) -> Factor     = f;
  ((dfr16_tFirStructPriv *)pFirDec) -> Count      = f;

  return pFirDec;
}


void dfr16FIRDecInit  (dfr16_tFirDecStruct *pFIRDec, Frac16 *pC, UInt16 n, UInt16 f)
{
  dfr16FIRInit (pFIRDec, pC, n);

  ((dfr16_tFirStructPriv *)pFIRDec) -> Factor     = f;
  ((dfr16_tFirStructPriv *)pFIRDec) -> Count      = f;
}


UInt16  dfr16FIRDec (dfr16_tFirDecStruct *pFIRDEC, Frac16 *pX, Frac16 *pZ, UInt16 nx)
{
  UInt16                 i;
  Frac16               * pMemEnd;
  UInt16                 outcnt;
  dfr16_tFirStructPriv * pFIRDecPriv = (dfr16_tFirStructPriv *)pFIRDEC;

  pMemEnd = pFIRDecPriv -> pHistory + pFIRDecPriv -> n - 1;
  outcnt  = 0;

  for (i = 0; i < nx; i++)
  {
    pFIRDecPriv -> Count--;

    if (pFIRDecPriv -> Count <= 0)
    {
      pFIRDecPriv -> Count = pFIRDecPriv -> Factor;

      *pZ++ = dfr16FIRs (pFIRDEC, *pX++);

      outcnt++;
    }
    else
    {
      /* replace oldest with newest input value */
      *(pFIRDecPriv -> pNextHistoryAddr++) = *pX++;

      if (pFIRDecPriv -> pNextHistoryAddr > pMemEnd)
      {
          pFIRDecPriv -> pNextHistoryAddr = pFIRDecPriv -> pHistory;
      }
    }
  }

  return outcnt;
}


dfr16_tFirIntStruct * dfr16FIRIntCreate  (Frac16 *pC, UInt16 n, UInt16 f)
{
  dfr16_tFirStruct * pFirInt;
  UInt16             Count;
  UInt16             SizeCoefArray;

  pFirInt = (dfr16_tFirIntStruct *)memMallocEM(sizeof(dfr16_tFirIntStruct));

  if(pFirInt != NULL)
  {
    /*
      Make coef array an even multiple of interpolation factor
      so that we can optimize the interpolation algorithm loops
    */
    Count                      = (n + f - 1) / f;
    SizeCoefArray              = Count * f;

    pFirInt -> pC              = (Frac16 *)memMallocIM(SizeCoefArray * sizeof(Frac16));

    pFirInt -> pHistory        = (Frac16 *)memMallocAlignedEM(Count * sizeof(Frac16));

    if (pFirInt -> pC != NULL && pFirInt -> pHistory != NULL)
    {
      /* All memory allocations have succeeded, so initialize structure */

      dfr16FIRIntInit (pFirInt, pC, n, f);
    }
    else
    {
      if (pFirInt -> pC != NULL)
      {
        memFreeIM (pFirInt -> pC);
      }

      if (pFirInt -> pHistory != NULL)
      {
        memFreeEM (pFirInt -> pHistory);
      }

      memFreeEM (pFirInt);
      pFirInt = NULL;
    }
  }

  return pFirInt;
}


void dfr16FIRIntInit  (dfr16_tFirIntStruct *pFIRInt, Frac16 *pC, UInt16 n, UInt16 f)
{
  UInt16                 i, j;
  UInt16                 SubArrayLen;
  Frac16               * pNewC;
  dfr16_tFirStructPriv * pFIRIntPriv = (dfr16_tFirStructPriv *)pFIRInt;
  Frac16               * pCtemp;

  assert (pFIRInt != NULL);
  assert (pFIRInt -> pC != NULL);
  assert (pFIRInt -> pHistory != NULL);

  pFIRIntPriv -> Factor           = f;
  pFIRIntPriv -> Count            = (n + f - 1) / f;
  pFIRIntPriv -> n                = pFIRIntPriv -> Count * f;

  pFIRIntPriv -> bCanUseModAddr   = memIsAligned(pFIRIntPriv -> pHistory,
                          (pFIRIntPriv -> Count)*sizeof(Frac16));

  pFIRIntPriv -> bCanUseDualMAC   = pFIRIntPriv -> bCanUseModAddr && memIsIM(pFIRIntPriv -> pC);

  pFIRIntPriv -> pNextHistoryAddr = pFIRIntPriv -> pHistory;

  /* Clear History array */
  for (i=0; i < pFIRIntPriv -> Count; i++)
  {
    *(pFIRIntPriv -> pNextHistoryAddr++) = 0;
  }

  pNewC = pFIRIntPriv -> pC + pFIRIntPriv -> Count - 1;

  /* Create f coefficient sub-arrays from the user's original coefficient array */
  for (i=n; i>n-f; i--)
  {
    pCtemp = pC + i - 1;

    SubArrayLen = (i + f - 1) / f;

    for (j=0; j < SubArrayLen; j++)
    {
      /* Reverse order of coefficients so that dual parallel MAC can be used */
      *pNewC-- = *pCtemp;
      pCtemp  -= f;
    }

    if (SubArrayLen < pFIRIntPriv -> Count)
    {
      /* Zero pad is required */
      *pNewC-- = 0;
    }

    pNewC += 2 * pFIRIntPriv -> Count;

  }

  pFIRIntPriv -> pNextHistoryAddr = pFIRIntPriv -> pHistory;
}



UInt16  dfr16FIRIntC (dfr16_tFirIntStruct *pFIRINT, Frac16 *pX, Frac16 *pZ, UInt16 n)
{
  Int16                  i, j, k;
  Frac16               * pCoefs;
  Frac16               * pMem;
  Frac16               * pMemEnd;
  Frac16               * pMemTemp;
//  Frac32                 temp1;
  Frac32                 total32;
  dfr16_tFirStructPriv * pFirIntPriv = (dfr16_tFirStructPriv *)pFIRINT;

  for (i = 0; i < n; i++)
  {
    pMem    = pFirIntPriv -> pNextHistoryAddr;
    pMemEnd = pFirIntPriv -> pHistory + pFirIntPriv -> Count - 1;

    /* replace oldest with newest input value */
    *pMem++ = *pX++;

    if (pMem > pMemEnd)
    {
       pMem = pFirIntPriv->pHistory;
    }

    pCoefs   = pFirIntPriv -> pC;

    for (j=0; j<pFirIntPriv->Factor; j++)
    {
      /* initialize total and setup pointer to walk through coefficients */
      total32  = 0;
      pMemTemp = pMem;

      for (k=0; k<pFirIntPriv->Count; k++)
      {
        total32 = L_mac (total32, *pCoefs++, *pMemTemp++);

        if (pMemTemp > pMemEnd)
        {
          pMemTemp = pFirIntPriv->pHistory;
        }
      }
      *pZ++ = round(total32);
    }

    pFirIntPriv->pNextHistoryAddr = pMem;
  }

  return n * pFirIntPriv -> Factor;
}


/*******************************************************
* 16-bit Fractional IIR Filters
*******************************************************/
/*--------------------------------------------------------------
* Revision History:
*
* VERSION    CREATED BY    MODIFIED BY      DATE       COMMENTS
* -------    ----------    -----------      -----      --------
*   1.0      Meera S. P.        -         03-02-2000   Reviewed.
*   1.1       -      Abdul Farah    07-12-2001   Add IIRinit
*
*-------------------------------------------------------------*/
/********************************************************************************
* File Name : dfr16.c                                                           *
*                                                                               *
* Function : dfr16_tIirStruct * dfr16IIRCreate     (Frac16 *pC, UInt16 nbiq)    *
*                                                                               *
* Description:                                                                  *
* This routine initializes the data structure pointed to by pIIR, which is used *
* used to preserve the filter states between the calls. The data structure      *
* stores no. of biquads, pointer to filter coefficients and pointer to the past *
* history of 2 data elements (w) per biquad.                                    *
*                                                                               *
* Inputs :                                                                      *
*                                                                               *
*         1) Pointer to the Filter coefficient buffer                           *
*         2) The no. of biquads                                                 *
*                                                                               *
* Outputs :                                                                     *
*                                                                               *
*         1) Pointer to the data structiure pIIR                                *
********************************************************************************/

dfr16_tIirStruct * dfr16IIRCreate     (Frac16 *pC, UInt16 nbiq)
{
  dfr16_tIirStruct * pIir;
//  UInt16    i;
//  Frac16    * pNewC;

  pIir = (dfr16_tIirStruct *)memMallocEM(sizeof(dfr16_tIirStruct));

  if(pIir != NULL)
  {
    pIir -> nbiq = nbiq;
    pIir -> pHistory = (Frac16 *)memMallocAlignedEM(nbiq * FILT_STATES_PER_BIQ * sizeof(Frac16));
      pIir -> pC = (Frac16 *)memMallocIM(nbiq * FILT_COEF_PER_BIQ * sizeof(Frac16));

    if (pIir -> pC != NULL && pIir -> pHistory != NULL)
    {
      /* All memory allocations have succeeded, so initialize structure */
       dfr16IIRInit (pIir, pC, nbiq);
    }
    else
    {
      if (pIir -> pC != NULL)
      {
        memFreeIM (pIir -> pC);
      }

      if (pIir -> pHistory != NULL)
      {
        memFreeEM (pIir -> pHistory);
      }

      memFreeEM (pIir);
      pIir = NULL;
    }
  }

  return pIir;
}
/********************************************************************************
* File Name : dfr16.c                                                           *
*                                                                               *
* Function :void dfr16IIRInit (dfr16_tIirStruct * pIir, Frac16 *pC, UInt16 nbiq)*
*                                                                               *
* Description:                                                                  *
* This routine initializes the data structure pointed to by pIir, which is used *
* used to preserve the filter states between the calls. The data structure      *
* stores no. of biquads, pointer to filter coefficients and pointer to the past *
* history of 2 data elements (w) per biquad.                                    *
*                                                                               *
* Inputs :                                                                      *
*                                                                               *
*         1) pIir -  Pointer to the data structre pIIR (Statically allocated by  *
*           the caller before dfr16IIRInit is called)          *
*         2) Pointer to the Filter coefficient buffer                           *
*         3) The no. of biquads                                               *
* Outputs :                                                                     *
*                                                                               *
*         1) Pointer to the data structiure pIIR                                *
********************************************************************************/
void dfr16IIRInit (dfr16_tIirStruct * pIir, Frac16 *pC, UInt16 nbiq)
{
  UInt16                 i;
  Frac16               * pNewC;

  assert (pIir != NULL);
  assert (pIir -> pC != NULL);
  assert (pIir -> pHistory != NULL);

      pNewC = pIir -> pC;

      pIir ->nbiq = nbiq;

      pIir -> bCanUseModAddr = memIsAligned(pIir -> pHistory,
                          (nbiq * FILT_STATES_PER_BIQ)*sizeof(Frac16));

      pIir -> bCanUseDualMAC = pIir -> bCanUseModAddr && memIsIM(pIir -> pC);

      pIir -> pNextHistoryAddr = pIir -> pHistory;

      for (i=0; i < pIir -> nbiq * FILT_STATES_PER_BIQ; i++)
      {
        /* Initialize history buffer */
        *(pIir -> pNextHistoryAddr++) = 0;
      }

      for (i=0; i < pIir -> nbiq * FILT_COEF_PER_BIQ; i++)
      {
        *(pNewC++) = *pC++;
      }

      pIir -> pNextHistoryAddr = pIir -> pHistory;

}

/********************************************************************************
* File Name : dfr16.c                                                           *
*                                                                               *
* Function : void    dfr16IIRDestroy (dfr16_tIirStruct *pIIR)                   *
*                                                                               *
* Description:                                                                  *
* This routine deallocates the data structure pIIr.                             *
* Inputs :                                                                      *
*                                                                               *
*         1) Pointer to the data structre pIIR                                  *
*                                                                               *
* Outputs :                                                                     *
*                                                                               *
*         None                                                                  *
********************************************************************************/

void    dfr16IIRDestroy (dfr16_tIirStruct *pIIR)
{
  if (pIIR != NULL) {

    if (pIIR -> pC != NULL)
    {
      memFreeIM (pIIR -> pC);
    }

    if (pIIR -> pHistory != NULL)
    {
      memFreeEM (pIIR -> pHistory);
    }

    memFreeEM (pIIR);
  }
}

/********************************************************************************
* File Name : dfr16.c                                                           *
*                                                                               *
* Function : Result  dfr16IIR (dfr16_tIirStruct *pIIR, Frac16 *pX, Frac16 *pZ,  *
*                              UInt16 n)                                        *
*                                                                               *
* Description:                                                                  *
* This routine computes a Infinite Impulse Response (IIR) filter for a vector of*
* fractional data values using a cascade filter of biquad coefficients.         *
*                                                                               *
* Inputs :                                                                      *
*                                                                               *
*         1) pIIR - Pointer to the data structre pIIR                           *
*         2) pX   - Pointer to the input vector                                 *
*         3) pZ   - Pointer to output vector                                    *
*         4) n    - Length of the input and output vector                       *
*                                                                               *
* Outputs :                                                                     *
*                                                                               *
*         FAIL(-1) - if length of input and output vecotor is greater than 8191 *
*         PASS(0)  - if length of input and output vecotor is not greater than  *
*                    8191                                                       *
********************************************************************************/

Result  dfr16IIRC (dfr16_tIirStruct *pIIR, Frac16 *pX, Frac16 *pZ, UInt16 n)
{
  UInt16   i, j;
  Frac16 * pCoefs;
  Frac16   w0,w1,w2;
  Frac16    xn;
  Frac32   Acc,long_xn;

  if (n > PORT_MAX_VECTOR_LEN)
  {
    return FAIL;
  }



   for(i = 0; i < n; i++)  /* Sample loop */
  {
    /* initialize total and setup pointer to walk through coefficients */
    pCoefs = pIIR -> pC;
      Acc = 0;

    xn = *pX++;

    pIIR -> pNextHistoryAddr = pIIR ->pHistory;

    for (j = 0; j < pIIR->nbiq; j++)    /* States Loop */
    {

         w1 = *(pIIR -> pNextHistoryAddr++);
         w2 = *(pIIR -> pNextHistoryAddr--);

         Acc = L_mult(*pCoefs++,w1);   /* a1*w1 */
         Acc = L_mac(Acc,*pCoefs++,w2); /* a2*w2 */

         long_xn = L_deposit_h(xn);
         Acc = L_add(long_xn,Acc);

         w0 = round(Acc);

         Acc = L_mult(*pCoefs++,w0);   /* b0 *w0 */
         Acc = L_mac(Acc,*pCoefs++,w1); /*b1 * w1*/
         Acc = L_mac(Acc,*pCoefs++,w2); /*b2 * w2 */

         *(pIIR -> pNextHistoryAddr++) = w0;
         *(pIIR -> pNextHistoryAddr++) = w1;

         xn = round(Acc);

    }

    *pZ++ = xn;
  }

  pIIR->pNextHistoryAddr = pIIR -> pHistory;

  return PASS;
}






/*******************************************************
* 16-bit Fractional Correlations
*******************************************************/

/*--------------------------------------------------------------
* Revision History:
*
* VERSION    CREATED BY    MODIFIED BY      DATE       COMMENTS
* -------    ----------    -----------      -----      --------
*   0.1      Meera S. P.        -          07-01-2000   Reviewed.
*
*-------------------------------------------------------------*/


/********************************************************************************
* File Name : dfr16.c                                                           *
*                                                                               *
* Function : dfr16AutoCorrC(UInt16 options, Frac16 *pX, Frac16 *pZ, UInt16 nx,  *
*                           UInt16 nz)                                          *
*                                                                               *
* Description:                                                                  *
* This routine computes the first nz points of auto-correlation of a vector of  *
* fractional data values.                                                       *
*                                                                               *
* Inputs :                                                                      *
*                                                                               *
*        1) options - Selects between raw, biased, and unbiased auto correlation*
*        2) pX      - Pointer to the input vector                               *
*        3) pZ      - Pointer to output vector                                  *
*        4) nX      - Length of the input vector                                *
*        5) nZ      - Length of the output vector                               *
*                                                                               *
* Outputs :                                                                     *
*                                                                               *
*         FAIL(-1) - if length of output vector is greater than 8191            *
*         PASS(0)  - if length of output vector is not greater than 8191        *
********************************************************************************/

Result dfr16AutoCorrC(UInt16 options, Frac16 *pX, Frac16 *pZ, UInt16 nx, UInt16 nz)
  {
    Frac16 bias;
    Frac32 sum = 0;
    int    j, k, count = 0;
//    int    i;

       if(nz > PORT_MAX_VECTOR_LEN || options >= 3 || (2*nx - 1) > PORT_MAX_VECTOR_LEN)
        return FAIL;


      for(j = (int) (nx-1); j >= 0; j--)
      {
         switch(options)
         {
             case CORR_RAW:
                 bias = 0x7fff;
                 break;
             case CORR_BIAS:
                 bias = div_s(1,(Word16) nx);
                 break;
             case CORR_UNBIAS:
                 bias = sub((Word16) nz,j);
                 bias = div_s(1,bias);
                 break;
         }

         for(k = 0;k <= (nx-(j+1));k++)
         {
             sum = L_mac(sum,*(pX+j+k),*(pX+k));
         }

         *(pZ+count) = mult_r(round(sum),bias);
         sum = 0;
         count++;

      }

      for(j = 1; j < nx; j++)
      {
         switch(options)
        {
           case CORR_RAW:
               bias = 0x7fff;
               break;
           case CORR_BIAS:
               bias = div_s(1,(Word16) nx);
               break;
           case CORR_UNBIAS:
               bias = sub((Word16) nz,j);
               bias = div_s(1,bias);
               break;
         }

         for(k = 0;k <= (nx-(j+1));k++)
         {
           sum = L_mac(sum,*(pX+j+k),*(pX+k));
         }

         *(pZ+count) = mult_r(round(sum),bias);
         sum = 0;
         count++;

       }

      return PASS;
   }

/*--------------------------------------------------------------
* Revision History:
*
* VERSION    CREATED BY    MODIFIED BY      DATE       COMMENTS
* -------    ----------    -----------      -----      --------
*   0.1      Meera S. P.        -          07-01-2000   Reviewed.
*
*-------------------------------------------------------------*/

/********************************************************************************
* File Name : dfr16.c                                                           *
*                                                                               *
* Function : dfr16CorrC(UInt16 options,Frac16 *px, Frac16 *py, Frac16 *pz,      *
*                       UInt16 nx, UInt16 ny)                                   *
*                                                                               *
* Description:                                                                  *
* This routine computes the full length correlation of two vectors offractional *
* data values.                                                                  *
*                                                                               *
* Inputs :                                                                      *
*                                                                               *
*        1) options - Selects between raw, biased, and unbiased auto correlation*
*        2) pX      - Pointer to the input vector                               *
*        3) pX      - Pointer to the first input vector                         *
*        4) pZ      - Pointer to the second input vector                        *
*        5) nX      - Length of the input vector pointed to by pX               *
*        6) nY      - Length of the input vector pointed to by pY               *
*                                                                               *
* Outputs :                                                                     *
*                                                                               *
*         FAIL(-1) - if length of output vector is greater than 8191            *
*         PASS(0)  - if length of output vector is not greater than 8191        *
********************************************************************************/


Result dfr16CorrC(UInt16 options,Frac16 *px, Frac16 *py, Frac16 *pz, UInt16 nx, UInt16 ny)
{
//  int i;
  int j,k,itr,lpcnt;
  Frac16 bias,count = 0;
  Frac32 sum = 0;
//  Frac32 temp1;

    if((nx+ny-1) > PORT_MAX_VECTOR_LEN)     /*check whether length of output vector exceed 8191 */
    {
        return FAIL;
    }

    if(nx <= ny)
    {
      itr = (int) nx;
    }
    else
    {
        itr = (int) ny;
    }

    for(j = (int) (nx-1); j > 0; j--)
    {
        if((nx-(j+1)) > (itr-1))
      {
             lpcnt = itr-1;
         }
         else
         {
            lpcnt = (int) (nx-(j+1));
        }

        switch(options)
        {
            case CORR_RAW:
               bias = 0x7fff;
               break;
            case CORR_BIAS:
                bias = add((Word16) nx, (Word16) ny);
                bias = sub(bias,1);
                bias = div_s(1,bias);
                break;
            case CORR_UNBIAS:
                bias = add((Word16) nx, (Word16) ny);
                bias = sub(bias,1);
                bias = sub(bias,j);
                bias = div_s(1,bias);
                break;
        }


        for(k = 0; k <= lpcnt; k++)
        {
            sum = L_mac(sum,*(px+j+k),*(py+k));
      }

        *(pz+count) = mult_r(round(sum),bias);
        sum = 0;
        count++;
    }

    for(j = 0; j < ny; j++)
    {
      if((ny-(j+1)) > (itr-1))
      {
            lpcnt = itr-1;
        }
        else
        {
            lpcnt = (int) (ny-(j+1));
        }

        switch(options)
        {
            case CORR_RAW:
                bias = 0x7fff;
                break;
            case CORR_BIAS:
                bias = add((Word16) nx, (Word16) ny);
                bias = sub(bias,1);
                bias = div_s(1,bias);break;
            case CORR_UNBIAS:
                bias = add((Word16) nx, (Word16) ny);
                bias = sub(bias,1);
                bias = sub(bias,j);
                bias = div_s(1,bias);
                break;
        }

        for(k = 0; k <= lpcnt; k++)
        {
            sum = L_mac(sum,*(py+j+k),*(px+k)) ;
        }

        *(pz+count) = mult_r(round(sum),bias);
        sum = 0;
        count++;
    }

    return PASS;
}
