;********************************************************************
;
; (c) Freescale Semiconductor
; 2004 All Rights Reserved
;
;
;********************************************************************
;
; File Name:    dfr16corr.asm
;
; Description:  Assembly module for Cross Correlation
;
; Modules
;    Included:  Fdfr16Corr
;
; Author(s):    Alwin Anbu.D
;
; Date:         29 Nov 2001
;
;**********************************************************************

    include  "portasm.h"

    SECTION  rtlib

    GLOBAL   Fdfr16Corr

    org      p:

;**********************************************************************
;
; Module Name:  Fdfr16Corr
;
; Description:  This routine computes the cross corelation of two
;               input vectors having fractional data values.
;
; Input(s):     1. options - Selects between raw, biased, and
;                            unbiased cross correlation
;               2. pX      - Pointer to the first input vector
;               3. pY      - Pointer to the second input vector
;               4. pZ      - Pointer to output vector
;               5. nX      - Length of the first input vector
;               6. nY      - Length of the second input vector
;
; Output(s):
;               FAIL(-1) - if an invalid parameter is passed
;               PASS(0)  - if no invalid parameters are passed
;
; Functions
;     Called:   None
;
; Calling
; Requirements: 1. r2 - Pointer to first input vector pX
;               2. r3 - Pointer to second input vector pZ
;               3. y0 - Options
;               4. y1 - Length of first input vector
;               5. a  - Length of second input vector
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
; DO loops:     2 nested
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
;     Issues:   1. The second vinput vector is taken as the reference
;                  vector.
;               2. DO Loop Nesting Level is 2. Hence the calling fuction
;                  should take care if needs to be called inside a DO
;                  Loop
;
;******************************Change History***************************
;
;    DD/MM/YY     Code Ver      Description     Author(s)
;    --------     --------     ------------     ---------
;   23-11-2001      0.1        Module created   Alwin Anbu.D
;   27-11-2001      1.0        Baselined        Alwin Anbu.D
;
;**********************************************************************

    DEFINE    nX      'x:(SP-1)'
    DEFINE    old_omr 'x:(SP)'

Fdfr16Corr:
    adda      #2,sp
    move.l    c2,x:(sp)+
    move.l    c10,x:(sp)+
    move.l    d2,x:(sp)+
    move.l    d10,x:(sp)+
    move.l    r5,x:(sp)+

    move.w    omr,old_omr
    move.w    a,d                           ; d=nY
    add       y1,d                          ; d=nY+nX
    dec.w     d                             ; d=nY+nX-1

    bfset     #$300,omr                     ; Enable 32 bit mode
    cmp.w     #PORT_MAX_VECTOR_LEN,d        ; Check if nY+nX-1>65535
    bhi       AcorrFail                     ; If yes,Jump to fail
    cmp.w     #2,y0                         ; Check if option given
                                            ;  is valid
    ble       Length_Ok                     ; If not,Jump  to fail

AcorrFail:
    move.w    #-1,y0                        ; If Fail,return -1(FAIL)
    jmp       Endcorr

Length_Ok:
    bfclr     #$100,omr                     ; Disable 32 Bit mode
    move.w    y1,nX                         ; Move y1 to stack
    tfra      r2,r0                         ; r0 points to the input
                                            ;  vector
    tfra      r3,r1                         ; r1 points to the output
                                            ;  vector
RAW:
    tst.w     y0                            ; Check if y0=0
    bne       BIAS
    move.w    #$7fff,b0                     ; Set Bias=$7fff
    bra       Loop1

BIAS:
    cmp.w     #1,y0                         ; Check if y0=1
    bne       Loop1
    move.w    #1,b                          ; --
    bfclr     #$0001,sr                     ;   |- b0=1/(nX+nY-1)
    rep       #16                           ;   |
    div       d1,b                          ; --

Loop1:
    bfclr     #$10,omr                      ; Disable Saturation mode
    moveu.w   y1,n                          ; j=nX
    move.w    b0,a0                         ; Move bias to a0
    move.w    #1,c1                         ; c1=1
    deca      n                             ; j=nX-1
    move.w    n,x0                          ; x0=nX-1
    sub       x0,d                          ; d=(nX+nY-1)-(nX-1)

  if CODEWARRIOR_WORKAROUND==1
    do        y1,>>End_Loop1                ; Loop Count is nX
    else
    do        y1,End_Loop1                  ; Loop Count is nX
    endif

    cmp.w     #2,y0                         ; Check if options is 2
    bne       Options01                     ; If not,Jump to Options01
    move.w    #1,b                          ; --
    bfclr     #$0001,sr                     ;   |
    rep       #16                           ;   |- b0=1/[(nX+nY-1)-j]
    div       d1,b                          ; --
    inc.w     d                             ; d1= (nX+nY-1)-j
    move.w    b0,a0

Options01:
    adda      n,r0                          ; r0=r0+j
    clr       b            x:(r3)+,x0

  if CODEWARRIOR_WORKAROUND==1
    do        c1,>>End_loop2
    else
    do        c1,End_loop2
    endif

    move.w    x:(r0)+,y1
    mac       x0,y1,b      x:(r3)+,x0

End_loop2:
    rnd       b
    cmp.w     c,a                           ; Compare counter with nY
    ble       No_Increment                  ; If nY <= counter,
                                            ;  no increment
    inc.w     c                             ; Otherwise counter is
                                            ;  incremented by 1
No_Increment
    tfra      r2,r0                         ; r0 is loaded again with
                                            ;  r2 (pX)
    tfra      r1,r3                         ; r3 is loaded with
                                            ;  r1 (pY)
    deca      n                             ; j=j-1
    move.w    a0,x0
    move.w    b,b1
    mpyr      b1,x0,b                       ; Multiply with bias
    move.w    b,x:(r4)+                     ; Move result to output
                                            ;  array
End_Loop1:

Next_Loop:
    move.w    nX,y1                         ; y1=nX

RAW1:
    tst.w     y0                            ; Check if y0=0
    bne       UNBIAS1
    move.w    #$7fff,b0                     ; If yes,set bias=7fff
    bra       Loop12

UNBIAS1:
    cmp.w     #1,y0                         ; Check if y0=1
    bne       Loop12
    add       y1,d                          ; d1=(nX+nY-1)+1
    dec.w     d                             ; d1=nX+nY-1
    move.w    #1,b                          ; --
    bfclr     #$0001,sr                     ;   |
    rep       #16                           ;   |-b0=1/(nX+nY-1)
    div       d1,b                          ; --

Loop12
    move.w    #1,n                          ; Counter=1
    sub.w     #2,d                          ; d = nX+nY-1-1
                                            ;  (d used for CORR_BIAS)
    dec.w     a                             ; nY=nY-1
    move.w    y1,c1                         ; c1=nX
    moveu.w   a1,lc                         ; lc=nY-1

    if CODEWARRIOR_WORKAROUND==1
    doslc     >>End_OuterDo
    else
    doslc     End_OuterDo
    endif

    cmp.w     #2,y0                         ; Check if y0=2
    bne       Options01_Next                ; If not,jump to
                                            ;  Options01_Next
    move.w    #1,b                          ; --
    bfclr     #$0001,sr                     ;   |
    rep       #16                           ;   |- b0= (1/(nX+nY-1-j)]
    div       d1,b                          ; --
    dec.w     d
    move.w    b0,a0                         ; Move bias to a0

Options01_Next:
    adda      n,r3                          ; r3 = r3+n
    moveu.w   a,r5                          ; r5(No.of multiplications)=k
    cmp.w     c,a                           ; Compare nX with k
    ble       No_Change
    moveu.w   c1,r5                         ; If nX<k , r5 = nX

No_Change:
    clr       b            x:(r3)+,x0

  if CODEWARRIOR_WORKAROUND==1
    do        r5,>>End_InnerDo
    else
    do        r5,End_InnerDo
    endif

    move.w    x:(r0)+,y1
    mac       y1,x0,b       x:(r3)+,x0

End_InnerDo:
    rnd       b
    dec.w     a                             ; k=k-1
    tfra      r2,r0                         ; Reload r0 with pX
    tfra      r1,r3                         ; Reload r3 with pY
    adda      #1,n                          ; Increment counter
    move.w    a0,x0
    move.w    b,b1
    mpyr      b1,x0,b                       ; Multiply with bias
    move.w    b,x:(r4)+                     ; Store the result

End_OuterDo:

Endcorr:
    moveu.w   old_omr,omr                   ; Restore the OMR from
                                            ;  the stack

    suba      #2,sp                         ; Restore the stack Pointer
    move.l    x:(sp)-,r5
    move.l    x:(sp)-,d
    move.l    x:(sp)-,d2
    move.l    x:(sp)-,c
    move.l    x:(sp)-,c2

    rts

    ENDSEC

;************************* End of file ********************************
