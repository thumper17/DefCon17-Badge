;********************************************************************
;
; (c) Freescale Semiconductor
; (c) Copyright 2000, 2001 Freescale, Inc.
;
;
;********************************************************************
;
; File Name:    dfr16rfft.asm
;
; Description:  Assembly module for Real FFT
;
; Modules
;    Included:  Fdfr16RFFT
;
; Author(s):    Sandeep S (Optimized by Meera S P)
;
; Date:         22 Feb 2000
;
;********************************************************************

    SECTION  rtlib

    include  "portasm.h"

    GLOBAL   Fdfr16RFFT

    org      p:

;********************************************************************
;
; Module Name:  Fdfr16RFFT
;
; Description:  Computes real FFT for the given input block.
;
; Functions
;      Called:  Fdfr16Cbitrev - Complex bit reverse function.
;
; Calling
; Requirements: 1. r2 -> Pointer to RFFT Handle.
;               2. r3 -> Pointer to input buffer whose RFFT is to be
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
; DO loops:     20; max. nesting depth = 2.
;
; REP loops:    None.
;
; Environment:  MetroWerks on PC.
;
; Special
;     Issues:   FFT with No Scale option gives correct results only for
;               low energy signals.
;
;******************************Change History************************
;
;    DD/MM/YY     Code Ver     Description      Author(s)
;    --------     --------     -----------      ------
;    22/02/2000   0.1          Module created   Sandeep S
;    09/03/2000   1.0          Reviewed &
;                              Baselined        Sandeep S
;    12/03/2000   1.1          Optimized the
;                              Post-Processing
;                              part             Sandeep S
;    29/01/2001   1.2          Optimized,
;                              Reviewed &
;                              Baselined        Meera S P
;    27/04/2001   2.0          Modified for V2  Jeff Ruhge
;    11/12/2001   2.1          Optimised for    Alwin Anbu D
;                              V2
;********************************************************************


Fdfr16RFFT

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



   DEFINE    old_omr        'X:(SP-1)'      ; Scratch to store old omr reg.
   DEFINE    ScaleBF        'X:(SP-2)'      ; Counter to count the number of
                                            ;  stages wherein scaling is done
                                            ;  in BFP method.
   DEFINE    tmp            'X:(SP-3)'      ; Temporary variable


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
    tfra      r2,r5
    bfclr     #$30,omr                      ; S-bit = 0,Convergent
                                            ;  rounding.

FFT_Computation

;-----------------------------------------------
; Decide whether scaling is AS or BFP
;-----------------------------------------------

    brset     #FFT_SCALE_RESULTS_BY_N,x:(r2+Offset_options),AutoScaling
    move.w    x:(r2+Offset_options),x0      ; Check if scaling type is BFP
    andc      #FFT_SCALE_RESULTS_BY_DATA_SIZE,x0
    tst.w     x0
    bne       >BlockFloatingPoint           ; Block floating point...
                                            ; If not, it implies no scaling.
    bra       >NoScaling

AutoScaling

;----------------------------
; CASE 1: Code for AS.
;----------------------------

    moveu.w   x:(r5+Offset_n),n
    tfra      r4,r2                          ; r2 -> output array
    tfra      r3,r1                          ; r1 -> input array
    adda      n,r3                           ; r3 -> 1st Br of 1st pass
    tfra      r2,r0                          ; r0 -> 1st Br of first pass
    move.w    x:(r1)+,a                      ; Get 1st Ar of 1st pass
    adda      n,r0                           ; Adjust r0
    deca      r0
    asra      n                              ; Half the no. of DFT points
    move.w    x:(r0),b                       ; Save the memory contents so
                                             ;  that first parallel move
                                             ;  doesn't corrupt the data

    move.w    x:(r3)+,y0                     ; Get 1st Br of 1st pass

    if CODEWARRIOR_WORKAROUND==1
    do        n,>>_first_pass                ; The first pass has to be
                                             ;  repeated length/2 times
    else
    do        n,_first_pass                  ; The first pass has to be
                                             ;  repeated length/2 times
    endif

    add       y0,a        b,x:(r0)+          ; Find Cr,save Di in previous
                                             ;  Bi. r0 -> Br
    asr       a           x:(r1)+,b          ; Find Cr/2,get Ai,r1 -> next Ar
    rnd       a           x:(r3)+,x0         ; round, get Bi, r3 -> next Br
    sub       y0,a        a,x:(r2)+          ; Find Dr/2 ,save Cr/2,r2 -> Ci
    add       x0,b        a,x:(r0)+          ; Find Ci,save Dr/2 ,r0 ->Di
    asr       b           x:(r1)+,a          ; Find Ci/2, get next Ar,
                                             ;  r1 -> next Ai
    rnd       b           x:(r3)+,y0         ; round Ci/2, get next Br,
                                             ;  r3 -> next Bi
    sub       x0,b        b,x:(r2)+          ; Find Di/2, save Ci/2 ,
                                             ;  r2 -> next Cr. Di/2 saved
                                             ;  in the next loop
_first_pass
    move.w    b,x:(r0)                       ; Save last Di/2 of the 1st pass
    tfra      r4,r1                          ; r1 -> 1st Ar of 2nd pass
    tfra      r1,r3                          ; r3 -> 1st Ar of 2nd pass
    move.w    #2,c                           ; c1=passes
    adda      n,r3                           ; r3 -> first Br of second pass
    move.w    x:(r5+Offset_No_of_Stages),d
    sub.w     #2,d                           ; Set counter for no. of passes
    tst.w     d1
    beq       _end_second_pass

_second_pass
    dec.w    d
    LoadRx    x:(r5+Offset_Twiddle_br),r0    ; r0 ->mem. location of the first
                                             ;  twiddle fac. ,twiddle fac.
                                             ;  stored in bit reversed fashion
    asra      n                              ;  n =n/2
                                             ;   butterflies per group is n
    tfra      r3,r2                          ; r2 -> first Br of the first pass

    if CODEWARRIOR_WORKAROUND==1
    do        c1,>>_end_group                ; Middle loop is done b times
                                             ; c=2**(pass number-1)
                                             ; c = no. of groups/pass
    else
    do        c1,_end_group                  ; Middle loop is done b times
                                             ; c=2**(pass number-1)
                                             ; c = no. of groups/pass
    endif

    move.w    x:(r0)+,y0
    move.w    x:(r3)+,x0                     ; y0=Wr,x0=Br,r0 ->Wi, r3 ->Bi
    move.w    x:(r0)+,y1                     ; y1=Wi, r0 -> next Wr
                                             ;  (in bit reversed order)
    moveu.w   r0,n3                          ; Save twiddle factor pointer

    tfra      r1,r0                          ; Move r1 to r0
    adda      #-1,r2
    move.w    x:(r2),b                       ; Save the contents so that the
                                             ;  mem. contents aren't corrupted
                                             ;  in the first middle loop
    if CODEWARRIOR_WORKAROUND==1
    do        n,>>_end_bfly                    ; Inner loop is done n times
                                             ;  n=2**(L-passnumber)
                                             ;  n=no. of butterflies/group
    else
    do        n,_end_bfly                    ; Inner loop is done n times
                                             ;  n=2**(L-passnumber)
                                             ;  n=no. of butterflies/group
    endif

    mpy       y0,x0,b     b,x:(r2)+          ; b=WrBr,store the previous
                                             ;  butterfly's Di,r1->current Ar
    mpy       y1,x0,a     x:(r3)+,x0         ; a=+WiBr,get Bi
    mac       -y0,x0,a                       ; a=-WrBi+WiBr
    mac       y1,x0,b     x:(r1)+,x0         ; b=WrBr+WiBi,x0=Ar,r1 -> Ai
    add       x0,b                           ; Find Cr in b
    asr       b                              ; b=Cr/2,Scale down for storage
    rnd       b                              ; Round.The rounding done here
                                             ;  closely matches with the
                                             ;  autoscale mode rounding
    sub       b,x0                           ; Find Dr/2

    neg       a           b,x:(r0)+          ; a= WrBi-WiBr
                                             ; Store Cr/2
    move.w    x:(r1)+,b                      ; fetch Ai into b,r1 -> next Ar
    add       b,a         x0,x:(r2)+         ; Find Ci ,store Dr/2,r2 -> Di
    asr       a           x:(r3)+,x0         ; a=Ci/2,x0 = Next Br,
                                             ; r3 -> Next Bi
    rnd       a                              ; Round
    sub       a,b         a,x:(r0)+          ; b = Di/2, a = Ci/2
                                             ;  store Ci/2
                                             ;  Di/2 stored in next loop
_end_bfly

    move.w    b,x:(r2)+                      ; Store last butterfly's Ci

    moveu.w   n3,r0                         ; Restore the pointer pointing
                                            ;  to the twiddle factors

    tfra      r2,r1                          ; r1 -> next group's first Ar
    adda.l    n,r2                           ; r2 -> next group's first Br

    tfra      r2,r3                          ; r3 -> next group's first Br
_end_group

    tfra      r4,r1                          ; r1 ->1st Ar,at start of each pass
    tfra      r1,r3                          ; r3 ->1st Ar,at start of each pass
    asl       c                              ; double the no of groups for next
                                             ;  pass.
    adda      n,r3                           ;  r3 -> first Br of the next pass
    tst.w     d1                             ; Test the pass counter for Zero
    bne       _second_pass                   ; If less than zero then go to
_end_second_pass                             ;  last pass .

    LoadRx    x:(r5+Offset_Twiddle_br),r0    ; Get address of twiddle factors
    move.w    x:(r0)+,y0                     ; y0=Wr,r0 -> Wi

    if CODEWARRIOR_WORKAROUND==1
    do        c1,>>_last_pass                ; N/2 groups in last pass
    else
    do        c1,_last_pass                  ; N/2 groups in last pass
    endif

    tfra      r3,r2
    move.w    x:(r3)+,x0                     ; x0=Br,r3 -> Bi
    mpy       x0,y0,b     x:(r0)+,y1         ; b=BrWr, y1=Wi, r0 ->Next Wr
    mpy       y1,x0,a     x:(r3)+,x0         ; a=WiBr
    mac       -y0,x0,a                       ; a=-WrBi+WiBr
    mac       y1,x0,b     x:(r1)+,x0         ; b=WiBi+WrBr, x0=Ar, r1 -> Ai
    add       x0,b                           ; Find Cr
    asr       b                              ; Find Cr/2
    rnd       b                              ; Round
    sub       b,x0                           ; Find Dr/2
    move.w    b,x:(r1-1)                     ; Store   Cr/2 , r1 -> Ai
    move.w    x:(r1)+,b                      ; b = Ai, r1 -> Ai
    sub       b,a         x0,x:(r2)+         ; Find -Ci , store Dr/2,r2 ->Di
    asr       a           x:(r3)+n,x0        ; Find -Ci/2, Dummy read to
                                             ;  adjust r3 to next Br
    rnd       a           x:(r0)+,y0         ; y0=Wr,r0 -> Wi
    add       a,b         x:(r1)+n,x0        ; Find Di/2,Dummy read to
                                             ;  adjust r1
    neg       a           b,x:(r2)+          ; Find Ci/2 ,store Di/2
    move.w    a,x:(r1-3)                     ; Store Ci/2
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

    tfra      r3,r1                          ; r1 -> 1st Ar of 1st pass
    moveu.w   x:(r5+Offset_n),n
    tfra      r4,r2
    move.w    n,a
    asl       a
    cmpa      r4,r1
    beq       _avoid_copy_bfp

    if CODEWARRIOR_WORKAROUND==1
    do        a,>>_endcpy
    else
    do        a,_endcpy
    endif

    move.w    x:(r1)+,y0
    move.w    y0,x:(r2)+
_endcpy

_avoid_copy_bfp

    tfra      r4,r1
    tfra      r1,r3                          ; r3 -> 1st Ar of 1st pass
    move.w    x:(r5+Offset_No_of_Stages),x0
    dec.w     x0
    move.w    x0,tmp
    move.w    #1,d                           ; d set to 1, for controlling
                                             ;  the no. of groups in the pass
    adda      n,r3                           ; r3 -> first Br of first pass
    move.w    #0,ScaleBF                     ; Memory location used as a flag
                                             ;  0 indicates last pass is not
                                             ;  to be scaled down
_first_passBF

    tfra      r1,r0                          ; r0 -> first Br
    move.w    x:(r5+Offset_n),y0             ; y0= number of points in fft
    move.w    #0,x0                          ; x0= 0; smallest positive number
    move.w    x:(r0)+,a                      ; a= first Ar, r0 -> First Ai

    if CODEWARRIOR_WORKAROUND==1
    do        y0,>>_chkp                     ; Repeat for all samples
    else
    do        y0,_chkp                       ; Repeat for all samples
    endif

    abs       a           x:(r0)+,b          ; Get the magnitude in acc a
                                             ;  get the next data in acc b
    or.w      a,x0                           ; Get the most significant bit of
                                             ;  the largest number in x0
    abs       b           x:(r0)+,a          ; Get the magnitude of next data
                                             ;  in b and get next data in a
    or.w      b,x0                           ; Find the max value in x0

_chkp

    LoadRx    x:(r5+Offset_Twiddle_br),r0    ; r0 ->memory location of the
                                             ;  first twiddle factor Wr
    asra      n                              ; n = n/2
    tfra      r3,r2                          ; r2 -> first Br of the pass
    tst.w     x0
    blt       _MSB_8000_4
    cmp.w     #$2000,x0                      ; Compare the max value with 0.25
    blt       _nscdown                       ; If greater, perform the sc_down
                                             ;  pass, else no scale down pass
_MSB_8000_4

    inc.w     ScaleBF                        ; Increment the scale_fac by one

    if CODEWARRIOR_WORKAROUND==1
    do        d1,>>_end_groupBF              ; Middle loop is done d times
                                             ;  d=2**(pass number-1)
                                             ;  d = no. of groups/pass
    else
    do        d1,_end_groupBF                ; Middle loop is done d times
                                             ;  d=2**(pass number-1)
                                             ;  d = no. of groups/pass
    endif

    move.w    x:(r0)+,y0
    move.w    x:(r3)+,x0                     ; y0=Wr, x0=Br, r0 -> Wi, r3 -> Bi
    move.w    x:(r0)+,y1                     ; y1=Wi, r0 -> next Wr
                                             ;  (in bit reversed order)

    moveu.w   r0,n3                         ; Save twiddle factor

    tfra      r1,r0                          ; Move r1 to r0
    deca      r2
    move.w    x:(r2),b                       ; Save the contents so that they
                                             ;  are not corrupted in the first
                                             ;  middle loop
    if CODEWARRIOR_WORKAROUND==1
    do        n,>>_end_bflyBF                ; Inner loop is done n times
                                             ;  n=2**(L-passnumber)
                                             ;  n=no. of butterflies/group
    else
    do        n,_end_bflyBF                  ; Inner loop is done n times
                                             ;  n=2**(L-passnumber)
                                             ;  n=no. of butterflies/group
    endif

    mpy       y0,x0,b     b,x:(r2)+          ; b=WrBr, store previous butter-
                                             ;  fly's Di , r1 -> current Ar
    mpy       y1,x0,a     x:(r3)+,x0         ; a=+WiBr, get Bi
    mac       -y0,x0,a                       ; a=-WrBi+WiBr
    mac       y1,x0,b     x:(r1)+,x0         ; b=WrBr+WiBi,x0=Ar,r1 -> Ai
    add       x0,b                           ; Find Cr in b
    asr       b                              ; b=Cr/2, Scale down for storage
    rnd       b                              ; Round. The rounding done here
                                             ;  closely matches with the
                                             ;  autoscale down mode rounding
    sub       b,x0                           ; Find Dr/2
    neg       a           b,x:(r0)+          ; a = WrBi - WiBr
                                             ;  store Cr/2
    move.w    x:(r1)+,b                      ; Fetch Ai into b, r1-> next Ar
    add       b,a         x0,x:(r2)+         ; Find Ci, store Dr/2, r2-> Di
    asr       a           x:(r3)+,x0         ; a=Ci/2, x0= Next Br,
                                             ;  r3-> Next Bi
    rnd       a                              ; Round
    sub       a,b         a,x:(r0)+          ; b=Di/2, a=Ci/2, store Ci/2
                                             ;  Di/2 is stored in next loop
_end_bflyBF
    move.w    b,x:(r2)+                      ; Store last butterfly's Ci

    moveu.w   n3,r0                         ; Restore the pointer pointing
                                            ;  to the twiddle factors

    tfra      r2,r1                          ; r1 -> next group's first Ar
    adda.l    n,r2                           ; r2 -> next group's first Br
    tfra      r2,r3                          ; r3 -> next group's first Br'
_end_groupBF

    tfra      r4,r3                          ; r3 -> 1st Ar, for start of each
                                             ;  pass
    tfra      r3,r1                          ; r1 -> 1st Ar,for start of each
                                             ;  pass
    asl       d                              ; Double the no of groups for next
                                             ;  pass
                                             ; d=2**(pass number-1)
    adda      n,r3                           ;  r3-> first Br of the next pass
    dec.w     tmp                            ; Test for pass counter
    bne       _first_passBF                  ; perform the loop till counter= 0
    bra       _lpass
_nscdown                                     ; No scale down mode

    move.w    n,c1                           ; Save the number of buttrflies
    moveu.w   #0,n                           ; Offset for each pass

    if CODEWARRIOR_WORKAROUND==1
    do        d1,>>_end_group2               ; Middle loop is done r2 times
                                             ;  d=2**(pass number-1)

    else
    do        d1,_end_group2                 ; Middle loop is done r2 times
                                             ;  d=2**(pass number-1)

    endif

    move.w    x:(r0)+,y0
    move.w    x:(r3)+,x0                     ; y0=Wr, x0=first Br of the
                                             ;  group. r0->Next Wr. r3->first
                                             ;  Bi of the group.
    if CODEWARRIOR_WORKAROUND==1
    do        c1,>>_end_bfly2                ; Inner loop is done n times
                                             ;  c1=2**(L-passnumber)
    else
    do        c1,_end_bfly2                  ; Inner loop is done n times
                                             ;  c1=2**(L-passnumber)
    endif

    mpy       x0,y0,b     x:(r0)+n,y1        ; y1=Wi, b=WrBr, x0=Bi, r3->Br
    move.w    x:(r3)-,x0                     ;  r0->Wi

    macr      x0,y1,b     x:(r1)+n,a         ; b=WrBr+WiBi, a=Ar, r1->Ar
    add       a,b                            ; Find Cr in b, b=Ar+WrBr+WiBi,
    asl       a           b,x:(r1)+          ; a=2Ar, store Cr. r1->Ai
    sub       b,a         x:(r3)+,b          ; Find Dr in a, b=Br, r3->Bi
    neg       b           a,x:(r2)+          ; b=-Br, store Dr, r2->Bi
    mpy       b1,y1,b     x:(r3)+,x0         ; b=-Brwi, x0=Bi, r3->Next Br
    macr      y0,x0,b     x:(r1)+n,a         ; b=WrBi-WiBr, a=Ai, r1->Ai
    add       a,b         x:(r3)+,x0         ; Find Ci in b, x0=Next Br,
                                             ;  r3->Next Bi
    asl       a           b,x:(r1)+          ; a=2Ai, store Ci, r1->Next Ar
    sub       b,a                            ; Find Di
    move.w    a,x:(r2)+                      ; Store Di, r2->Next Br
_end_bfly2

    adda      #1,r0                          ; r0->First Wr of next group
    moveu.w   c1,r3                          ; Restore the no. of butterflies
    tfra      r2,r1                          ; r1->next group,s first Ar
    adda.l    r3,r2

    tfra      r2,r3                          ; r2 -> next group's first Br'
                                             ; r3-> Next groups first Br
_end_group2

    moveu.w   c1,n
    tfra      r4,r3                          ; r3 -> 1st Ar, for start of each
                                             ;  pass
    tfra      r3,r1                          ; r3 -> 1st Ar of next pass
    asl       d                              ; Double the no of groups for next
                                             ;  pass ,Dummy read into x0 to
                                             ;  d=2**(pass number-1)
    adda      n,r3                           ;  adjust r3, r3 -> first Br of
                                             ;  the next pass
    dec.w     tmp                            ; Test the pass counter for zero
    bne       >_first_passBF                 ; If not equal to zero then go to
                                             ;  beginning of the pass
_lpass                                       ;  data scanning for the last pass

    tfra      r4,r0                          ; r0->first Ar
    move.w    x:(r5+Offset_n),y0             ; y0= number of points in fft
    move.w    #0,x0                          ; x0= 0; smallest positive number
    move.w    x:(r0)+,a                      ; a= first Ar, r0 -> First Ai

    if CODEWARRIOR_WORKAROUND==1
    do        y0,>>_chklp                    ; Repeat for all samples
    else
    do        y0,_chklp                      ; Repeat for all samples
    endif

    abs       a           x:(r0)+,b          ; Get the magnitude in acc a
                                             ;  get the next data in acc b
    or.w      a,x0                           ; Get the most significant bit of
                                             ;  the largest number in x0
    abs       b           x:(r0)+,a          ; Get the magnitude of next data
                                             ;  in b and get next data in a
    or.w      b,x0                           ; Find the max value in x0
_chklp

    LoadRx    x:(r5+Offset_Twiddle_br),r0    ; r0->first Wr
    tst.w     x0
    blt       _MSB_8000_5
    cmp.w     #0.25,x0                       ; if the largest value is > 0.25
    blt       _lnscdown                      ;  perform scale down pass

_MSB_8000_5

    inc.w     ScaleBF                        ; Increment the scale factor
    move.w    x:(r0)+,y0                     ; y0=Wr, r0 -> Wi

    if CODEWARRIOR_WORKAROUND==1
    do        d1,>>_last_pass1               ; N/2 groups in last pass
    else
    do        d1,_last_pass1                 ; N/2 groups in last pass
    endif

    tfra      r3,r2
    move.w    x:(r3)+,x0                     ; y0=Wr, x0=Br, r0 -> Wi, r3 -> Bi
    mpy       x0,y0,b     x:(r0)+,y1         ; b=BrWr, y1=Wi, r0 -> Next Wr
    mpy       y1,x0,a     x:(r3)+,x0         ; a=WiBr
    mac       -y0,x0,a                       ; a=-WrBi+WiBr
    mac       y1,x0,b     x:(r1)+,x0         ; b=WiBi+WrBr, x0=Ar, r1 -> Ai
    add       x0,b                           ; Find Cr
    asr       b                              ; Find Cr/2
    rnd       b                              ; Round
    sub       b,x0                           ; Find Dr/2
    move.w    b,x:(r1-1)                     ; Store Cr/2 , r1 -> Ai
    move.w    x:(r1)+,b                      ; b = Ai, r1 -> Ai
    sub       b,a         x0,x:(r2)+         ; Find -Ci , store Dr/2,r2 ->Di
    asr       a           x:(r3)+n,x0        ; Find -Ci/2, Dummy read to adjust
                                             ;  r3 to next Br
    rnd       a           x:(r0)+,y0                    ; Round,Dummy read to adjust r1
                                             ; r1 -> Next Ar
    add       a,b         x:(r1)+n,x0        ; Find Di/2
    neg       a           b,x:(r2)+          ; Find Ci/2 ,store Di/2
    move.w    a,x:(r1-3)                     ; Store Ci/2
_last_pass1
    bra       _rev                           ; Perform bitreversal operation
_lnscdown                                    ; Last pass no scale down
    move.w    #0,n                           ; Offset for the last pass
    move.w    x:(r0)+,y0                     ; y0=Wr, r0->Wi

    if CODEWARRIOR_WORKAROUND==1
    do        d1,>>_last_pass2               ; N/2 number of butterflies
    else
    do        d1,_last_pass2                 ; N/2 number of butterflies
    endif

    move.w    x:(r3)+,x0                     ; y0=Wr, x0=Br, r0->Wi, r3->Bi
    mpy       x0,y0,b     x:(r0)+,y1
    move.w    x:(r3)-,x0
                                             ; b=WrBr, y1=Wi, x0=Bi, r0->Wi
                                             ;  r3->Br
    macr      x0,y1,b     x:(r1)+n,a         ; b=WrBr+WiBi, a=Ar, r1->Ar
    add       a,b                            ; Find Cr in b, b=Ar+WrBr+WiBi,
    asl       a           b,x:(r1)+          ; a=2Ar, store Cr, r1->Ai
    sub       b,a         x:(r3)+n,b         ; Find Dr in a, a=2Ar-Cr, b=Br
                                             ;  r3->Br
    neg       b           a,x:(r3)+          ; b=-Br, store Dr, r3->Bi
    mpy       b1,y1,b     x:(r3)+n,x0        ; b=-BrWi, x0=Bi, r3->Bi
    macr      y0,x0,b     x:(r1)+n,a         ; b=WrBi-BrWi, a=Ai, r1->Ai
    add       a,b         x:(r0)+,y0         ; find Ci in acc b
    asl       a           b,x:(r1)+          ; a=2Ai, store Ci, r1->Br
    sub       b,a         x:(r1)+,x0         ; Find Di in acc a, dummy read
                                             ;  in x0, r1->Bi
    move.w    a,x:(r3)+                      ; Store Di, r3-> Ar of next
                                             ;  butterfly
    adda      #1,r1                          ;  r1->Ar of next
    adda      #2,r3                          ;  butterfly
                                             ; r3-> Br of next butterfly
_last_pass2
_rev

    jmp       PostProcessing

NoScaling

;---------------------------------------------------------------
; CASE 3: Code for no-scaling.
;---------------------------------------------------------------

    moveu.w   x:(r2+Offset_n),n
    tfra      r4,r2
    tfra      r3,r1                          ; r3 -> 1st Br of 1st pass
    adda      n,r3
    tfra      r2,r0                          ; r0 -> 1st Br of first pass
    move.w    x:(r1)+,a                      ; Get 1st Ar of 1st pass
    adda      n,r0                           ; Adjust r0 so that in first
    deca      r0
    asra      n                              ;  half the no. of DFT points
    move.w    x:(r0),b                       ; Save the memory cotents so
                                             ;  that first parallel move
                                             ;  doesn't corrupt the data
    move.w    x:(r3)+,y0                     ; Get 1st Br of 1st pass

    if CODEWARRIOR_WORKAROUND==1
    do        n,>>_first_pass_NS             ; The first pass has to be
                                             ;  repeated length/2 times
    else
    do        n,_first_pass_NS               ; The first pass has to be
                                             ;  repeated length/2 times
    endif

    add       y0,a        b,x:(r0)+          ; Find Cr,save Di in previous
    rnd       a           x:(r3)+,x0         ; round, get Bi, r3 -> next Br
    sub       y0,a        a,x:(r2)+          ; save Cr,r2 -> Ci
    sub       y0,a        x:(r1)+,b          ; Find Dr,get Ai,r1 -> next Ar
    add       x0,b        a,x:(r0)+          ; Find Ci,save Dr ,r0 ->Di
    rnd       b           x:(r3)+,y0         ; round Ci, get next Br,
                                             ; r3 -> next Bi
    sub       x0,b        b,x:(r2)+          ; Find Di, save Ci,
                                             ; r2 -> next Cr.
    sub       x0,b        x:(r1)+,a          ; Find Di, get next Ar,
                                             ; in the next loop
_first_pass_NS
    move.w    b,x:(r0)                       ; Save last Di/2 of the 1st pass
    tfra      r4,r1                          ; r1 -> 1st Ar of 2nd pass
    tfra      r1,r3                          ; r3 -> 1st Ar of 2nd pass
                                             ; n set to N, used for
                                             ; controlling the no.of bflies
                                             ; per group in the pass.
    move.w    #2,c                           ; c1 set to 2,used for controlling
                                             ;  the no. of groups in the pass
    adda      n,r3                           ; r3 -> first Br of second pass
    move.w    x:(r5+Offset_No_of_Stages),d   ; Set counter for no. of passes
                                             ;  last pass is also separate
    sub.w     #2,d
    tst.w     d1
    beq       _fft_4_NS

_second_pass_NS
    LoadRx    x:(r5+Offset_Twiddle_br),r0    ; r0 ->mem. location of the first
    dec.w     d                              ; twiddle fac. ,twiddle fac.
                                             ; stored in bit reversed fashion
    asra      n                              ; n =n/2
                                             ; butterflies per group is n
    tfra      r3,r2                          ; r2 -> first Br of the first pass

    if CODEWARRIOR_WORKAROUND==1
    do        c1,>>_end_group_NS             ; Middle loop is done c1 times
                                             ; c1 = no. of groups/pass
    else
    do        c1,_end_group_NS               ; Middle loop is done c1 times
                                             ; c1 = no. of groups/pass
    endif

    move.w    x:(r0)+,y0
    move.w    x:(r3)+,x0                     ; y0=Wr,x0=Br,r0 ->Wi, r3 ->Bi
    move.w    x:(r0)+,y1                     ; y1=Wi, r0 -> next Wr
                                             ; (in bit reversed order)

    moveu.w   r0,n3                         ; Save twiddle factor pointer

    tfra      r1,r0                          ; Move r1 to r0
    deca      r2
    move.w    x:(r2),b                       ; Save the contents so that the
                                             ;  mem. contents aren't corrupted
                                             ;  in the first middle loop
    if CODEWARRIOR_WORKAROUND==1
    do        n,>>_end_bfly_NS               ; Inner loop is done n times
                                             ; n=2**(L-passnumber)
                                             ; n=no. of butterflies/group
    else
    do        n,_end_bfly_NS                 ; Inner loop is done n times
                                             ; n=2**(L-passnumber)
                                             ; n=no. of butterflies/group
    endif

    mpy       y0,x0,b     b,x:(r2)+          ; b=WrBr,store the previous
                                             ; butterfly's Di,r1->current Ar
    mpy       y1,x0,a     x:(r3)+,x0         ; a=+WiBr,get Bi
    mac       -y0,x0,a                       ; a=-WrBi+WiBr
    mac       y1,x0,b     x:(r1)+,x0         ; b=WrBr+WiBi,x0=Ar,r1 -> Ai
    add       x0,b                           ; Find Cr in b
    rnd       b                              ; Round.The rounding done here
                                             ;  closely matches with the
                                             ;  autoscale mode rounding
    asl       x0                             ; Get 2*Ar
    sub       b,x0                           ; Find Dr [ Dr = 2*Ar - Cr]
    neg       a           b,x:(r0)+          ; a= WrBi-WiBr
                                             ; Store Cr
    move.w    x:(r1)+,b                      ; fetch Ai into b,r1 -> next Ar
    add       b,a         x0,x:(r2)+         ; Find Ci ,store Dr,r2 -> Di
                                             ; r3 -> Next Bi
    rnd       a           x:(r3)+,x0         ; Round
    asl       b                              ; Get 2*Ai
    sub       a,b         a,x:(r0)+          ; Find Di,[Di = 2*Ai - Ci]
                                             ; store Ci
                                             ; Di stored in next loop
_end_bfly_NS

    move.w    b,x:(r2)+                      ; Store last butterfly's Ci
    moveu.w   n3,r0                         ; Restore the pointer pointing
                                            ;  to the twiddle factors
    tfra      r2,r1                          ; r1 -> next group's first Ar
    adda.l    n,r2                           ; r2 -> next group's first Br
    tfra      r2,r3                          ; r3 -> next group's first Br
_end_group_NS
    tfra      r4,r1                          ; r1 ->1st Ar,at start of each pass
    tfra      r1,r3                          ; r3 ->1st Ar,at start of each pass
    asl       c                              ; double the no of groups for next
                                             ;  pass
    adda      n,r3                           ; r3 -> first Br of the next pass
    tst.w     d1                             ; Test the pass counter for Zero
    bne       _second_pass_NS                ; If less than zero then go to
                                             ; last pass .
_fft_4_NS

    LoadRx    x:(r5+Offset_Twiddle_br),r0    ; Get address of twiddle factors
    move.w    x:(r0)+,y0                     ; y0=Wr,r0 -> Wi

    if CODEWARRIOR_WORKAROUND==1
    do        c1,>>_last_pass_NS             ; N/2 groups in last pass
    else
    do        c1,_last_pass_NS               ; N/2 groups in last pass
    endif

    tfra      r3,r2
    move.w    x:(r3)+,x0                     ; y0=Wr,x0=Br,r0 -> Wi,r3 -> Bi
    mpy       x0,y0,b     x:(r0)+,y1         ; b=BrWr, y1=Wi, r0 ->Next Wr
    mpy       y1,x0,a     x:(r3)+,x0         ; a=WiBr
    mac       -y0,x0,a                       ; a=-WrBi+WiBr
    mac       y1,x0,b     x:(r1)+,x0         ; b=WiBi+WrBr, x0=Ar, r1 -> Ai
    add       x0,b        x:(r0)+,y0         ; Find Cr
    rnd       b                              ; Round
    asl       x0                             ; Get 2*Ar
    sub       b,x0                           ; Find Dr [Dr = 2*Ar - Cr]
    move.w    b,x:(r1-1)                     ; Store Cr , r1 -> Ai
    move.w    x:(r1)+,b                      ; b = Ai, r1 -> Ai
    sub       b,a         x0,x:(r2)+         ; Find -Ci , store Dr,r2 ->Di
    rnd       a           x:(r3)+n,x0        ; Find -round(Ci), Dummy read to
                                             ; adjust r3 to next Br
    asl       b                              ; Get 2*Ai
    add       a,b         x:(r1)+n,x0        ; Find Di,Dummy read to
                                             ; adjust r1
    neg       a           b,x:(r2)+          ; Find Ci ,store Di
    move.w    a,x:(r1-3)                     ; Store Ci
_last_pass_NS
_End_fft_NS

PostProcessing

    ;--------------------------------------------------------
    ; Set up the parameters for calling bit-reverse function
    ;--------------------------------------------------------

    move.w    x:(r5+Offset_n),y0             ; y0 = N, the size of FFT.
    tfra      r4,r2                          ; r2 -> pRFFT.
    tfra      r2,r3
    jsr       Fdfr16Cbitrev_                 ; Call bit reverse function.
    cmp.w     #FAIL,y0                       ; Result is returned in y0
    bne       BitRevPass                     ; Bit reverse passed, go to FFT.
    moveu.w   old_omr,omr                    ; Restore previous OMR value.
    adda      #-Scratch_Size,sp
    move.l    x:(sp)-,r5
    move.l    x:(sp)-,d
    move.l    x:(sp)-,d2
    move.l    x:(sp)-,c
    move.l    x:(sp)-,c2
    rts                                      ; FAIL is returned in y0.

    ;----------------------------------------------
    ; Post processing for RFFT on the output buffer
    ;----------------------------------------------

BitRevPass

    brset     #FFT_SCALE_RESULTS_BY_N,x:(r5+Offset_options),Start1
    move.w    x:(r5+Offset_options),x0       ; Group offset.
    andc      #FFT_SCALE_RESULTS_BY_DATA_SIZE,x0
    tst.w     x0
    jeq       DonotScale                     ; No Scaling ...

    tfra      r4,r0                          ; r0 -> pZ, the output buffer.
    move.w    x:(r5+Offset_n),y0             ; y0 = N.
    move.w    #0,x0                          ; x0 = smallest positive number.
    move.w    x:(r0)+,a                      ; a = First data value (real).

    if CODEWARRIOR_WORKAROUND==1
    do        y0,>>ChkMagnitude
    else
    do        y0,ChkMagnitude
    endif

    abs       a           x:(r0)+,b          ; a = |a|, b = Next data value
                                             ;  (imag).
    or.w      a,x0                           ; Get the most significant bit of
                                             ;  the largest number, in x0.
    abs       b           x:(r0)+,a          ; b = |b|, a = Next data value
                                             ;  (real).
    or.w      b,x0                           ; Get the most significant bit of
                                             ;  the largest number, in x0.
ChkMagnitude

    tst.w     x0
    blt       _MSB_8000_6
    cmp.w     #THRESHOLD_BFP,x0              ; Compare the max value with 0.25.
                                             ; If <=, no scaling is required in
    blt       DonotScale                                       ;  the first pass.


_MSB_8000_6

    inc.w     ScaleBF                        ; Increment the count which finally

Start1
    tfra      r4,r1                          ; r1 --> zr[0]
    LoadRx    x:(r5+Offset_Twiddle),r3       ; a = Twiddle[0].real
    adda      #3,r3                          ; r3 -> Twiddle[1].imag
    tfra      r1,r0                          ; r0 -->zr[0]
    moveu.w   x:(r5+Offset_n),n              ; size of FFT
    adda.l    n,r0
    move.w    #-1,n                          ; n set for decrementing r1,r0,r3
    move.w    x:(r1)+,b                      ; A = b = zr[0], r1 --> zi[0]
    move.w    x:(r1)-,a                      ; C = a = zi[0], r1 --> zi[0]
    move.w    a,y1                           ; Copy of C
    add       b,a         x:(r0)+n,x0        ; a = A + C, Dummy read,
                                             ;  r0 -> pW[N/2-1].imag
                                             ;  (i.e., last location)
    asr       a                              ; a = (A + C)/2 = zr[0]/2
    sub       y1,a        a,x:(r1)+          ; a = (A - C)/2, Store zr[0]/2,
                                             ;  r1 -> zi[0]
    move.w    a,x:(r1)+                      ; Store zi[0]/0, r1 -> zr[1]
    move.w    x:(r5+Offset_n),a
    asr       a           x:(r1)+,y1         ; y1 = pW[i].real = zr[1]
                                             ;  r1 -> zi[1].
    if CODEWARRIOR_WORKAROUND==1
    do        a1,>>_endpass_AS               ; for k=0 to (N/4+1)
    else
    do        a1,_endpass_AS                 ; for k=0 to (N/4+1)
    endif

    move.w    x:(r0)+n,b                     ; get zi[N/2-k], r0 --> zr[N/2-k]
    asr       b           x:(r1)+n,a         ; get zi[k], r1 --> zr[k]
    asr       a           x:(r3)+n,x0        ; get sin(2*pi*k/N)
                                             ;  r3 --> cos(2*pi*k/N)
    add       a,b                            ; compute C
    asl       a           b,x:(r1)+          ; Find 2*zi[k],save C at zr[k]
                                             ;  r1->zi[k]
    sub       b,a         x:(r0)+,b          ; compute D = 2*zi[k] - C
    asr       b                              ;  get zr[N/2-k],r0 -> zi[N/2-k]
    tfr       y1,a        a,x:(r0)+n         ; Get zr[k],save D at zi[N/2-k]
                                             ;  r0 --> zr[N/2-k]
    asr       a           x:(r1)+n,y0        ; compute B, dummy read
                                             ;  r1 --> zr[k] or C
    add       b,a         x:(r1)+,y1         ; compute A, y1 = C, r1 --> zi[k]
    asl       b                              ; Find 2*zr[N/2-k]
    sub       a,b
    move.w    b,y0                           ; Save B
    mpy       x0,y0,b     x:(r3)+,x0         ; Compute sin(2PIk/N)*B
                                             ;  get cos(2*pi*k/N)
                                             ;  r3 --> sin(2*pi*k/N)
    macr      y1,x0,b                        ; compute sin(2PIk/N)*B
                                             ;  cos(2PIk/N)*C
    add       a,b                            ; compute 2*xr[k]
    asr       b                              ; compute xr[k]
    sub       b,a         b,x:(r0)+          ; compute xr[N/2-k]
                                             ;  save xr[k] at zr[N/2-k]
                                             ;  r0 ->zi[k]
    mpy       y0,x0,b     x:(r0)+n,y0        ; b = B * Twid[i].real
                                             ;  y0 = D.
    move.w    x:(r3)+,x0                     ; x0 = Twid[i].imag
                                             ; compute B*cos(2*pi*k/N)
                                             ;  get D,r0 -> zr[N/2-k]
                                             ;  get sin(2*pi*k/N)
                                             ;  r3 -> cos(2*pi*k/N)
    macr      -y1,x0,b                       ; compute -C*sin(2*pi*k/N)+
                                             ;  B*cos(2*pi*k/N)
    add       y0,b        x:(r3)+,x0         ; compute 2*xi[k],dummy read
                                             ;  r3 -> sin(2*pi*(k+1)/N)
    asr       b           x:(r1)+n,x0        ; compute xi[k],
                                             ;  dummy read ,r1 -> zr[k]
    move.w    b,x0
    sub       y0,b        x:(r0)+,y0         ; compute xi[N/2-k]

    move.w    y0,x:(r1)+
    move.w    b,x:(r0)+n
    move.w    a,x:(r0)+n
    move.w    x0,x:(r1)+
    move.w    x:(r1)+,y1                     ; get zr[k+1],r1 -> zi[k+1]
                                             ;  r1 -> zi[k+1]
_endpass_AS
                                             ; computations for k=N/4+1
                                             ;  at this stage r0 = r1
    move.w    x:(r1)+n,y0
    move.w    y1,x:(r1)+
    move.w    y0,x:(r1)

_end_rfft_AS

    bra       return_AS

DonotScale

    tfra      r4,r1                          ; r1 --> zr[0]

    LoadRx    x:(r5+Offset_Twiddle),r3       ; a = Twiddle[0].rea
    adda      #3,r3                          ; r3 -> Twiddle[1].imag
    tfra      r1,r0                          ; r0 -->zr[0]
    moveu.w   x:(r5+Offset_n),n              ; size of FFT
    adda.l    n,r0                           ; r0 -->zr[N/2-k]
    move.w    #-1,n                          ; n set for decrementing r1,r0,r3
    move.w    x:(r1)+,b                      ; A = b = zr[0], r1 --> zi[0]
    move.w    x:(r1)-,a                      ; C = a = zi[0], r1 --> zi[0]
    move.w    a,y1                           ; Copy of C
    add       b,a         x:(r0)+n,x0        ; a = A + C, Dummy read,
                                             ;  r0 -> pW[N/2-1].imag
                                             ;  (i.e., last location)
    sub       y1,b        a,x:(r1)+          ; b = A - C, Store zr[0],
                                             ;  r1 -> zi[0]
    move.w    b,x:(r1)+                      ; Store zi[0], r1 -> zr[1]

    move.w    x:(r5+Offset_n),a
    asr       a           x:(r1)+,y1         ; y1 = pW[i].real

    if CODEWARRIOR_WORKAROUND==1
    do        a1,>>_endpass_NS               ; for k=0 to (N/4+1)
    else
    do        a1,_endpass_NS                 ; for k=0 to (N/4+1)
    endif

    move.w    x:(r0)+n,b                     ; get zi[N/2-k], r0 --> zr[N/2-k]
    asr       b           x:(r1)+n,a         ; get zi[k], r1 --> zr[k]
    asr       a           x:(r3)+n,x0        ; get sin(2*pi*k/N)
                                             ;  r3 --> cos(2*pi*k/N)
    add       a,b                            ; compute C
    asl       a           b,x:(r1)+          ; Find 2*zi[k],save C at zr[k]
                                             ;  r1->zi[k]
    sub       b,a         x:(r0)+,b          ; compute D = 2*zi[k] - C
    asr       b                              ;  get zr[N/2-k],r0 -> zi[N/2-k]
    tfr       y1,a        a,x:(r0)+n         ; Get zr[k],save D at zi[N/2-k]
                                             ;  r0 --> zr[N/2-k]
    asr       a           x:(r1)+n,y0        ; compute B, dummy read
                                             ;  r1 --> zr[k] or C
    add       b,a         x:(r1)+,y1         ; compute A, y1 = C, r1 --> zi[k]
    asl       b                              ; Find 2*zr[N/2-k]
    sub       a,b
    move.w    b,y0                           ; Save B
    mpy       x0,y0,b     x:(r3)+,x0         ; Compute sin(2PIk/N)*B
                                             ;  get cos(2*pi*k/N)
                                             ;  r3 --> sin(2*pi*k/N)
    mac       y1,x0,b                        ; compute sin(2PIk/N)*B+
                                             ;  cos(2PIk/N)*C
    add       a,b                            ; compute 2*xr[k]
    rnd       b
    asl       a           b,x:(r0)+          ; a = 2*A
                                             ;  save xr[k] at zr[N/2-k]
                                             ;  r0 ->zi[k]
    sub       b,a                            ; compute xr[N/2-k]
    mpy       y0,x0,b     x:(r0)+n,y0        ; b = B * Twid[i].real
                                             ;  y0 = D.
    move.w    x:(r3)+,x0                     ; x0 = Twid[i].imag
                                             ; compute B*cos(2*pi*k/N)
                                             ;  get D,r0 -> zr[N/2-k]
                                             ;  get sin(2*pi*k/N)
                                             ;  r3 -> cos(2*pi*k/N)
    mac       -y1,x0,b                       ; compute -C*sin(2*pi*k/N)+
                                             ;  B*cos(2*pi*k/N)
    add       y0,b        x:(r3)+,x0         ; compute 2*xi[k],dummy read
                                             ;  r3 -> sin(2*pi*(k+1)/N)
    rnd       b           x:(r1)+n,x0        ; compute xi[k],
                                             ;  dummy read ,r1 -> zr[k]
    move.w    b,x0
    asl       y0                             ; y0 = 2*D
    sub       y0,b        x:(r0)+,y0         ; compute xi[N/2-k]
    move.w    y0,x:(r1)+
    move.w    b,x:(r0)+n
    move.w    a,x:(r0)+n
    move.w    x0,x:(r1)+
    move.w    x:(r1)+,y1                     ; get zr[k+1],r1 -> zi[k+1]
                                             ;  r1 -> zi[k+1]
_endpass_NS
                                             ; computations for k=N/4+1
                                             ; at this stage r0 = r1
    move.w    x:(r1)+n,y0
    move.w    y1,x:(r1)+
    move.w    y0,x:(r1)

_end_rfft_NS

;----------------------------------------------------------
; Restore the existing omr and set the saturation bit to 0
;----------------------------------------------------------

return_AS


    move.w    x:(r5+Offset_options),x0
    brclr     #FFT_SCALE_RESULTS_BY_N,x0,return_BFP
                                             ; Is it AS?
    move.w    x:(r5+Offset_No_of_Stages),y0
    add.w     #1,y0

    moveu.w   old_omr,omr                    ; Restore previous OMR value.
    adda      #-Scratch_Size,sp
    move.l    x:(sp)-,r5
    move.l    x:(sp)-,d
    move.l    x:(sp)-,d2
    move.l    x:(sp)-,c
    move.l    x:(sp)-,c2

    rts

return_BFP

    brclr     #FFT_SCALE_RESULTS_BY_DATA_SIZE,x0,return_NS
    move.w    ScaleBF,y0                     ; Return amount of scaling done in
    moveu.w   old_omr,omr                    ; Restore previous OMR value.
    adda      #-Scratch_Size,sp
    move.l    x:(sp)-,r5
    move.l    x:(sp)-,d
    move.l    x:(sp)-,d2
    move.l    x:(sp)-,c
    move.l    x:(sp)-,c2                                         ;  BFP method.

    rts

return_NS


    move.w    #0,y0                          ; No scaling is done, return 0.
    moveu.w   old_omr,omr                    ; Restore previous OMR value.
    adda      #-Scratch_Size,sp
    move.l    x:(sp)-,r5
    move.l    x:(sp)-,d
    move.l    x:(sp)-,d2
    move.l    x:(sp)-,c
    move.l    x:(sp)-,c2
    rts

    ENDSEC
