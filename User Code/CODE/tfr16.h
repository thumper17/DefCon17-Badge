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

/* File: tfr16.h */

#ifndef __TFR16_H
#define __TFR16_H

#include "port.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************
* To switch between C and assembly implementations
*       #if 0  => assembly
*       #if 1  => C
*******************************************************/

/*******************************************************
* Trigonometric Functions for 16-bit Fractional
*******************************************************/

EXPORT Frac16 tfr16SinPIx      (Frac16 x);

EXPORT Frac16 tfr16CosPIx      (Frac16 x);

EXPORT Frac16 tfr16AsinOverPI  (Frac16 x);

EXPORT Frac16 tfr16AcosOverPI  (Frac16 x);

EXPORT Frac16 tfr16AtanOverPI  (Frac16 x);

EXPORT Frac16 tfr16Atan2OverPI (Frac16 y, Frac16 x);

/*******************************************************
* Sine Wave Generation Functions for 16-bit Fractional
*******************************************************/

/* Table lookup method via integer delta */
typedef struct tfr16_sSineWaveGenIDTL
{
 UInt16 * pIndex;
 Frac16 * pEndTable;
 ibool    bAligned;
 UInt16   Delta;
 UInt16   SineTableLength;
// Word16 PrivateData[5]; /* Private data for the IDTL sine generation function */
}tfr16_tSineWaveGenIDTL;

EXPORT tfr16_tSineWaveGenIDTL * tfr16SineWaveGenIDTLCreate(Frac16 * pSineTable,
                     UInt16   SineTableLength,
                     Int16    SineFreq,
                     Int16    SampleFreq,
                     Frac16   InitialPhasePIx);

EXPORT void tfr16SineWaveGenIDTLDestroy(tfr16_tSineWaveGenIDTL * pSWG);


EXPORT void tfr16SineWaveGenIDTLInit(tfr16_tSineWaveGenIDTL * pSWG,
             Frac16                 * pSineTable,
             UInt16                   SineTableLength,
             Int16                    SineFreq,
                                         Int16                    SampleFreq,
             Frac16                   InitialPhasePIx);

EXPORT void tfr16SineWaveGenIDTL(tfr16_tSineWaveGenIDTL * pSWG, Frac16 * pValues, UInt16 Nsamples);

/* Table lookup method via real delta */
typedef struct tfr16_sSineWaveGenRDTL
{
 Frac16 * pSineTable;
 Frac16   Phase;
 Frac16   Delta;
 UInt16   SineTableLength;
// Word16 PrivateData[4]; /* Private data for the RDTL sine generation function */
}tfr16_tSineWaveGenRDTL;

EXPORT tfr16_tSineWaveGenRDTL * tfr16SineWaveGenRDTLCreate(Frac16 * pSineTable,
                     UInt16   SineTableLength,
                     Int16    SineFreq,
                     Int16    SampleFreq,
                     Frac16   InitialPhasePIx);

EXPORT void tfr16SineWaveGenRDTLDestroy(tfr16_tSineWaveGenRDTL * pSWG);


EXPORT void tfr16SineWaveGenRDTLInit(tfr16_tSineWaveGenRDTL * pSWG,
             Frac16                 * pSineTable,
             UInt16                   SineTableLength,
             Int16                    SineFreq,
                                                 Int16                    SampleFreq,
             Frac16                   InitialPhasePIx);

EXPORT void tfr16SineWaveGenRDTL(tfr16_tSineWaveGenRDTL * pSWG, Frac16 * pValues, UInt16 Nsamples);


/* Table lookup method via real delta with interpolation */
typedef struct tfr16_sSineWaveGenRDITL
{
 Frac16 * pSineTable;
 Frac16   Phase;
 Frac16   Delta;
 UInt16   SineTableLength;
 UInt16   Shift;
// Word16 PrivateData[5]; /* Private data for the RDITL sine generation function */
}tfr16_tSineWaveGenRDITL;

EXPORT tfr16_tSineWaveGenRDITL * tfr16SineWaveGenRDITLCreate(Frac16 * pSineTable,
                     UInt16   SineTableLength,
                     Int16    SineFreq,
                     Int16    SampleFreq,
                     Frac16   InitialPhasePIx);

EXPORT void tfr16SineWaveGenRDITLDestroy(tfr16_tSineWaveGenRDITL * pSWG);


EXPORT void tfr16SineWaveGenRDITLInit(tfr16_tSineWaveGenRDITL * pSWG,
              Frac16                  * pSineTable,
              UInt16                    SineTableLength,
              Int16                     SineFreq,
              Int16                     SampleFreq,
              Frac16                    InitialPhasePIx);

EXPORT void tfr16SineWaveGenRDITL(tfr16_tSineWaveGenRDITL * pSWG, Frac16 * pValues, UInt16 Nsamples);


/* Table lookup method via real delta with interpolation, quarter of a sine LUT */
typedef struct tfr16_sSineWaveGenRDITLQ
{
 Frac16 * pSineTable;
 Frac16   Phase;
 Frac16   Delta;
 UInt16   SineTableLength;
 UInt16   Shift;
// Word16 PrivateData[5]; /* Private data for the RDITLQ sine generation function */
}tfr16_tSineWaveGenRDITLQ;

EXPORT tfr16_tSineWaveGenRDITLQ * tfr16SineWaveGenRDITLQCreate(Frac16 * pSineTable,
                     UInt16   SineTableLength,
                     Int16    SineFreq,
                     Int16    SampleFreq,
                     Frac16   InitialPhasePIx);

EXPORT void tfr16SineWaveGenRDITLQDestroy(tfr16_tSineWaveGenRDITLQ * pSWG);


EXPORT void tfr16SineWaveGenRDITLQInit(tfr16_tSineWaveGenRDITLQ * pSWG,
              Frac16                    * pSineTable,
              UInt16                      SineTableLength,
              Int16                       SineFreq,
              Int16                       SampleFreq,
              Frac16                      InitialPhasePIx);

EXPORT void tfr16SineWaveGenRDITLQ(tfr16_tSineWaveGenRDITLQ * pSWG, Frac16 * pValues, UInt16 Nsamples);


/* Digital oscillator method */
typedef struct tfr16_sSineWaveGenDOM
{
 Frac16 FilterState1;
 Frac16 FilterState2;
 Frac16 FilterCoefs;
// Word16 PrivateData[3]; /* Private data for the Digital Oscillator method */
}tfr16_tSineWaveGenDOM;

EXPORT tfr16_tSineWaveGenDOM * tfr16SineWaveGenDOMCreate(Int16   SineFreq,
                  Int16   SampleFreq,
                  Frac16  InitialPhasePIx,
                  Frac16  Amplitude);

EXPORT void tfr16SineWaveGenDOMDestroy(tfr16_tSineWaveGenDOM * pSWG);


EXPORT void tfr16SineWaveGenDOMInit(tfr16_tSineWaveGenDOM * pSWG,
            Int16                  SineFreq,
            Int16                  SampleFreq,
            Frac16                 InitialPhasePIx,
            Frac16                 Amplitude);

EXPORT void tfr16SineWaveGenDOM(tfr16_tSineWaveGenDOM * pSWG, Frac16 * pValues, UInt16 Nsamples);

/* Polynomial approximation method */
typedef struct tfr16_sSineWaveGenPAM
{
 Frac16 PreviousAlpha;
 Frac16 Delta;
 Frac16 NextAlpha;
 Frac16 Amplitude;
// Word16 PrivateData[4]; /* Private data for the polynomial approximation method */
}tfr16_tSineWaveGenPAM;

EXPORT tfr16_tSineWaveGenPAM * tfr16SineWaveGenPAMCreate(Int16   SineFreq,
                  Int16   SampleFreq,
                  Frac16  InitialPhasePIx,
                  Frac16  Amplitude);

EXPORT void tfr16SineWaveGenPAMDestroy(tfr16_tSineWaveGenPAM * pSWG);


EXPORT void tfr16SineWaveGenPAMInit(tfr16_tSineWaveGenPAM * pSWG,
            Int16       SineFreq,
            Int16       SampleFreq,
            Frac16      InitialPhasePIx,
            Frac16      Amplitude);

EXPORT void tfr16SineWaveGenPAM(tfr16_tSineWaveGenPAM * pSWG, Frac16 * pValues, UInt16 Nsamples);


/* Table lookup method via real delta with interpolation, quarter of a sine LUT */
typedef struct tfr16_sWaveGenRDITLQ
{
 Frac16 * pSineTable;
 Frac16   Phase;
 UInt16   SineTableLength;
 UInt16   Shift;
// Word16 PrivateData[4]; /* Private data for the RDITLQ wave generation function */
}tfr16_tWaveGenRDITLQ;

EXPORT tfr16_tWaveGenRDITLQ * tfr16WaveGenRDITLQCreate(Frac16 * pSineTable,
                UInt16     SineTableLength,
              Frac16     InitialPhasePIx);

EXPORT void tfr16WaveGenRDITLQDestroy(tfr16_tWaveGenRDITLQ * pSWG);


EXPORT void tfr16WaveGenRDITLQInit(tfr16_tWaveGenRDITLQ * pSWG,
            Frac16  * pSineTable,
            UInt16    SineTableLength,
            Frac16    InitialPhasePIx);

EXPORT Frac16 tfr16WaveGenRDITLQ(tfr16_tWaveGenRDITLQ * pSWG, Frac16 PhaseIncrement);


/* Table lookup method via real delta with interpolation, quarter of a sine LUT */
typedef struct tfr16_sSinPIxLUT
{
 Frac16 * pSineTable;
 UInt16   SineTableLength;
 UInt16   Shift;
// Word16 PrivateData[3]; /* Private data for the SinPIxLUT function */
}tfr16_tSinPIxLUT;

EXPORT tfr16_tSinPIxLUT * tfr16SinPIxLUTCreate(Frac16 * pSineTable,
              UInt16   SineTableLength);

EXPORT void tfr16SinPIxLUTDestroy(tfr16_tSinPIxLUT * pSWG);


EXPORT void tfr16SinPIxLUTInit(tfr16_tSinPIxLUT * pSWG,
          Frac16  * pSineTable,
          UInt16    SineTableLength);

EXPORT Frac16 tfr16SinPIxLUT(tfr16_tSinPIxLUT * pSWG, Frac16 PhasePIx);


/* Table lookup method via real delta with interpolation, quarter of a sine LUT */
typedef struct tfr16_sCosPIxLUT
{
 Frac16 * pSineTable;
 UInt16   SineTableLength;
 UInt16   Shift;
// Word16 PrivateData[4]; /* Private data for the CosPIxLUT function */
}tfr16_tCosPIxLUT;

EXPORT tfr16_tCosPIxLUT * tfr16CosPIxLUTCreate(Frac16 * pSineTable,
               UInt16   SineTableLength);

EXPORT void tfr16CosPIxLUTDestroy(tfr16_tCosPIxLUT * pSWG);


EXPORT void tfr16CosPIxLUTInit(tfr16_tCosPIxLUT * pSWG,
          Frac16  * pSineTable,
          UInt16    SineTableLength);

EXPORT Frac16 tfr16CosPIxLUT(tfr16_tCosPIxLUT * pSWG, Frac16 PhasePIx);

#ifdef __cplusplus
}
#endif

#endif
