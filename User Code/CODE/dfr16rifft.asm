;********************************************************************
;
; (c) Freescale Semiconductor
; (c) Copyright 2000, 2001 Freescale, Inc.
;
;
;********************************************************************
;
; File Name:    dfr16rifft.asm
;
; Description:  Assembly module for Real IFFT
;
; Modules
;    Included:  Fdfr16RIFFT
;
; Author(s):    Sandeep S (Optimized by Meera S P)
;
; Date:         01 Mar 2000
;
;********************************************************************

    SECTION   rtlib

    include   "portasm.h"

    GLOBAL    Fdfr16RIFFT

    org       p:

;********************************************************************
;
; Module Name:  Fdfr16RIFFT
;
; Description:  Computes real IFFT for the given input block.
;
; Functions
;      Called:  Fdfr16Cbitrev - Complex bit reverse function.
;
; Calling
; Requirements: 1. r2 -> Pointer to RIFFT Handle.
;               2. r3 -> Pointer to input buffer whose RIFFT is to be
;                        computed.
;               3. r4 -> Pointer to output buffer.
;
; C Callable:   Yes.
;
; Reentrant:    Yes.
;
; Globals:      None.
;
; Statics:      None.
;
; Registers
;      Changed: All.
;
; DO loops:     19; max. nesting depth = 2.
;
; REP loops:    None.
;
; Environment:  MetroWerks on PC.
;
; Special
;     Issues:   IFFT with No Scale option gives correct results only for
;               low energy signals.
;
;******************************Change History************************
;
;    DD/MM/YY     Code Ver     Description      Author(s)
;    --------     --------     -----------      ------
;    01/03/2000   0.1          Module created   Sandeep S
;    09/03/2000   1.0          Reviewed &
;                              Baselined        Sandeep S
;    12/03/2000   1.1          Optimized the
;                              Pre-Processing
;                              part             Sandeep S
;    29/01/2001   1.2          Optimized,
;                              Reviewed &
;                              Baselined        Meera S P
;    27/04/2001   2.0          Modified for V2  Jeff Ruhge
;    12/12/2001   2.1          Optimised for    Alwin Anbu D
;                              V2
;********************************************************************

Fdfr16RIFFT

Offset_options        equ    0
Offset_n              equ    Offset_options+1
Offset_Twiddle        equ    Offset_n+1
Offset_Twiddle_br     equ    Offset_Twiddle+PTR_SIZE
Offset_No_of_Stages   equ    Offset_Twiddle_br+PTR_SIZE

Scratch_Size          equ    4

START_BFLY_SZ         equ    2              ; To start with, butterfly size = 2.
THRESHOLD_BFP         equ    0.25           ; Minimum absolute value used in
                                            ;  BFP for comparison.

FFT_DEFAULT_OPTIONS               equ   0   ; Default all options bits
                                            ;  to 0
FFT_SCALE_RESULTS_BY_N            equ   1   ; Unconditionally scale by N

FFT_SCALE_RESULTS_BY_DATA_SIZE    equ   2   ; Scale according to data
                                            ;  sizes
FFT_INPUT_IS_BITREVERSED          equ   4   ; Default to normal (linear)
                                            ;  input
FFT_OUTPUT_IS_BITREVERSED         equ   8   ; Default to normal (linear)

    DEFINE    old_omr        'X:(SP-1)'     ; Scratch to store old omr reg.
    DEFINE    ScaleBF        'X:(SP-2)'     ; Counter to count the number of
                                            ;  stages wherein scaling is done
                                            ;  in BFP method.
    DEFINE    tmp            'X:(SP-3)'     ; Temporary variable

;---------------------------------
; Save c and d
;---------------------------------

    adda      #2,sp
    move.l    c2,x:(sp)+
    move.l    c10,x:(sp)+
    move.l    d2,x:(sp)+
    move.l    d10,x:(sp)+
    move.l    r5,x:(sp)

;---------------------------------
; Accommodate for scratch in stack
;---------------------------------

    adda      #Scratch_Size,sp              ; Update stack pointer.

;----------------------------------------------------------
; Store the existing omr and set the saturation bit to 0
;----------------------------------------------------------

    move.w    omr,old_omr                   ; Store old omr.
    tfra      r2,r5                         ; r5 -> pRIFFT
    bfclr     #$30,omr                      ; Sat mode enabled,
                                            ; Convergent rounding


;--------------------------------------------------------------------
; Input data structure -> pZ
; Output vector        -> pX
; Twiddle factor table -> pRIFFT->Twiddle
;--------------------------------------------------------------------
; Real inverse FFT of length 2N
;
; pX[0].real = pZ->z0/2 + pZ->zNDiv/2
; pX[0].imag = pZ->z0/2 - pZ->zNDiv/2
;
; pX[N/2].real = pZ->cz[N/2-1].real
; pX[N/2].imag = - pZ->cz[N/2-1].imag
;
; For rest of N/2 - 2 values
;
; Let cz[i] = ar + j ai
;     Twiddle = wr + j wi
;     cz[N-i] = br + j bi
;     pX[i] = xr + j xi
;     pX[N-i] = yr + j yi
;
; xr = (ar + br)/2 + (ar -br)*wi/2 - (ai + bi)*wr/2
; xi = (ai - bi)/2 + (ai +bi)*wi/2 + (ar - br)*wr/2
; yr = (ar + br) - xr
; yi = xi - (ai - bi)'
;--------------------------------------------------------------------


    move.w    x:(r2+Offset_options),x0
    brset     #FFT_SCALE_RESULTS_BY_N,x0,ScaleByN
    andc      #FFT_SCALE_RESULTS_BY_DATA_SIZE,x0
    tst.w     x0                            ; Check the type of scaling
    beq       >DonotScale

    move.w    x:(r2+Offset_n),y0            ; y0 = N.
    tfra      r3,r0                         ; r0 -> pX
    move.w    #0,x0                         ; x0 = smallest positive number.
    move.w    #0,ScaleBF                    ; Clear ScaleBF counter for future
                                            ;  use.
    move.w    x:(r0)+,a                     ; a = First data value (real).

    if CODEWARRIOR_WORKAROUND==1
    do        y0,>>Magnitudechk
    else
    do        y0,Magnitudechk
    endif

    abs       a            x:(r0)+,b        ; a = |a|, b = Next data value
                                            ;  (imag).
    or.w      a,x0                          ; Get the most significant bit of
                                            ;  the largest number, in x0.
    abs       b            x:(r0)+,a        ; b = |b|, a = Next data value
                                            ;  (real).
    or.w      b,x0                          ; Get the most significant bit of
                                            ;  the largest number, in x0.

Magnitudechk
    tst.w     x0
    blt       _MSB_8000
    cmp.w     #THRESHOLD_BFP,x0             ; Compare the max value with 0.25.
    blt       DonotScale                    ; If <=, no scaling is required in
                                            ;  the first pass.
_MSB_8000

    inc.w     ScaleBF                       ; Increment the count

ScaleByN
    tfra      r4,r1                         ; Pointer to output array
    move.w    x:(r3)+,a                     ; Load pZ->z0
    LoadRx   x:(r2+Offset_Twiddle),r0      ; r0 points to Twiddle factors
    move.w    x:(r3)+,b                     ; Load pZ->zNDiv2
    moveu.w   x:(r2+Offset_n),n             ; Load the FFT length
                                            ;  in x0
    add       a,b          x:(r0)+,y0       ; pX[0].real = pZ->z0/2 +
                                            ;           pZ->zNDiv2/2
    asr       b            x:(r0)+,y0
    sub       b,a          b,x:(r1)+        ; pX[0].imag = pZ->z0/2 -
                                            ;           pZ->zNDiv/2
    move.w    a,x:(r1)+                     ; Store pX[0].imag
    move.w    n,a
    adda      #-2,n                         ; Offset between cz[i] and cz[N-i]
    asla      n
    lsr.w     a                             ; N/2
    sub.w     #1,a                          ; loop count N/2-1

    if CODEWARRIOR_WORKAROUND==1
    do        a,>>_loop_rifft_scale
    else
    do        a,_loop_rifft_scale
    endif

    move.w    x:(r3+n),b                    ; br=cz[N-i].real
    move.w    x:(r3)+,a                     ; ar=cz[i].real
    add       a,b                           ; (ar+br)/2 = cz[i] + cz[N-i]
    asr       b            x:(r0)+,x0       ;  ar
    sub       b,a          b,x:(r1)+        ; ar -(ar+br)/2, load Twiddle factor
                                            ; Twiddle[i].real and update r0 to
                                            ; point to Twiddle[i].imag.
                                            ; Temporary store (ar+br)/2
    move.w    a,y0                          ; y0 = (ar-br)/2
    move.w    x:(r0),y1                     ; Load Twiddle[i].imag
    mac       -y0,y1,b                      ; (ar+br)/2 + (ar-br)*wi/2
    move.w    x:(r3+n),y1                   ; Load bi=cz[N-i].imag
    move.w    x:(r3)+,a                     ; Load ai=cz[i].imag
    sub       y1,a                          ; (ai-bi)
    asr       a                             ; (ai-bi)/2
    add       a,y1                          ; (ai+bi)/2
    move.w    a,x:(r1)-                     ; Temporary store (ai-bi)/2
    macr      -y1,x0,b                      ; xr = (ar+br)/2 +
                                            ; (ar-br)*wi/2 - (ai+bi)*wr/2
    mac       y0,x0,a      x:(r0)+,x0       ; (ai-bi)/2 + (ar-br)*wr/2
    macr      -y1,x0,a                      ; xi = (ai-bi)/2 + (ai+bi)*wi/2
                                            ; -(ar+br)*wr/2
    move.w    x:(r1),x0                     ; load (ar+br)/2
    asl       x0                            ; (ar+br)
    sub       b,x0                          ; yr = (ar+br) - xr
    move.w    x0,x:(r1+n)                   ; store pX[N-i].real
    move.w    b,x:(r1)+                     ; store pX[i].real
    move.w    x:(r1),b                      ; Load (ai-bi)/2
    asl       b                             ; (ai-bi)
    move.w    a,x0                          ; Temporary store
    sub       b,x0                          ; yi = xi -(ai-bi)
    move.w    x0,x:(r1+n)                   ; store pX[N-i].imag
    move.w    a,x:(r1)+                     ; store pX[i].imag
    adda      #-4,n                         ; Update the offset for next
                                            ; butterfly

_loop_rifft_scale

    move.w    x:(r3)+,a                     ; cz[N/2].real
    move.w    x:(r3),b                      ; cz[N/2].imag
    neg       b            a,x:(r1)+
    move.w    b,x:(r1)

    bra       EndofProcessing

DonotScale


    tfra      r4,r1                         ; Pointer to output array
    move.w    x:(r3)+,a                     ; Load pZ->z0
    LoadRx    x:(r2+Offset_Twiddle),r0      ; r0 points to Twiddle factors
    move.w    x:(r3)+,b                     ; Load pZ->zNDiv2
    moveu.w   x:(r2+Offset_n),n             ; Load the FFT length
                                            ;  in x0
    add       a,b          x:(r0)+,y0       ; pX[0].real = pZ->z0/2 +
                                            ;           pZ->zNDiv2/2
    asl       a            x:(r0)+,y0
    sub       b,a          b,x:(r1)+        ; pX[0].imag = pZ->z0/2 -
                                            ;           pZ->zNDiv/2
    move.w    a,x:(r1)+                     ; Store pX[0].imag
    move.w    n,a
    adda      #-2,n                         ; Offset between cz[i] and cz[N-i]
    asla      n
    lsr.w     a                             ; N/2
    sub.w     #1,a                          ; loop count N/2-1

    if CODEWARRIOR_WORKAROUND==1
    do        a,>>_loop_rifft_noscale
    else
    do        a,_loop_rifft_noscale
    endif

    move.w    x:(r3+n),b                    ; br=cz[N-i].real
    move.w    x:(r3)+,a                     ; ar=cz[i].real
    add       a,b                           ; (ar+br) = cz[i] + cz[N-i]
    asl       a            x:(r0)+,x0       ; ar
    sub       b,a          b,x:(r1)+        ; 2*ar -(ar+br), load Twiddle factor
                                            ; Twiddle[i].real and update r0 to
                                            ; point to Twiddle[i].imag.
                                            ; Temporary store (ar+br)
    move.w    a,y0                          ; y0 = (ar-br)
    move.w    x:(r0),y1                     ; Load Twiddle[i].imag
    mac       -y0,y1,b                      ; (ar+br) + (ar-br)*wi
    move.w    x:(r3+n),y1                   ; Load bi=cz[N-i].imag
    move.w    x:(r3)+,a                     ; Load ai=cz[i].imag
    add       a,y1                          ; (ai+bi)
    asl       a                             ; ai
    sub       y1,a                          ; (ai-bi)
    move.w    a,x:(r1)-                     ; Temporary store (ai-bi)
    macr      -y1,x0,b                      ; xr = (ar+br) +
                                            ; (ar-br)*wi - (ai+bi)*wr
    mac       y0,x0,a      x:(r0)+,x0       ; (ai-bi) + (ar-br)*wr
                                            ; Load Twiddle[i].imag and update
                                            ; the pointer to point to
                                            ; Twiddle[i+1].real
    macr      -y1,x0,a                      ; xi = (ai-bi) + (ai+bi)*wi
                                            ; -(ar+br)*wr
    move.w    x:(r1),x0                     ; load (ar+br)
    asl       x0                            ; (ar+br)
    sub       b,x0                          ; yr = 2*(ar+br) - xr
    move.w    x0,x:(r1+n)                   ; store pX[N-i].real
    move.w    b,x:(r1)+                     ; store pX[i].real
    move.w    x:(r1),b                      ; Load (ai-bi)
    asl       b                             ; (ai-bi)
    move.w    a,x0                          ; Temporary store
    sub       b,x0                          ; yi = xi -2*(ai-bi)'
    move.w    x0,x:(r1+n)                   ; store pX[N-i].imag
    move.w    a,x:(r1)+                     ; store pX[i].imag
    adda      #-4,n                         ; Update the offset for next
                                            ; butterfly
_loop_rifft_noscale

    move.w    x:(r3)+,a                     ; cz[N/2].real
    move.w    x:(r3),b                      ; cz[N/2].imag
    asl       a
    asl       b
    neg       b            a,x:(r1)+
    move.w    b,x:(r1)

EndofProcessing
;-----------------------------------------------
; Decide whether scaling is AS or BFP
;-----------------------------------------------

    move.w    x:(r5+Offset_options),x0
    brset     #FFT_SCALE_RESULTS_BY_N,x0,AutoScaling
    andc      #FFT_SCALE_RESULTS_BY_DATA_SIZE,x0
    tst.w     x0
    bne       >BlockFloatingPoint           ; Block floating point...
    bra       >NoScaling                    ; If not, it implies no scaling.

AutoScaling

;----------------------------
; CASE 1: Code for AS.
;----------------------------

    moveu.w   x:(r5+Offset_n),n
    tfra      r4,r2
    tfra      r2,r1
    tfra      r1,r3                         ; r3 -> 1st Ar of first pass
    adda      n,r3                          ; r3 -> 1st Br of 1st pass
    tfra      r2,r0                         ; r0 -> 1st Br of first pass
    move.w    x:(r1)+,a                     ; Get 1st Ar of 1st pass
    adda      n,r0
    deca      r0                            ; Adjust r0 so that in first
    asra      n                             ; half the no. of DFT points

    move.w    x:(r0),b                      ; Save the memory cotents so
                                            ;  that first parallel move
                                            ;  doesn't corrupt the data

    move.w    x:(r3)+,y0                    ; Get 1st Br of 1st pass

    if CODEWARRIOR_WORKAROUND==1
    do        n,>>_first_pass               ; The first pass has to be
                                            ;  repeated length/2 times
    else
    do        n,_first_pass                 ; The first pass has to be
                                            ;  repeated length/2 times
    endif

    add       y0,a         b,x:(r0)+        ; Find Cr,save Di in previous
                                            ;  Bi. r0 -> Br
    asr       a            x:(r1)+,b        ; Find Cr/2,get Ai,r1 -> next Ar
    rnd       a            x:(r3)+,x0       ; round, get Bi, r3 -> next Br
    sub       y0,a         a,x:(r2)+        ; Find Dr/2 ,save Cr/2,r2 -> Ci
    add       x0,b         a,x:(r0)+        ; Find Ci,save Dr/2 ,r0 ->Di
    asr       b            x:(r1)+,a        ; Find Ci/2, get next Ar,
                                            ;  r1 -> next Ai
    rnd       b            x:(r3)+,y0       ; round Ci/2, get next Br,
                                            ;  r3 -> next Bi
    sub       x0,b         b,x:(r2)+        ; Find Di/2, save Ci/2 ,
                                            ;  r2 -> next Cr. Di/2 saved
                                            ;  in the next loop
_first_pass
    move.w    b,x:(r0)                      ; Save last Di/2 of the 1st pass
    tfra      r4,r1                         ; r1 -> 1st Ar of 2nd pass
    tfra      r1,r3                         ; r3 -> 1st Ar of 2nd pass
                                            ; n set to N, used for
                                            ;  controlling the no.of bflies
                                            ;  per group in the pass.
    move.w    #2,c                          ; c1 set to 2,used for controlling
                                            ;  the no. of groups in the pass
    adda      n,r3                          ; r3 -> first Br of second pass
    move.w    x:(r5+Offset_No_of_Stages),d  ; Set counter for no. of passes
    sub.w     #2,d                          ; last pass is also separate
    tst.w     d1
    beq       _fft_4
_second_pass
    dec.w     d

    LoadRx    x:(r5+Offset_Twiddle_br),r0   ; r0 ->mem. location of the first
                                            ;  twiddle fac. ,twiddle fac.
                                            ;  stored in bit reversed fashion
    asra      n

    tfra      r3,r2                         ; r2 -> first Br of the first pass

    if CODEWARRIOR_WORKAROUND==1
    do        c1,>>_end_group               ; Middle loop is done b times
                                            ; c1=2**(pass number-1)
                                            ; c1 = no. of groups/pass
    else
    do        c1,_end_group                 ; Middle loop is done b times
                                            ; c1=2**(pass number-1)
                                            ; c1 = no. of groups/pass
    endif

    move.w    x:(r0)+,y0                    ; y0=Wr, r0 ->Wi
    move.w    x:(r0)+,b                     ; b=Wi, r0 -> next Wr
                                            ;  (in bit reversed order)
    neg       b            x:(r3)+,x0       ; x0=Br, r3 ->Bi
    move.w    b,y1                          ; y1 = -Wi.

    moveu.w   r0,n3

    tfra      r1,r0                         ; Move r1 to r0
    deca      r2

    move.w    x:(r2),b                      ; Save the contents so that the
                                            ;  mem. contents aren't corrupted
                                            ;  in the first middle loop
    if CODEWARRIOR_WORKAROUND==1
    do        n,>>_end_bfly                 ; Inner loop is done n times
                                            ;  n=2**(L-passnumber)
                                            ;  n=no. of butterflies/group
    else
    do        n,_end_bfly                   ; Inner loop is done n times
                                            ;  n=2**(L-passnumber)
                                            ;  n=no. of butterflies/group
    endif

    mpy       y0,x0,b      b,x:(r2)+        ; b=WrBr,store the previous
                                            ;  butterfly's Di,r1->current Ar
    mpy       y1,x0,a      x:(r3)+,x0       ; a=WiBr,get Bi
    mac       -y0,x0,a                      ; a=-WrBi+WiBr
    mac       y1,x0,b      x:(r1)+,x0       ; b=WrBr+WiBi,x0=Ar,r1 -> Ai
    add       x0,b                          ; Find Cr in b
    asr       b                             ; b=Cr/2,Scale down for storage
    rnd       b                             ; Round.The rounding done here
                                            ;  closely matches with the
                                            ;  autoscale mode rounding
    sub       b,x0                          ; Find Dr/2
    neg       a            b,x:(r0)+        ; a= WrBi-WiBr
                                            ;  Store Cr/2
    move.w    x:(r1)+,b                     ; fetch Ai into b,r1 -> next Ar
    add       b,a          x0,x:(r2)+       ; Find Ci ,store Dr/2,r2 -> Di
    asr       a            x:(r3)+,x0       ; a=Ci/2,x0 = Next Br,
                                            ;  r3 -> Next Bi
    rnd       a                             ; Round
    sub       a,b          a,x:(r0)+        ; b = Di/2, a = Ci/2
                                            ;  store Ci/2
                                            ;  Di/2 stored in next loop
_end_bfly


    move.w    b,x:(r2)+                     ; Store last butterfly's Ci

    moveu.w   n3,r0                         ; Restore the pointer pointing
                                            ;  to the twiddle factors

    tfra      r2,r1

    adda.l    n,r2
    tfra      r2,r3                         ; r3 -> next group's first Br
_end_group

    tfra      r4,r1                         ; r1 ->1st Ar,at start of each pass
    tfra      r1,r3                         ; r3 ->1st Ar,at start of each pass

    asl       c                             ; double the no of groups for next
                                            ;  pass.
    adda      n,r3                          ;  pass,Dummy read  to adjust r3
                                            ;  r3 -> first Br of the next pass
    tst.w     d1                            ; Test the pass counter for Zero

    bne       _second_pass                  ; If less than zero then go to

_fft_4


    LoadRx    x:(r5+Offset_Twiddle_br),r0   ; Get address of twiddle factors
    move.w    x:(r0)+,y0

    if CODEWARRIOR_WORKAROUND==1
    do        c1,>>_last_pass               ; N/2 groups in last pass
    else
    do        c1,_last_pass                 ; N/2 groups in last pass
    endif

    tfra      r3,r2
    move.w    x:(r3)+,x0                    ; y0=Wr,x0=Br,r0 -> Wi,r3 -> Bi
    mpy       x0,y0,b      x:(r0)+,y1       ; b=BrWr, y1=Wi, r0 ->Next Wr
    mpy       y1,x0,a      x:(r3)+,x0       ; a=-WiBr
    mac       y0,x0,a                       ; a=+WrBi-WiBr
    mac       -y1,x0,b                      ; b=WiBi+WrBr
    neg       a            x:(r1)+,x0       ; a=-WrBi+WiBr, x0=Ar, r1 -> Ai

    add      x0,b                           ; Find Cr
    asr      b                              ; Find Cr/2
    rnd      b                              ; Round
    sub      b,x0                           ; Find Dr/2
    move.w   b,x:(r1-1)                     ; Store Cr/2 , r1 -> Ai
    move.w   x:(r1)+,b                      ; b = Ai, r1 -> Ai
    sub      b,a           x0,x:(r2)+       ; Find -Ci , store Dr/2,r2 ->Di
    asr      a             x:(r3)+n,x0      ; Find -Ci/2, Dummy read to
                                            ;  adjust r3 to next Br
    rnd      a             x:(r0)+,y0
    add      a,b           x:(r1)+n,x0      ; Find Di/2,Dummy read to
                                            ;  adjust r1
    neg      a             b,x:(r2)+        ; Find Ci/2 ,store Di/2
    move.w   a,x:(r1-3)                     ; Store Ci/2
_last_pass
_End_fft
    bra      >PostProcessing


;---------------------------------------------------------------
; CASE 2: Code for BFP
;---------------------------------------------------------------

BlockFloatingPoint

;---------------------------------------------------------------
; See whether the magnitude of any of the data values is > 0.25
;---------------------------------------------------------------

    moveu.w   x:(r5+Offset_n),n
    tfra      r4,r1
    tfra      r1,r3                         ; r3 -> 1st Ar of 1st pass
    move.w    x:(r5+Offset_No_of_Stages),x0 ; move    passes,x0
    dec.w     x0
    move.w    x0,tmp
    move.w    #1,d                          ; r2 set to 1, for controlling
                                            ;  the no. of groups in the pass
    adda      n,r3                          ; r3 -> first Br of first pass

_first_passBF
    tfra      r1,r0                         ; r0 -> first Br
    move.w    x:(r5+Offset_n),y0            ; y0= number of points in fft
    move.w    #0,x0                         ; x0= 0; smallest positive number
    move.w    x:(r0)+,a                     ; a= first Ar, r0 -> First Ai

    if CODEWARRIOR_WORKAROUND==1
    do        y0,>>_chkp                    ; Repeat for all samples
    else
    do        y0,_chkp                      ; Repeat for all samples
    endif

    abs       a            x:(r0)+,b        ; Get the magnitude in acc a
                                            ;  get the next data in acc b
    or.w      a,x0                          ; Get the most significant bit of
                                            ;  the largest number in x0
    abs       b            x:(r0)+,a        ; Get the magnitude of next data
                                            ;  in b and get next data in a
    or.w      b,x0                          ; Find the max value in x0
_chkp
    LoadRx    x:(r5+Offset_Twiddle_br),r0   ; r0 ->memory location of the
                                            ;  first twiddle factor Wr
    asra      n
    tfra      r3,r2                         ; r2 -> first Br of the pass
    tst.w     x0
    blt       _MSB_8000_2
    cmp.w     #$2000,x0                     ; Compare the max value with 0.25
    blt       _nscdown                      ; If greater, perform the sc_down
                                            ;  pass, else no scale down pass
_MSB_8000_2

    inc.w     ScaleBF                       ; Increment the scale_fac by one

    if CODEWARRIOR_WORKAROUND==1
    do        d1,>>_end_groupBF             ; Middle loop is done d1 times
                                            ;  d1=2**(pass number-1)
                                            ;  d1 = no. of groups/pass
    else
    do        d1,_end_groupBF               ; Middle loop is done d1 times
                                            ;  d1=2**(pass number-1)
                                            ;  d1 = no. of groups/pass
    endif

    move.w    x:(r0)+,y0
    move.w    x:(r3)+,x0                    ; y0=Wr, x0=Br, r0 -> Wi, r3 -> Bi
    move.w    x:(r0)+,y1                    ; y1=Wi, r0 -> next Wr
                                            ;  (in bit reversed order)

    moveu.w   r0,n3                         ; Save twiddle factor

    tfra      r1,r0

    deca      r2
    move.w    x:(r2),b                      ; Save the contents so that they
                                            ;  are not corrupted in the first
                                            ;  middle loop
    if CODEWARRIOR_WORKAROUND==1
    do        n,>>_end_bflyBF               ; Inner loop is done n times
                                            ;  n=2**(L-passnumber)
                                            ;  n=no. of butterflies/group
    else
    do        n,_end_bflyBF                 ; Inner loop is done n times
                                            ;  n=2**(L-passnumber)
                                            ;  n=no. of butterflies/group
    endif

    mpy       y0,x0,b      b,x:(r2)+        ; b=WrBr, store previous butter-'
                                            ;  fly's Di , r1 -> current Ar
    mpy       y1,x0,a      x:(r3)+,x0       ; a=+WiBr,
                                            ; get Bi
    mac       y0,x0,a                       ; a=-WrBi+WiBr
    mac       -y1,x0,b                      ; b=WrBr+WiBi,
    move.w    x:(r1)+,x0                    ; x0=Ar,r1 -> Ai
    add       x0,b                          ; Find Cr in b
    asr       b                             ; b=Cr/2, Scale down for storage
    rnd       b                             ; Round. The rounding done here
                                            ;  closely matches with the
                                            ;  autoscale down mode rounding
    sub       b,x0                          ; Find Dr/2
    move.w    b,x:(r0)+                     ; a = WrBi - WiBr
                                            ;  store Cr/2
    move.w    x:(r1)+,b                     ; Fetch Ai into b, r1-> next Ar
    add       b,a          x0,x:(r2)+       ; Find Ci, store Dr/2, r2-> Di
    asr       a            x:(r3)+,x0       ; a=Ci/2, x0= Next Br,
                                            ;  r3-> Next Bi
    rnd       a                             ; Round
    sub       a,b          a,x:(r0)+        ; b=Di/2, a=Ci/2, store Ci/2
                                            ;  Di/2 is stored in next loop
_end_bflyBF
    move.w    b,x:(r2)+                     ; Store last butterfly's Ci

    moveu.w   n3,r0                         ; Restore the pointer pointing
                                            ;  to the twiddle factors

    tfra      r2,r1                         ; r1 -> next group's first Ar
    adda.l    n,r2

    tfra      r2,r3                         ; r3 -> next group's first Br
_end_groupBF
    tfra      r4,r3                         ; r1 -> 1st Ar, for start of each
    tfra      r3,r1                         ;  pass
    asl       d                             ; Double the no of groups for next
                                            ;  pass
    adda      n,r3                          ;  r3-> first Br of the next pass
    dec.w     tmp                           ; Test for pass counter
    bne       _first_passBF                 ; perform the loop till counter= 0
    bra       _lpass

_nscdown                                    ; No scale down mode
    move.w    n,c1
    moveu.w   #0,n

    if CODEWARRIOR_WORKAROUND==1
    do        d1,>>_end_group2              ; Middle loop is done d1 times'
                                            ; d1=2**(pass number-1)
    else
    do        d1,_end_group2                ; Middle loop is done d1 times'
                                            ; d1=2**(pass number-1)
    endif

    move.w    x:(r0)+,y0
    move.w    x:(r3)+,x0                    ; y0=Wr, x0=first Br of the
                                            ;  group. r0->Next Wr. r3->first
                                            ;  Bi of the group.

    if CODEWARRIOR_WORKAROUND==1
    do        c1,>>_end_bfly2               ; Inner loop is done c1 times
                                            ;  c1=2**(L-passnumber)
    else
    do        c1,_end_bfly2                 ; Inner loop is done c1 times
                                            ;  c1=2**(L-passnumber)
    endif

    mpy       x0,y0,b      x:(r0)+n,y1
    move.w    x:(r3)-,x0
                                            ; y1=Wi, b=WrBr, x0=Bi, r3->Br
                                            ;  r0->Wi
    macr      -y1,x0,b                      ; b=WrBr+WiBi,
    move.w    x:(r1)+n,a                    ; a=Ar, r1->Ar
    add       a,b                           ; Find Cr in b, b=Ar+WrBr+WiBi,
    asl       a            b,x:(r1)+        ; a=2Ar, store Cr. r1->Ai
    sub       b,a          x:(r3)+,b        ; Find Dr in a, b=Br, r3->Bi
    mpy       b1,y1,b      a,x:(r2)+
    move.w    x:(r3)+,x0                    ; b=-Brwi, x0=Bi, r3->Next Br
    macr      y0,x0,b      x:(r1)+n,a       ; b=WrBi-WiBr, a=Ai, r1->Ai
    add       a,b          x:(r3)+,x0       ; Find Ci in b, x0=Next Br,
                                            ;  r3->Next Bi
    asl       a            b,x:(r1)+        ; a=2Ai, store Ci, r1->Next Ar
    sub       b,a                           ; Find Di
    move.w    a,x:(r2)+                     ; Store Di, r2->Next Br

_end_bfly2
    adda      #1,r0
    moveu.w   c1,r3
    tfra      r2,r1                         ; r1->next group,s first Ar
    adda.l    r3,r2                         ; r2 -> next group's first Br
    tfra      r2,r3                         ; r3-> Next groups first Br

_end_group2
    moveu.w   c1,n
    tfra      r4,r3                         ; r3 -> 1st Ar of next pass
    tfra      r3,r1                         ; r1 -> 1st Ar of next pass
    asl       d                             ; Double the no of groups for next
                                            ;  pass
    adda      n,r3                          ;  adjust r3, r3 -> first Br of
                                            ;  the next pass
    dec.w      tmp                          ; Test the pass counter for zero

    bne       >_first_passBF                ; If not equal to zero then go to
                                            ;  beginning of the pass

_lpass                                      ; data scanning for the last pass
    tfra      r4,r0                         ; r0->first Ar
    move.w    x:(r5+Offset_n),y0            ; y0= number of points in fft
    move.w    #0,x0                         ; x0= 0; smallest positive number
    move.w    x:(r0)+,a                     ; a= first Ar, r0 -> First Ai'

    if CODEWARRIOR_WORKAROUND==1
    do        y0,>>_chklp                   ; Repeat' for all samples
    else
    do        y0,_chklp                     ; Repeat' for all samples
    endif

    abs       a            x:(r0)+,b        ; Get the magnitude in acc a
                                            ;  get the next data in acc b
    or.w      a,x0                          ; Get the most significant bit of
                                            ;  the largest number in x0
    abs       b            x:(r0)+,a        ; Get the magnitude of next data
                                            ;  in b and get next data in a
    or.w      b,x0                          ; Find the max value in x0

_chklp
    LoadRx    x:(r5+Offset_Twiddle_br),r0   ; r0->first Wr
    tst.w     x0
    blt       _MSB_8000_3
    cmp.w     #0.25,x0                      ; if the largest value is > 0.25
    blt       _lnscdown                     ;  perform scale down pass

_MSB_8000_3

    inc.w     ScaleBF                       ; Increment the scale factor
    move.w    x:(r0)+,y0

    if CODEWARRIOR_WORKAROUND==1
    do        d1,>>_last_pass1              ; N/2 groups in last pass
    else
    do        d1,_last_pass1                ; N/2 groups in last pass
    endif

    tfra      r3,r2
    move.w    x:(r3)+,x0                    ; y0=Wr, x0=Br, r0 -> Wi, r3 -> Bi
    mpy       x0,y0,b      x:(r0)+,y1       ; b=BrWr, y1=Wi, r0 -> Next Wr
    mpy       y1,x0,a      x:(r3)+,x0       ; a=-WiBr
    mac       y0,x0,a                       ; a=WrBi-WiBr
    mac       -y1,x0,b                      ; b=WiBi+WrBr
    neg       a            x:(r1)+,x0       ; a=-WrBi+WiBr, x0=Ar, r1 -> Ai
    add       x0,b                          ; Find Cr
    asr       b                             ; Find Cr/2
    rnd       b                             ; Round
    sub       b,x0                          ; Find Dr/2
    move.w    b,x:(r1-1)                    ; Store Cr/2 , r1 -> Ai
    move.w    x:(r1)+,b                     ; b = Ai, r1 -> Ai
    sub       b,a          x0,x:(r2)+       ; Find -Ci , store Dr/2,r2 ->Di
    asr       a            x:(r3)+n,x0      ; Find -Ci/2, Dummy read to adjust
                                            ;  r3 to next Br
    rnd       a            x:(r0)+,y0       ; Round,Dummy read to adjust r1
                                            ; r1 -> Next Ar
    add       a,b          x:(r1)+n,x0      ; Find Di/2
    neg       a            b,x:(r2)+        ; Find Ci/2 ,store Di/2
    move.w    a,x:(r1-3)                    ; Store Ci/2
_last_pass1
    bra       _rev                          ; Perform bitreversal operation
_lnscdown                                   ; Last pass no scale down
    moveu.w   #0,n                          ; Offset for the last pass
    move.w    x:(r0)+,y0

    if CODEWARRIOR_WORKAROUND==1
    do        d1,>>_last_pass2              ; N/2 number of butterflies
    else
    do        d1,_last_pass2                ; N/2 number of butterflies
    endif

    move.w    x:(r3)+,x0                    ; y0=Wr, x0=Br, r0->Wi, r3->Bi
    mpy       x0,y0,b      x:(r0)+,y1       ; y1=Wi
    move.w    x:(r3)-,x0
                                            ; b=WrBr, y1=Wi, x0=Bi, r0->Wi
                                            ;  r3->Br

    macr      -y1,x0,b
    move.w    x:(r1)+n,a                    ; b=WrBr+WiBi, a=Ar, r1->Ar
    add       a,b                           ; Find Cr in b, b=Ar+WrBr+WiBi,
    asl       a            b,x:(r1)+        ; a=2Ar, store Cr, r1->Ai
    sub       b,a          x:(r3)+n,b       ; Find Dr in a, a=2Ar-Cr, b=Br
                                            ;  r3->Br
    mpy       b1,y1,b      a,x:(r3)+        ; b=-Br, store Dr, r3->Bi
    move.w    x:(r3)+n,x0                   ; b=-BrWi, x0=Bi, r3->Bi
    macr      y0,x0,b      x:(r1)+n,a       ; b=WrBi-BrWi, a=Ai, r1->Ai
    add       a,b          x:(r0)+,y0       ; find Ci in acc b
    asl       a            b,x:(r1)+        ; a=2Ai, store Ci, r1->Br
    sub       b,a          x:(r1)+,x0       ; Find Di in acc a, dummy read
                                            ;  in x0, r1->Bi
    move.w    a,x:(r3)+                     ; Store Di, r3-> Ar of next
                                            ;  butterfly
    adda      #1,r1                         ;  r1->Ar of next
    adda      #2,r3                         ;  butterfly
                                            ; r3-> Br of next butterfly


_last_pass2
_rev

    bra      >PostProcessing

NoScaling

;---------------------------------------------------------------
; CASE 3: Code for no-scaling.
;---------------------------------------------------------------

    moveu.w   x:(r2+Offset_n),n
    tfra      r4,r2
    tfra      r2,r1
    tfra      r1,r3                         ; r3 -> 1st Ar of first pass
    adda      n,r3                          ; r3 -> 1st Br of 1st pass
    tfra      r2,r0                         ; r0 -> 1st Br of first pass
    move.w    x:(r1)+,a                     ; Get 1st Ar of 1st pass
    adda      n,r0
    deca      r0                            ; Adjust r0
    asra      n                             ; half the no. of DFT points
    move.w    x:(r0),b                      ; Save the memory cotents so
                                            ;  that first parallel move
                                            ;  doesn't corrupt the data
    move.w    x:(r3)+,y0                    ; Get 1st Br of 1st pass

    if CODEWARRIOR_WORKAROUND==1
    do        n,>>_first_pass_NS            ; The first pass has to be
                                            ; repeated length/2 times
    else
    do        n,_first_pass_NS              ; The first pass has to be
                                            ; repeated length/2 times
    endif

    add       y0,a         b,x:(r0)+        ; Find Cr,save Di in previous
    rnd       a            x:(r3)+,x0       ; round, get Bi, r3 -> next Br
    sub       y0,a         a,x:(r2)+        ; save Cr,r2 -> Ci
    sub       y0,a         x:(r1)+,b        ; Find Dr,get Ai,r1 -> next Ar
    add       x0,b         a,x:(r0)+        ; Find Ci,save Dr ,r0 ->Di
    rnd       b            x:(r3)+,y0       ; round Ci, get next Br,
                                            ; r3 -> next Bi
    sub       x0,b         b,x:(r2)+        ; Find Di, save Ci,
                                            ; r2 -> next Cr.
    sub       x0,b         x:(r1)+,a        ; Find Di, get next Ar,
                                            ; in the next loop
_first_pass_NS
    move.w    b,x:(r0)                      ; Save last Di/2 of the 1st pass
    tfra      r4,r1                         ; r1 -> 1st Ar of 2nd pass
    tfra      r1,r3                         ; r3 -> 1st Ar of 2nd pass
                                            ; n set to N, used for
                                            ; controlling the no.of bflies
                                            ; per group in the pass.
    move.w    #2,c                          ; r2 set to 2,used for controlling
                                            ; the no. of groups in the pass
    adda      n,r3                          ; r3 -> first Br of second pass
    move.w    x:(r5+Offset_No_of_Stages),d  ; Set counter for no. of passes
    sub.w     #2,d                          ; last pass is also separate
    tst.w     d1
    beq       _fft_4_NS

_second_pass_NS

    LoadRx    x:(r5+Offset_Twiddle_br),r0   ; r0 ->mem. location of the first
                                            ; twiddle fac. ,twiddle fac.
                                            ; stored in bit reversed fashion
    dec.w     d                             ; Decrement Counter
    asra      n
    tfra      r3,r2                         ; r2 -> first Br of the first pass

    if CODEWARRIOR_WORKAROUND==1
    do        c1,>>_end_group_NS            ; Middle loop is done c1 times
                                            ; c1=2**(pass number-1)
    else
    do        c1,_end_group_NS              ; Middle loop is done c1 times
                                            ; c1=2**(pass number-1)
    endif

    move.w    x:(r0)+,y0                    ; y0=Wr,r0 ->Wi
    move.w    x:(r0)+,b                     ; y1=Wi, r0 -> next Wr
                                            ; (in bit reversed order)
    neg       b            x:(r3)+,x0       ; x0=Br, r3 ->Bi
    move.w    b,y1                          ; y1 = -Wi.

    moveu.w   r0,n3                         ; Save twiddle factor pointer

    tfra      r1,r0                         ; Move r1 to r0
    deca      r2
    move.w    x:(r2),b                      ; Save the contents so that the
                                            ; mem. contents aren't corrupted
                                            ; in the first middle loop
    if CODEWARRIOR_WORKAROUND==1
    do        n,>>_end_bfly_NS              ; Inner loop is done n times
                                            ; n=2**(L-passnumber)
                                            ; n=no. of butterflies/group
    else
    do        n,_end_bfly_NS                ; Inner loop is done n times
                                            ; n=2**(L-passnumber)
                                            ; n=no. of butterflies/group
    endif

    mpy       y0,x0,b      b,x:(r2)+        ; b=WrBr,store the previous
                                            ; butterfly's Di,r1->current Ar
    mpy       y1,x0,a      x:(r3)+,x0       ; a=+WiBr,get Bi
    mac       -y0,x0,a                      ; a=-WrBi+WiBr
    mac       y1,x0,b      x:(r1)+,x0       ; b=WrBr+WiBi,x0=Ar,r1 -> Ai
    add       x0,b                          ; Find Cr in b
    rnd       b                             ; Round.The rounding done here
                                            ; closely matches with the
                                            ; autoscale mode rounding
    asl       x0                            ; Get 2*Ar
    sub       b,x0                          ; Find Dr [ Dr = 2*Ar - Cr]
    neg       a            b,x:(r0)+        ; a= WrBi-WiBr
                                            ; Store Cr
    move.w    x:(r1)+,b                     ; fetch Ai into b,r1 -> next Ar
    add       b,a          x0,x:(r2)+       ; Find Ci ,store Dr,r2 -> Di
                                            ; r3 -> Next Bi
    rnd       a            x:(r3)+,x0       ; Round
    asl       b                             ; Get 2*Ai
    sub       a,b          a,x:(r0)+        ; Find Di,[Di = 2*Ai - Ci]
                                            ; store Ci
                                            ; Di stored in next loop
_end_bfly_NS

    move.w    b,x:(r2)+                     ; Store last butterfly's Ci

    moveu.w   n3,r0                         ; Restore the pointer pointing
                                            ;  to the twiddle factors

    tfra      r2,r1                         ; r1 -> next group's first Ar
    adda.l    n,r2                          ; r2 -> next group's first Br
    tfra      r2,r3                         ; r3 -> next group's first Br

_end_group_NS
    tfra      r4,r1                         ; r1 ->1st Ar,at start of each pass
    tfra      r1,r3                         ; r3 ->1st Ar,at start of each pass
    asl       c                             ; double the no of groups for next
                                            ;  pass
    adda      n,r3                          ; r3 -> first Br of the next pass
    tst.w     d1                            ; Test the pass counter for Zero
    bne       _second_pass_NS               ; If less than zero then go to
                                            ; last pass .
_fft_4_NS


    LoadRx    x:(r5+Offset_Twiddle_br),r0   ; Get address of twiddle factors
    move.w    x:(r0)+,y0

    if CODEWARRIOR_WORKAROUND==1
    do        c1,>>_last_pass_NS            ; N/2 groups in last pass
    else
    do        c1,_last_pass_NS              ; N/2 groups in last pass
    endif

    tfra      r3,r2
    move.w    x:(r3)+,x0                    ; y0=Wr,x0=Br,r0 -> Wi,r3 -> Bi
    mpy       x0,y0,b      x:(r0)+,y1       ; b=BrWr, y1=Wi, r0 ->Next Wr
    mpy       y1,x0,a      x:(r3)+,x0

    mac       y0,x0,a
    mac       -y1,x0,b
    neg       a    x:(r1)+,x0               ; b=WiBi+WrBr, x0=Ar, r1 -> Ai
    add       x0,b                          ; Find Cr
    rnd       b                             ; Round
    asl       x0                            ; Get 2*Ar
    sub       b,x0                          ; Find Dr [Dr = 2*Ar - Cr]
    move.w    b,x:(r1-1)                    ; Store Cr , r1 -> Ai
    move.w    x:(r1)+,b                     ; b = Ai, r1 -> Ai
    sub       b,a          x0,x:(r2)+       ; Find -Ci , store Dr,r2 ->Di
    rnd       a            x:(r3)+n,x0      ; Find -round(Ci), Dummy read to
                                            ; adjust r3 to next Br
    asl       b            x:(r0)+,y0       ; Get 2*Ai
    add       a,b          x:(r1)+n,x0      ; Find Di,Dummy read to
                                            ; adjust r1
    neg       a            b,x:(r2)+        ; Find Ci ,store Di
    move.w    a,x:(r1-3)                    ; Store Ci
_last_pass_NS
_End_fft_NS

PostProcessing

;--------------------------------------------------------
; Set up the parameters for calling bit-reverse function
;--------------------------------------------------------


    move.w    x:(r5+Offset_n),y0            ; y0 = N, the size of FFT.
    tfra      r4,r2                         ; r2 -> pZ, r3 -> pZ.
    tfra      r2,r3
    jsr       Fdfr16Cbitrev_                ; Call bit reverse function.

    cmp.w     #FAIL,y0
    bne       FFT_Computation               ; Bit reverse passed, go to FFT.

    moveu.w   old_omr,omr                   ; Restore previous OMR value.
    adda      #-Scratch_Size,sp             ; Restore SP
    move.l    x:(sp)-,r5
    move.l    x:(sp)-,d
    move.l    x:(sp)-,d2
    move.l    x:(sp)-,c
    move.l    x:(sp)-,c2

    rts                                     ; FAIL is returned in y0.


FFT_Computation

;----------------------------------------------------------------
; Return the amount of scaling done in terms of number of shifts
;----------------------------------------------------------------

    move.w    x:(r5+Offset_options),x0
    brclr     #FFT_SCALE_RESULTS_BY_N,x0,return_BFP
                                            ; Is it AS?
    move.w    x:(r5+Offset_No_of_Stages),y0
    add.w     #1,y0                         ; Return amount of scaling done
    moveu.w   old_omr,omr                   ; Restore previous OMR value.
    adda      #-Scratch_Size,sp             ; Restore SP
    move.l    x:(sp)-,r5
    move.l    x:(sp)-,d
    move.l    x:(sp)-,d2
    move.l    x:(sp)-,c
    move.l    x:(sp)-,c2

    rts

return_BFP
    brclr     #FFT_SCALE_RESULTS_BY_DATA_SIZE,x0,return_NS
    move.w    ScaleBF,y0                    ; Return amount of scaling done
    moveu.w   old_omr,omr                   ; Restore previous OMR value.
    adda      #-Scratch_Size,sp             ; Restore SP
    move.l    x:(sp)-,r5
    move.l    x:(sp)-,d
    move.l    x:(sp)-,d2
    move.l    x:(sp)-,c
    move.l    x:(sp)-,c2
    rts

return_NS
    move.w    #0,y0                         ; No scaling is done, return 0.
    moveu.w   old_omr,omr                   ; Restore previous OMR value.
    adda      #-Scratch_Size,sp             ; Restore SP
    move.l    x:(sp)-,r5
    move.l    x:(sp)-,d
    move.l    x:(sp)-,d2
    move.l    x:(sp)-,c
    move.l    x:(sp)-,c2
    rts

    ENDSEC

;************************* End of file ********************************
