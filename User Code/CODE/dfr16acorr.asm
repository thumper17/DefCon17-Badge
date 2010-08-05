;*********************************************************************
;
; (c) Freescale Semiconductor
; 2004 All Rights Reserved
;
;
;*********************************************************************
;
; File Name:    dfr16acorr.asm
;
; Description:  Assembly module for Auto Correlation
;
; Modules
;    Included:  Fdfr16AutoCorr
;
; Author(s):    Alwin Anbu.D
;
; Date:         23 Nov 2001
;
;**********************************************************************

    include "portasm.h"

    SECTION rtlib

    GLOBAL  Fdfr16AutoCorr

  org     p:

;**********************************************************************
;
; Module Name:  Fdfr16AutoCorr
;
; Description:  This routine computes the first nz points of auto-correlation of a vector of
;               fractional data values.
;
; Input(s):     1. options - Selects between raw, biased, and unbiased auto correlation
;               2. pX      - Pointer to the input vector
;               3. pZ      - Pointer to output vector
;               4. nX      - Length of the input vector
;               5. nZ      - Length of the output vector
;
; Output(s):
;               FAIL(-1) - if an invalid parameter is passed
;               PASS(0)  - if no invalid parameters are passed
;
; Functions
;     Called:   None
;
;
; Calling
; Requirements: 1. r2 - Pointer to Input vector pX
;               2. r3 - Pointer to Output vector pZ
;               3. y0 - Options
;               4. y1 - Length Of Input Vector
;               5. a  - Length Of Output Vector
;
;
; C Callable:   Yes
;
; Reentrant:    Yes
;
; Globals:      None
;
; Statics:      None
;
; Registers
;      Changed: All
;
; DO loops:     2 (nested) plus 1
;
; REP loops:    2
;
; Environment:  MetroWerks on PC
;
; Reference:    None
;
; Cycle Count:  Peak cycle count:
;               Average cycle count:
;
; Special
;     Issues:   1. Input Vector Size should NOT exceed 32767
;               2. DO Loop Nesting Level is 2.Hence the calling fuction
;                  should take care if needs to be called inside a DO
;                  Loop
;
;******************************Change History************************
;
;    DD/MM/YY     Code Ver      Description     Author(s)
;    --------     --------     ------------     ---------
;   23-11-2001      0.1        Module created   Alwin Anbu.D
;   27-11-2001      1.0        Baselined        Alwin Anbu.D
;
;********************************************************************

    DEFINE    old_omr 'x:(SP)'

Fdfr16AutoCorr:

    adda     #2,sp
    move.l   c2,x:(sp)+
    move.l   c10,x:(sp)+
    move.l   d2,x:(sp)+
    move.l   d10,x:(sp)+

    move.w    omr,old_omr

    move.w   y1,d1                        ; d1=nX

    IF       ASSERT_ON_INVALID_PARAMETER==1
    asl.w    y1                           ; y1=2*nX
    dec.w    y1                           ; 2*nX-1
    bfset    #$100,omr                    ; Enable 32 bit mode
    cmp.w    #PORT_MAX_VECTOR_LEN,y1      ; Check if 2*nX-1 > 65535
    bhi      AcorrFail                    ; If yes,Jump to fail
    cmp.w    #2,y0                        ; Check if option given is valid
    bgt      AcorrFail                    ; If not,Jump  to fail
    cmp.w    #PORT_MAX_VECTOR_LEN,a       ; Compare if nZ<= 65535
    bls      Length_OK

AcorrFail:
    move.w   #-1,y0                       ; If Fail,return -1(FAIL)
    bra      >EndAcorr
    ENDIF

Length_OK:

    bfclr    #$100,omr                    ; Disable 32 bit mode
    tfra     r3,r4                        ; r4 now points to output

Comp_Raw:
    tst.w    y0                           ; Check if options equals 0
    bne      Comp_Bias                    ; If not,Jump to Comp_Bias
    move.w   #$7fff,b0                    ; If yes,set Bias as $7fff
    bra      Loop1                        ; Branch to Loop1
Comp_Bias:
    cmp.w    #1,y0                        ; Check if options equals 1
    bne      Loop1                        ; If not,Branch to Loop1
    move.w   #1,b                         ; --
    bfclr    #$0001,sr                    ;   |
    rep      #16                          ;   |--If yes,set Bias=1/nX
    div      d1,b                         ; --

Loop1:
    bfclr    #$10,omr                     ; Clear Rounding Bit in OMR for Convergent Rounding
    moveu.w  d1,n                         ; n=nX
    move.w   b0,a0                        ; Store Bias in a0
    deca     n                            ; n=nx-1

  if CODEWARRIOR_WORKAROUND==1
    do       d1,>>Loop1_End
    else
    do       d1,Loop1_End
    endif

Comp_Unbias:
    cmp.w    #2,y0                        ; Check if options is 2
    bne      Cont_loop1                   ; If not,Jump to Cont_Loop1
    move.w   a,c                          ; If yes,get nZ in c1,clearing c0
    move.w   n,b                          ; b=j,where j is the loop counter
    sub      b,c                          ; c=nZ-j
    move.w   #1,b
    bfclr    #$0001,sr                    ; --
    rep      #16                          ;   |
    div      c1,b                         ;   |-- b0=1/(nz-j)
    move.w   b0,a0                        ; --

Cont_loop1:
    tfra     r2,r0                        ; r0 points to the input vector
    tfra     r2,r3                        ; r3 points to the input vector
    move.w   n,x0                         ; x0=j
    adda     n,r0                         ; r0=r0+j
    move.w   d1,c                         ; c=nX
    sub      x0,c                         ; c=nx-j
    clr      b            x:(r0)+,y1      ; Clear Accumulator b for storing output y1=nX[j+k]

  if CODEWARRIOR_WORKAROUND==1
    do       c1,>>Loop2_End
    else
    do       c1,Loop2_End
    endif

    move.w   x:(r3)+,x0                   ; x0=nX[j]
    macr     y1,x0,b      x:(r0)+,y1      ; The Accumulator b contains the
                                          ; result

Loop2_End:
    move.w   a0,x0                        ; x0=Bias
    move.w   b,b1                         ; Limit the output
    mpyr     b1,x0,b                      ; Multiply with Bias
    deca     n                            ; Decrement Loop Counter
    move.w   b1,x:(r4)+                   ; Store the result at the output location

Loop1_End:
    tfra     r4,r3                        ; Move r4 to r3
    deca.l   r4                           ; r4=r4-2
    dec.w    d

  if CODEWARRIOR_WORKAROUND==1
    do       d1,>>Copy_Loop_End
    else
    do       d1,Copy_Loop_End
    endif

    move.w   x:(r4)-,x0                   ; Autocorrelation is a symmetic function
    move.w   x0,x:(r3)+                   ; Therefore,copy the computed values in the remaining locations

Copy_Loop_End:
    move.w   #0,y0                        ; Move 0(PASS) to y0
    adda     #1,r4
    tfra     r4,r2                        ; Move the output vector pointer to r2

EndAcorr:
    moveu.w  old_omr,omr                   ; Restore the OMR from

    suba     #2,sp                         ; Restore the stack Pointer
    move.l   x:(sp)-,d
    move.l   x:(sp)-,d2
    move.l   x:(sp)-,c
    move.l   x:(sp)-,c2

    rts

    ENDSEC

;************************* End of file ******************************
