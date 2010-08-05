;********************************************************************
;
; (c) Freescale Semiconductor
; (c) Copyright 2000, 2001 Freescale, Inc.
;
;
;********************************************************************
;
; File Name:    dfr16cifft.asm
;
; Description:  56852 Assembly module for Complex IFFT
;
; Modules
;    Included:  Fdfr16CIFFT
;
; Author(s):    Prasad N R (Optimized by Meera S P)
;               Further Optimized by Anuj Pachoree as per 56852
;               specifications
;
; Date:         01 Mar 2000
;
;********************************************************************

    include "portasm.h"

    SECTION rtlib

    GLOBAL  Fdfr16CIFFT

;********************************************************************
;
; Module Name:  Fdfr16CIFFT
;
; Description:  Computes complex IFFT for the given input block.
;
; Functions
;      Called:  Fdfr16Cbitrev - Complex bit reverse function.
;
; Calling
; Requirements: 1. r2 -> Pointer to CIFFT Handle.
;               2. r3 -> Pointer to input buffer whose IFFT is to be
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
; DO loops:     17; max. nesting depth = 2.
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
;    01/03/2000   0.1          Module created   Prasad N R,
;    09/03/2000   1.0          Reviewed &
;                              Baselined        Prasad N R
;    29/01/2001   1.1          Optimized,
;                              Reviewed &
;                              Baselined        Meera S P
;    10/12/2001   1.2          Optimized,       Anuj Pachoree
;                              Reviewed &
;                              Baselined
;
;********************************************************************

Fdfr16CIFFT

Offset_options        equ    0
Offset_n              equ    Offset_options+1
Offset_Twiddle        equ    Offset_n+1
Offset_No_of_Stages   equ    Offset_Twiddle+PTR_SIZE
Scratch_Size          equ    6
START_BFLY_SZ         equ    2            ;To start with, butterfly size = 2.
THRESHOLD_BFP         equ    0.25         ;Minimum absolute value used in
                                          ;  BFP for comparison.
FFT_DEFAULT_OPTIONS               equ   0 ;Default all options bits
                                          ;  to 0
FFT_SCALE_RESULTS_BY_N            equ   1 ;Unconditionally scale by N

FFT_SCALE_RESULTS_BY_DATA_SIZE    equ   2 ;Scale according to data
                                          ;  sizes
FFT_INPUT_IS_BITREVERSED          equ   4 ;Default to normal (linear)
                                          ;  input
FFT_OUTPUT_IS_BITREVERSED         equ   8 ;Default to normal (linear)


    DEFINE    old_omr        'X:(SP-1)'   ;Scratch to store old omr reg.
    DEFINE    ScaleBF        'X:(SP-2)'   ;Counter to count the number of
                                          ;stages wherein scaling is done
                                          ;in BFP method.
    DEFINE    pZ             'X:(SP-4)'   ;Scratch to store pointer to output buffer

;-----------------------------------
;Save c and d
;-----------------------------------

    adda    #2,sp
    move.l  c2,x:(sp)+
    move.l  c10,x:(sp)+
    move.l  d2,x:(sp)+
    move.l  d10,x:(sp)+
    move.l  r5,x:(sp)

;---------------------------------
; Accommodate for scratch in stack
;---------------------------------

    adda    #Scratch_Size,sp

;----------------------------------------------------------
; Store the existing omr and set the saturation bit to 0
;----------------------------------------------------------

    move.w  omr,old_omr                   ;Store old omr.
    tfra    r2,r5
    bfclr   #$30,omr                      ;S-bit = 0.
                                          ;Rounding mode enabled
                                          ;(2's complement)

;---------------------------------------------------------------
; Rearrange the input array in bit-reversed order, if required
;---------------------------------------------------------------

;    move.w  r4,pZ                         ;Store pointer to the output array in r4
    StoreRx  r4,pZ                         ;Store pointer to the output array in r4
    tfra    r3,r1                         ;Store r3 in r1
    brclr   #FFT_INPUT_IS_BITREVERSED,x:(r2+Offset_options),FFT_Computation
                                          ;Test if input is bit-reversed.

    ;--------------------------------------------------------
    ; Set up the parameters for calling bit-reverse function
    ;--------------------------------------------------------

    move.w  x:(r2+Offset_n),y0            ;y0 = N, the size of FFT.
    tfra    r3,r2                         ;r2 -> pX
    tfra    r4,r3
    tfra    r3,r1                         ;Store r3 in r1
    jsr     Fdfr16Cbitrev_                ;Call bit reverse function.
                                          ;Result is returned in y0.
    cmp.w   #FAIL,y0
    jne     FFT_Computation               ;Bit reverse passed, go to FFT.
    moveu.w old_omr,omr                   ;Restore previous OMR value.
    moveu.w #-Scratch_Size,n              ;Restore original stack pointer.
    adda    n,sp
    move.l  x:(sp)-,r5
    move.l  x:(sp)-,d
    move.l  x:(sp)-,d2
    move.l  x:(sp)-,c
    move.l  x:(sp)-,c2
    rts                                   ;FAIL is returned in y0.

FFT_Computation

;-----------------------------------------------
; Decide whether scaling is AS or BFP
;-----------------------------------------------

    tfra    r5,r2
    brset   #FFT_SCALE_RESULTS_BY_N,x:(r5+Offset_options),AutoScaling
                                          ;Check the type of scaling
                                          ;Autoscaling...
    move.w  x:(r5+Offset_options),x0      ;Check if scaling type is BFP
    andc    #FFT_SCALE_RESULTS_BY_DATA_SIZE,x0
    tst.w   x0
    jne     BlockFloatingPoint            ;Block floating point
    jmp     NoScaling                     ;If not, it implies no scaling.

AutoScaling

;----------------------------
; CASE 1: Code for AS.
;----------------------------


;    moveu.w pZ,r4                         ;get pointer to output buffer in r4
    LoadRx  pZ,r4
    moveu.w x:(r5+Offset_n),n             ;n = No. of points in FFT
    tfra    r4,r2                         ;r2 -> pointer to output buffer
    tfra    r1,r3                         ;r3 -> 1st Ar of first pass
    adda    n,r3                          ;r3 -> 1st Br of 1st pass
    tfra    r2,r0                         ;r0 -> 1st Br of first pass
    move.w  x:(r1)+,a                     ;Get 1st Ar of 1st pass
    adda    n,r0
    deca    r0                            ;Adjust r0 so that in first
    asra    n                             ;first pass , which is half
                                          ;half the no. of DFT points
    move.w  x:(r0),b                      ;Save the memory cotents so
                                          ;that first parallel move
                                          ;doesn't corrupt the data
    move.w  x:(r3)+,y0                    ;Get 1st Br of 1st pass

    if CODEWARRIOR_WORKAROUND==1
    do      n,>>_first_pass               ;The first pass has to be
                                          ;repeated length/2 times
    else
    do      n,_first_pass                 ;The first pass has to be
                                          ;repeated length/2 times
    endif

    add     y0,a    b,x:(r0)+             ;Find Cr,save Di in previous
                                          ;Bi. r0 -> Br
    asr     a       x:(r1)+,b             ;Find Cr/2,get Ai,r1 -> next Ar
    rnd     a       x:(r3)+,x0            ;round, get Bi, r3 -> next Br
    sub     y0,a    a,x:(r2)+             ;Find Dr/2 ,save Cr/2,r2 -> Ci
    add     x0,b    a,x:(r0)+             ;Find Ci,save Dr/2 ,r0 ->Di
    asr     b       x:(r1)+,a             ;Find Ci/2, get next Ar,
                                          ;r1 -> next Ai
    rnd     b       x:(r3)+,y0            ;round Ci/2, get next Br,
                                          ;r3 -> next Bi
    sub     x0,b    b,x:(r2)+             ;Find Di/2, save Ci/2 ,
                                          ;r2 -> next Cr. Di/2 saved
                                          ;in the next loop
_first_pass
    move.w  b,x:(r0)                      ;Save last Di/2 of the 1st pass
    tfra    r4,r1                         ;r1 -> 1st Ar of 2nd pass
    tfra    r1,r3                         ;r3 -> 1st Ar of 2nd pass
    moveu.w #2,n3                         ;r2 set to 2,used for controlling
                                          ;the no. of groups in the pass
    adda    n,r3                          ;r3 -> first Br of second pass

    move.w  x:(r5+Offset_No_of_Stages),c  ;Set counter for no. of passes
    sub.w   #2,c


_second_pass

    LoadRx  x:(r5+Offset_Twiddle),r0      ;r0 ->mem. location of the first
                                          ;twiddle fac. ,twiddle fac.
                                          ;stored in bit reversed fashion
    asra    n                             ;n =n/2
    move.w  n3,b                          ;Save the no. of groups/passin b
    tfra    r3,r2                         ;r2 -> first Br of the first pass

    if CODEWARRIOR_WORKAROUND==1
    do      b,>>_end_group                ;Middle loop is done b times
                                          ;b=2**(pass number-1)
                                          ;b = no. of groups/pass
    else
    do      b,_end_group                  ;Middle loop is done b times
                                          ;b=2**(pass number-1)
                                          ;b = no. of groups/pass
    endif

    move.w  x:(r0)+,y0                    ;y0=Wr, r0 ->Wi
    move.w  x:(r0)+,b                     ;b=Wi, r0 -> next Wr
                                          ;(in bit reversed order)
    neg     b            x:(r3)+,x0       ;x0=Br, r3 ->Bi
    move.w  b,y1                          ;y1 = -Wi
;    move.w  r0,d                          ;Save twiddle factor pointer
    StoreRx r0,d                          ;Save twiddle factor pointer
    tfra    r1,r0                         ;Move r1 to r0
    deca    r2

    move.w  x:(r2),b                      ;Save the contents so that the
                                          ;mem. contents aren't corrupted
                                          ;in the first middle loop

    if CODEWARRIOR_WORKAROUND==1
    do      n,>>_end_bfly                 ;Inner loop is done n times
                                          ;n=2**(L-passnumber)
                                          ;n=no. of butterflies/group
    else
    do      n,_end_bfly                   ;Inner loop is done n times
                                          ;n=2**(L-passnumber)
                                          ;n=no. of butterflies/group
    endif

    mpy     y0,x0,b      b,x:(r2)+        ;b=WrBr,store the previous
                                          ;butterfly's Di,r1->current Ar
    mpy     y1,x0,a      x:(r3)+,x0       ;a=WiBr,get Bi
    mac     -y0,x0,a                      ;a=-WrBi+WiBr
    mac     y1,x0,b      x:(r1)+,x0       ;b=WrBr+WiBi,x0=Ar,r1 -> Ai
    add     x0,b                          ;Find Cr in b
    asr     b                             ;b=Cr/2,Scale down for storage
    rnd     b                             ;Round.The rounding done here
                                          ;closely matches with the
                                          ;autoscale mode rounding
    sub     b,x0                          ;Find Dr/2
    neg     a            b,x:(r0)+        ;a= WrBi-WiBr
                                          ;Store Cr/2
    move.w  x:(r1)+,b                     ;fetch Ai into b,r1 -> next Ar
    add     b,a          x0,x:(r2)+       ;Find Ci ,store Dr/2,r2 -> Di
    asr     a            x:(r3)+,x0       ;a=Ci/2,x0 = Next Br,
                                          ;r3 -> Next Bi
    rnd     a                             ;Round
    sub     a,b          a,x:(r0)+        ;b = Di/2, a = Ci/2
                                          ;store Ci/2
                                          ;Di/2 stored in next loop
_end_bfly


    move.w  b,x:(r2)+                     ;Store last butterfly's Ci
    moveu.w  d,r0                         ;Restore the pointer pointing
                                          ;to the twiddle factors
    tfra    r2,r1                         ;r1 -> next group's first Ar
    adda.l    n,r2                        ;r2 -> next group's first Br
    tfra    r2,r3                         ;r3 -> next group's first Br
_end_group

    tfra    r4,r1                         ;r1 ->1st Ar,at start of each pass
    tfra    r1,r3                         ;r3 ->1st Ar,at start of each pass
    move.w  n3,b                          ;double the no of groups for next
    asl     b            x:(r3)+n,x0      ;pass,Dummy read  to adjust r3
                                          ;r3 -> first Br of the next pass
    moveu.w b,n3
    dec.w   c                             ;Test the pass counter for Zero
    tst.w   c
    bne     _second_pass                  ;If less than zero then go to
                                          ;last pass .
    LoadRx  x:(r5+Offset_Twiddle),r0      ;Get address of twiddle factors
    move.w  x:(r0)+,y0                    ;y0=Wr,r0 -> Wi

    if CODEWARRIOR_WORKAROUND==1
    do      b,>>_last_pass                ;N/2 groups in last pass
    else
    do      b,_last_pass                  ;N/2 groups in last pass
    endif

    tfra    r3,r2

    move.w  x:(r3)+,x0                    ;x0=Br,r3 -> Bi
    mpy     x0,y0,b      x:(r0)+,y1       ;b=BrWr, y1=Wi, r0 ->Next Wr
    mpy     -y1,x0,a                      ;a=WiBr
    move.w  x:(r3)+,x0
    mac     -y0,x0,a                      ;a=-WrBi+WiBr
    mac     -y1,x0,b                      ;b=WiBi+WrBr, x0=Ar, r1 -> Ai
    move.w  x:(r1)+,x0
    add     x0,b                          ;Find Cr
    asr     b                             ;Find Cr/2
    rnd     b                             ;Round
    sub     b,x0                          ;Find Dr/2
    move.w  b,x:(r1-1)                    ;Store Cr/2 , r1 -> Ai
    move.w  x:(r1)+,b                     ;b = Ai, r1 -> Ai
    sub     b,a         x0,x:(r2)+        ;Find -Ci , store Dr/2,r2 ->Di
    asr     a           x:(r3)+n,x0       ;Find -Ci/2, Dummy read to
                                          ;adjust r3 to next Br
    rnd     a           x:(r0)+,y0
    add     a,b         x:(r1)+n,x0       ;Find Di/2,Dummy read to
                                          ;adjust r1
    neg     a           b,x:(r2)+         ;Find Ci/2 ,store Di/2
    move.w  a,x:(r1-3)                    ;Store Ci/2
_last_pass
_End_fft

    jmp     PostProcessing


;---------------------------------------------------------------
; CASE 2: Code for BFP
;---------------------------------------------------------------

BlockFloatingPoint

;---------------------------------------------------------------
; See whether the magnitude of any of the data values is > 0.25
;---------------------------------------------------------------

;    moveu.w pZ,r4                         ;Copy pointer to output buffer in r4
    LoadRx  pZ,r4                         ;Copy pointer to output buffer in r4
    moveu.w x:(r5+Offset_n),n             ;n = No.of points in FFT
    tfra    r4,r2                         ;r2 -> output buffer
    asla    n
    cmpa    r1,r2
    beq     _avoid_copy_bfp

    if CODEWARRIOR_WORKAROUND==1
    do      n,>>_endcpy
    else
    do      n,_endcpy
    endif

    move.w  x:(r1)+,y0
    move.w  y0,x:(r2)+
_endcpy
_avoid_copy_bfp
    asra    n
    tfra    r4,r1
    tfra    r1,r3                         ;r3 -> 1st Ar of 1st pass


    dec.w   x:(r5+Offset_No_of_Stages)

    move.w #1,c                           ;c set to 1, for controlling
                                          ;the no. of groups in the pass
    adda    n,r3                          ;r3 -> first Br of first pass
    move.w  #0,ScaleBF                    ;Memory location used as a flag
                                          ;0 indicates last pass is not
                                          ;to be scaled down
    moveu.w n,n3                          ;Store no. of butterflies in n3
_first_passBF
    move.w  n3,b
    tfra    r1,r0                         ;r0 -> first Br
    move.w  #0,x0                         ;x0= 0; smallest positive number
    move.w  x:(r0)+,a                     ;a= first Ar, r0 -> First Ai

    if CODEWARRIOR_WORKAROUND==1
    do      b,>>_chkp                     ;Repeat for all samples
    else
    do      b,_chkp                       ;Repeat for all samples
    endif

    abs     a      x:(r0)+,b              ;Get the magnitude in acc a
                                          ;get the next data in acc b
    or.w    a,x0                          ;Get the most significant bit of
                                          ;the largest number in x0
    abs     b      x:(r0)+,a              ;Get the magnitude of next data
                                          ;in b and get next data in a
    or.w      b,x0                        ;Find the max value in x0
_chkp
    LoadRx  x:(r5+Offset_Twiddle),r0      ;r0 ->memory location of the
                                          ;first twiddle factor Wr
    asra     n
    tfra    r3,r2                         ;r2 -> first Br of the pass
    cmp.w   #$2000,x0                     ;Compare the max value with 0.25
    ble     _nscdown                      ;If greater, perform the sc_down
                                          ;pass, else no scale down pass
    inc.w    ScaleBF                      ;Increment the scale_fac by one

    if CODEWARRIOR_WORKAROUND==1
    do      c1,>>_end_groupBF             ;Middle loop is done b times
                                          ;b=2**(pass number-1)
                                          ;b = no. of groups/pass
    else
    do      c1,_end_groupBF               ;Middle loop is done b times
                                          ;b=2**(pass number-1)
                                          ;b = no. of groups/pass
    endif

    move.w  x:(r0)+,y0                    ;y0=Wr, r0 -> Wi
    move.w  x:(r0)+,b                     ;b=Wi, r0 -> next Wr
                                          ;(in bit reversed order)
    neg     b            x:(r3)+,x0       ;x0=Br, r3 -> Bi
    move.w  b,y1                          ;y1 = -Wi
;    move.w  r0,d                          ;Save twiddle factor
    StoreRx r0,d                          ;Save twiddle factor
    tfra    r1,r0                         ;Move r1 to r0
    deca    r2
    move.w  x:(r2),b                      ;Save the contents so that they
                                          ;are not corrupted in the first
                                          ;middle loop
    if CODEWARRIOR_WORKAROUND==1
    do      n,>>_end_bflyBF               ;Inner loop is done n times
                                          ;n=2**(L-passnumber)
                                          ;n=no. of butterflies/group
    else
    do      n,_end_bflyBF                 ;Inner loop is done n times
                                          ;n=2**(L-passnumber)
                                          ;n=no. of butterflies/group
    endif

    mpy     y0,x0,b      b,x:(r2)+        ;b=WrBr, store previous butter-
                                          ;fly's Di , r1 -> current Ar
    mpy     y1,x0,a      x:(r3)+,x0       ;get Bi
    mac     -y0,x0,a                      ;a=-WrBi+WiBr
    mac     y1,x0,b      x:(r1)+,x0       ;x0=Ar,r1 -> Ai
    add     x0,b                          ;Find Cr in b
    asr     b                             ;b=Cr/2, Scale down for storage
    rnd     b                             ;Round. The rounding done here
                                          ;closely matches with the
                                          ;autoscale down mode rounding
    sub     b,x0                          ;Find Dr/2
    neg     a            b,x:(r0)+        ;a = WrBi - WiBr
                                          ;  store Cr/2
    move.w  x:(r1)+,b                     ;Fetch Ai into b, r1-> next Ar
    add     b,a          x0,x:(r2)+       ;Find Ci, store Dr/2, r2-> Di
    asr     a            x:(r3)+,x0       ;a=Ci/2, x0= Next Br,
                                          ;r3-> Next Bi
    rnd     a                             ;Round
    sub     a,b          a,x:(r0)+        ;b=Di/2, a=Ci/2, store Ci/2
                                          ;Di/2 is stored in next loop
_end_bflyBF
    move.w  b,x:(r2)+                     ;Store last butterfly's Ci
    moveu.w  d,r0                         ;Restore the pointer pointing
                                          ;to the twiddle factors
    tfra    r2,r1                         ;r1 -> next group's first Ar
    adda.l  n,r2                          ;r2 -> next group's first Br
    tfra    r2,r3                         ;r3 -> next group's first Br
_end_groupBF

    tfra    r4,r3                         ;r3 -> 1st Ar, for start of each
                                          ;pass

    tfra    r3,r1                         ;r1 -> 1st Ar,for start of each
                                          ;pass
    asl      c                            ;Double the no of groups for next
                                          ;pass ,Dummy read into x0
    adda    n,r3                          ;r3-> first Br of the next pass
    dec.w   x:(r5+Offset_No_of_Stages)    ;Test for pass counter
                                          ;r2=2**(pass number-1)
    jne     _first_passBF                 ;perform the loop till counter= 0
    bra     _lpass
_nscdown                                  ;No scale down mode
    move.w  n,d
    moveu.w #0,n                          ;Save the number of buttrflies

    if CODEWARRIOR_WORKAROUND==1
    do      c1,>>_end_group2              ;Middle loop is done c1 times
                                          ;r2=2**(pass number-1)
    else
    do      c1,_end_group2                ;Middle loop is done c1 times
                                          ;r2=2**(pass number-1)
    endif

    move.w  x:(r0)+,y0
    move.w  x:(r3)+,x0                    ;y0=Wr, x0=first Br of the
                                          ;group. r0->Next Wr. r3->first
                                          ;Bi of the group.

    if CODEWARRIOR_WORKAROUND==1
    do      d1,>>_end_bfly2               ;Inner loop is done n times
                                          ;n=2**(L-passnumber)
    else
    do      d1,_end_bfly2                 ;Inner loop is done n times
                                          ;n=2**(L-passnumber)
    endif

    mpy     x0,y0,b      x:(r0)+n,y1      ;y1=Wi, b=WrBr, x0=Bi, r3->Br
                                          ;r0->Wi
    move.w  x:(r3)-,x0
    macr    -y1,x0,b                      ;b=WrBr+WiBi,
    move.w  x:(r1)+n,a                    ;a=Ar, r1->Ar
    add     a,b                           ;Find Cr in b, b=Ar+WrBr+WiBi,
    asl     a            b,x:(r1)+        ;a=2Ar, store Cr. r1->Ai
    sub     b,a          x:(r3)+,b        ;Find Dr in a, b=Br, r3->Bi
    mpy     b1,y1,b      a,x:(r2)+
    move.w  x:(r3)+,x0                    ;b=-Brwi, x0=Bi, r3->Next Br
    macr    y0,x0,b      x:(r1)+n,a       ;b=WrBi-WiBr, a=Ai, r1->Ai
    add     a,b          x:(r3)+,x0       ;Find Ci in b, x0=Next Br,
                                          ;r3->Next Bi
    asl     a            b,x:(r1)+        ;a=2Ai, store Ci, r1->Next Ar
    sub     b,a                           ;Find Di
    move.w  a,x:(r2)+                     ;Store Di, r2->Next Br
_end_bfly2
    adda    #1,r0                         ;r0->First Wr of next group
    moveu.w  d,r3                         ;Restore the pointer pointing
    tfra    r2,r1                         ;r1->next group,s first Ar
    adda.l  r3,r2                         ;r2 -> next group's first Br
    tfra    r2,r3                         ;r3-> Next groups first Br
_end_group2

    moveu.w d1,n
    tfra    r4,r3                         ;r3 -> 1st Ar, for start of each
    tfra    r3,r1                         ;r1 -> 1st Ar of next pass
    asl     c                             ;Double the no of groups for next
                                          ;pass ,Dummy read into x0 to
    adda    n,r3                          ;adjust r3, r3 -> first Br of
                                          ;the next pass
    dec.w   x:(r5+Offset_No_of_Stages)    ;Test the pass counter for zero

    bne     >_first_passBF                ;If not equal to zero then go to
                                          ;beginning of the pass
_lpass                                    ;data scanning for the last pass

    tfra    r4,r0                         ;r0->first Ar
    move.w  x:(r5+Offset_n),y0            ;y0= number of points in fft
    move.w  #0,x0                         ;x0= 0; smallest positive number
    move.w  x:(r0)+,a                     ;a= first Ar, r0 -> First Ai

    if CODEWARRIOR_WORKAROUND==1
    do      y0,>>_chklp                   ;Repeat for all samples
    else
    do      y0,_chklp                     ;Repeat for all samples
    endif

    abs     a      x:(r0)+,b              ;Get the magnitude in acc a
                                          ;get the next data in acc b
    or.w    a,x0                          ;Get the most significant bit of
                                          ;the largest number in x0
    abs     b      x:(r0)+,a              ;Get the magnitude of next data
                                          ;in b and get next data in a
    or.w    b,x0                          ;Find the max value in x0
_chklp
    LoadRx  x:(r5+Offset_Twiddle),r0      ;r0->first Wr
    cmp.w   #0.25,x0                      ;if the largest value is > 0.25
    ble     _lnscdown                     ;perform scale down pass
    inc.w   ScaleBF                       ;Increment the scale factor
    move.w  x:(r0)+,y0

    if CODEWARRIOR_WORKAROUND==1
    do      c1,>>_last_pass1              ;N/2 groups in last pass
    else
    do      c1,_last_pass1                ;N/2 groups in last pass
    endif

    tfra    r3,r2
    move.w  x:(r3)+,x0                    ;y0=Wr, x0=Br, r0 -> Wi, r3 -> Bi
    mpy     x0,y0,b      x:(r0)+,y1       ;b=BrWr, y1=Wi, r0 -> Next Wr
    mpy     -y1,x0,a                      ;a=WiBr
    move.w  x:(r3)+,x0
    mac     -y0,x0,a                      ;a=-WrBi+WiBr
    mac     -y1,x0,b                      ;b=WiBi+WrBr, x0=Ar, r1 -> Ai
    move.w  x:(r1)+,x0
    add     x0,b                          ;Find Cr
    asr     b                             ;Find Cr/2
    rnd     b                             ;Round
    sub     b,x0                          ;Find Dr/2
    move.w  b,x:(r1-1)                    ;Store Cr/2 , r1 -> Ai
    move.w  x:(r1)+,b                     ;b = Ai, r1 -> Ai
    sub     b,a         x0,x:(r2)+        ;Find -Ci , store Dr/2,r2 ->Di
    asr     a           x:(r3)+n,x0       ;Find -Ci/2, Dummy read to adjust
                                          ;r3 to next Br
    rnd     a           x:(r0)+,y0        ;Round,
                                          ;r1 -> Next Ar
    add     a,b         x:(r1)+n,x0       ;Find Di/2
    neg     a           b,x:(r2)+         ;Find Ci/2 ,store Di/2
    move.w  a,x:(r1-3)                    ;Store Ci/2
_last_pass1
    jmp     PostProcessing                ;Perform bitreversal operation
_lnscdown                                 ;Last pass no scale down
                                          ;Offset for the last pass
    move.w  x:(r0)+,y0                    ;y0=Wr,r0->Wi
    moveu.w #0,n

    if CODEWARRIOR_WORKAROUND==1
    do      c1,>>_last_pass2              ;N/2 number of butterflies
    else
    do      c1,_last_pass2                ;N/2 number of butterflies
    endif

    move.w  x:(r3)+,x0                    ;x0=Br, r3->Bi
    mpy     x0,y0,b      x:(r0)+,y1       ;y1=Wi
    move.w  x:(r3)-,x0                    ;b=WrBr, y1=Wi, x0=Bi, r0->Wi
                                          ;r3->Br
    macr    -y1,x0,b
    move.w  x:(r1)+n,a                    ;b=WrBr+WiBi, a=Ar, r1->Ar
    add     a,b                           ;Find Cr in b, b=Ar+WrBr+WiBi,
    asl     a            b,x:(r1)+        ;a=2Ar, store Cr, r1->Ai
    sub     b,a          x:(r3)+n,b       ;Find Dr in a, a=2Ar-Cr, b=Br
                                          ;r3->Br
    mpy     b1,y1,b      a,x:(r3)+        ;b=-Br, store Dr, r3->Bi
    move.w  x:(r3)+n,x0                   ;b=-BrWi, x0=Bi, r3->Bi
    macr    y0,x0,b      x:(r1)+n,a       ;b=WrBi-BrWi, a=Ai, r1->Ai
    add     a,b          x:(r0)+,y0       ;find Ci in acc b
    asl     a            b,x:(r1)+        ;a=2Ai, store Ci, r1->Br
    sub     b,a          x:(r1)+,x0       ;Find Di in acc a, dummy read
                                          ;in x0, r1->Bi
    move.w  a,x:(r3)+                     ;Store Di, r3-> Ar of next
                                          ;butterfly
    adda    #1,r1
    adda    #2,r3
_last_pass2
_rev
    jmp     PostProcessing


NoScaling

;---------------------------------------------------------------
; CASE 3: Code for no-scaling.
;---------------------------------------------------------------


;    moveu.w pZ,r4                         ;r4 pointer to output buffer
    LoadRx  pZ,r4                         ;r4 pointer to output buffer
    moveu.w x:(r5+Offset_n),n             ;n = No. of points in FFT
    tfra    r4,r2                         ;r2 pointer to output buffer
    tfra    r1,r3                         ;r3 -> 1st Ar of first pass
    adda    n,r3                          ;r3 -> 1st Br of 1st pass
    tfra    r2,r0                         ;r0 -> 1st Br of first pass
    move.w  x:(r1)+,a                     ;Get 1st Ar of 1st pass
    adda    n,r0
    deca    r0                            ;Adjust r0 so that in first
                                          ;first pass , which is half
    asra    n                             ;half the no. of DFT points
    move.w  x:(r0),b                      ;Save the memory cotents so
                                          ;that first parallel move
                                          ;doesn't corrupt the data
    move.w  x:(r3)+,y0                    ;Get 1st Br of 1st pass

    if CODEWARRIOR_WORKAROUND==1
    do      n,>>_first_pass_NS            ;The first pass has to be
                                          ;repeated length/2 times
    else
    do      n,_first_pass_NS              ;The first pass has to be
                                          ;repeated length/2 times
    endif

    add     y0,a    b,x:(r0)+             ;Find Cr,save Di in previous
    rnd     a       x:(r3)+,x0            ;round, get Bi, r3 -> next Br
    sub     y0,a    a,x:(r2)+             ;save Cr,r2 -> Ci
    sub     y0,a    x:(r1)+,b             ;Find Dr,get Ai,r1 -> next Ar
    add     x0,b    a,x:(r0)+             ;Find Ci,save Dr ,r0 ->Di
    rnd     b       x:(r3)+,y0            ;round Ci, get next Br,
                                          ;r3 -> next Bi
    sub     x0,b    b,x:(r2)+             ;Find Di, save Ci,
                                          ;r2 -> next Cr.
    sub     x0,b    x:(r1)+,a             ;Find Di, get next Ar,
                                          ;in the next loop
_first_pass_NS
    move.w  b,x:(r0)                      ;Save last Di/2 of the 1st pass
    tfra    r4,r1                         ;r1 -> 1st Ar of 2nd pass
    tfra    r1,r3                         ;r3 -> 1st Ar of 2nd pass
    moveu.w #2,n3                         ;n3 set to 2,used for controlling
                                          ;the no. of groups in the pass
    adda    n,r3                          ;r3 -> first Br of second pass
    move.w  x:(r5+Offset_No_of_Stages),c  ;Set counter for no. of passes
    sub.w   #2,c

_second_pass_NS

    LoadRx  x:(r5+Offset_Twiddle),r0     ;restore pointer to twiddle factor
                                          ;stored in bit reversed fashion
    asra    n                             ;n =n/2
    move.w  n3,b                          ;Save the no. of groups/passin b
    tfra    r3,r2                         ;r2 -> first Br of the first pass

    if CODEWARRIOR_WORKAROUND==1
    do      b,>>_end_group_NS             ;Middle loop is done b times
                                          ;b=2**(pass number-1)
                                          ;b = no. of groups/pass
    else
    do      b,_end_group_NS               ;Middle loop is done b times
                                          ;b=2**(pass number-1)
                                          ;b = no. of groups/pass
    endif

    move.w  x:(r0)+,y0                    ;y0=Wr,r0 -> Wi
    move.w  x:(r0)+,b                     ;b=Wi, r0 -> next Wr
                                          ;(in bit reversed order)
    neg     b            x:(r3)+,x0       ;x0=Br, r3 ->Bi
    move.w  b,y1                          ;y1 = -Wi.
;    move.w  r0,d                          ;Save twiddle factor pointer
    StoreRx r0,d                          ;Save twiddle factor pointer
    tfra    r1,r0                         ;Move r1 to r0
    deca    r2
    move.w  x:(r2),b                      ;Save the contents so that the
                                          ;mem. contents aren't corrupted
                                          ;in the first middle loop
    if CODEWARRIOR_WORKAROUND==1
    do      n,>>_end_bfly_NS              ;Inner loop is done n times
                                          ;n=2**(L-passnumber)
                                          ;n=no. of butterflies/group
    else
    do      n,_end_bfly_NS                ;Inner loop is done n times
                                          ;n=2**(L-passnumber)
                                          ;n=no. of butterflies/group
    endif

    mpy     y0,x0,b      b,x:(r2)+        ;b=WrBr,store the previous
                                          ;butterfly's Di,r1->current Ar
    mpy     y1,x0,a      x:(r3)+,x0       ;a=-WiBr, get Bi
    mac     -y0,x0,a                      ;a=-WrBi+WiBr
    mac     y1,x0,b      x:(r1)+,x0       ;b=WrBr-WiBi, x0=Ar,r1 -> Ai
    add     x0,b                          ;Find Cr in b
    rnd     b                             ;Round.The rounding done here
                                          ;closely matches with the
                                          ;autoscale mode rounding
    asl     x0                            ;Get 2*Ar
    sub   b,x0                            ;Find Dr [ Dr = 2*Ar - Cr]
    neg     a            b,x:(r0)+        ;a= WrBi-WiBr
                                          ;Store Cr
    move.w  x:(r1)+,b                     ;fetch Ai into b,r1 -> next Ar
    add     b,a          x0,x:(r2)+       ;Find Ci ,store Dr,r2 -> Di
                                          ;r3 -> Next Bi
    rnd     a            x:(r3)+,x0       ;Round
    asl     b                             ;Get 2*Ai
    sub     a,b          a,x:(r0)+        ;Find Di,[Di = 2*Ai - Ci]
                                          ;store Ci
                                          ;Di stored in next loop
_end_bfly_NS

    move.w  b,x:(r2)+                     ;Store last butterfly's Ci
    moveu.w  d,r0                         ;Restore the pointer pointing
                                          ;to the twiddle factors
    tfra    r2,r1                         ;r1->next group's first Ar   lea     (r2)+n
    adda    n,r2                          ;r2 -> next group's first Br
    tfra    r2,r3                         ;r3 -> next group's first Br
_end_group_NS

    tfra    r4,r1
    tfra    r1,r3                         ;r3 ->1st Ar,at start of each pass
    move.w  n3,b                          ;double the no of groups for next
    asl     b            x:(r3)+n,x0      ;pass,Dummy read  to adjust r3
                                          ;r3 -> first Br of the next pass
    moveu.w b,n3
    dec.w   c                             ;Test the pass counter for Zero
    tst.w   c
    bne     _second_pass_NS               ;If less than zero then go to
                                          ;last pass .
    LoadRx  x:(r5+Offset_Twiddle),r0      ;Get address of twiddle factors
    move.w  x:(r0)+,y0                    ;y0=Wr,r0 -> Wi

    if CODEWARRIOR_WORKAROUND==1
    do      b,>>_last_pass_NS             ;N/2 groups in last pass
    else
    do      b,_last_pass_NS               ;N/2 groups in last pass
    endif

    tfra    r3,r2
    move.w  x:(r3)+,x0                    ;x0=Br,r3 -> Bi
    mpy     x0,y0,b      x:(r0)+,y1       ;b=BrWr, y1=Wi, r0 ->Next Wr
    mpy     -y1,x0,a                      ;a=WiBr
    move.w  x:(r3)+,x0
    mac     -y0,x0,a                      ;a=-WrBi+WiBr
    mac     -y1,x0,b                      ;b=-WiBi+WrBr,
    move.w  x:(r1)+,x0                    ;x0=Ar, r1 -> Ai
    add     x0,b                          ;Find Cr
    rnd     b                             ;Round
    asl     x0                            ;Get 2*Ar
    sub     b,x0                          ;Find Dr [Dr = 2*Ar - Cr]
    move.w  b,x:(r1-1)                    ;Store Cr , r1 -> Ai
    move.w  x:(r1)+,b                     ;b = Ai, r1 -> Ai
    sub     b,a         x0,x:(r2)+        ;Find -Ci , store Dr,r2 ->Di
    rnd     a           x:(r3)+n,x0       ;Find -round(Ci), Dummy read to
                                          ;adjust r3 to next Br
    asl     b           x:(r0)+,y0        ;Get 2*Ai
    add     a,b         x:(r1)+n,x0       ;Find Di,Dummy read to
                                          ;adjust r1
    neg     a           b,x:(r2)+         ;Find Ci ,store Di
    move.w  a,x:(r1-3)                    ;Store Ci
_last_pass_NS
_End_fft_NS

PostProcessing

;--------------------------------------------------------------
; Rearrange the output array in bit-reversed order, if required
;--------------------------------------------------------------

    tfra    r5,r2
    brset   #FFT_OUTPUT_IS_BITREVERSED,x:(r2+Offset_options),no_bitrev_output

                                          ;Test if output is bit-reversed.
                                          ;No bit reverse required.


    ;--------------------------------------------------------
    ; Set up the parameters for calling bit-reverse function
    ;--------------------------------------------------------

    move.w  x:(r2+Offset_n),y0            ;y0 = N, the size of FFT.
    tfra    r4,r2
    tfra    r2,r3                         ;Output pointer
    jsr     Fdfr16Cbitrev_                ;Call bit reverse function.
                                          ;Result is returned in y0.
    cmp.w   #FAIL,y0
    jne     no_bitrev_output              ;Bit reverse passed, go to FFT.
    moveu.w old_omr,omr                   ;Restore previous OMR value.
    adda    #-Scratch_Size,sp             ;Restore original stack pointer.
    move.l  x:(sp)-,r5
    move.l  x:(sp)-,d
    move.l  x:(sp)-,d2
    move.l  x:(sp)-,c
    move.l  x:(sp)-,c2
    rts                                   ;FAIL is returned in y0.

no_bitrev_output

;----------------------------------------------------------------
; Return the amount of scaling done in terms of number of shifts
;----------------------------------------------------------------


    tfra    r5,r2
    move.w  ScaleBF,y1
    moveu.w old_omr,omr                   ;Restore previous OMR value.
    adda    #-Scratch_Size,sp             ;Restore Stack pointer.
    move.w  x:(r2+Offset_options),x0
    brclr   #FFT_SCALE_RESULTS_BY_N,x0,_return_BFP
                                          ;Return amount of scaling done
                                          ;in y0, for AS.
    move.w  x:(r2+Offset_No_of_Stages),y0
    move.l  x:(sp)-,r5
    move.l  x:(sp)-,d
    move.l  x:(sp)-,d2
    move.l  x:(sp)-,c
    move.l  x:(sp)-,c2
    rts

_return_BFP

    brclr   #FFT_SCALE_RESULTS_BY_DATA_SIZE,x0,return_NS
    move.w  y1,y0                         ;Return amount of scaling done in
                                          ;BFP method.
    move.l  x:(sp)-,r5
    move.l  x:(sp)-,d
    move.l  x:(sp)-,d2
    move.l  x:(sp)-,c
    move.l  x:(sp)-,c2
    rts

return_NS

    move.w  #0,y0
    move.l  x:(sp)-,r5
    move.l  x:(sp)-,d
    move.l  x:(sp)-,d2
    move.l  x:(sp)-,c
    move.l  x:(sp)-,c2                    ;No scaling is done, return 0.
    rts

    ENDSEC
