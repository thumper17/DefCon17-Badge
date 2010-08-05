;********************************************************************
;
; (c) Freescale Semiconductor
; 2004 All Rights Reserved
;
;
;********************************************************************
;
; File Name:    dfr16bitrev.asm
;
; Description:  Assembly module for Bit Reverse
;
; Modules
;    Included:  Fdfr16Cbitrev_
;
; Author(s):    Sandeep S
;               Alwin Anbu.D
;
; Date:         3 Dec 2001
;
;********************************************************************

        SECTION rtlib

    include "portasm.h"

    GLOBAL  Fdfr16Cbitrev_

;********************************************************************
;
; Module Name:  Fdfr16Cbitrev_
;
; Description:  Bit Reverses the Input Array
;
; Functions
;      Called:  None
;
; Calling
; Requirements: 1. r2 - Pointer to Input Buffer.
;               2. r3 - Pointer to Output Buffer.
;               3. y0 - Length of the input/output buffer
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
;      Changed: All except r1 and r5
;
; DO loops:     1
;
; REP loops:    None
;
; Environment:  MetroWerks on PC
;
; Special
;     Issues:   1.r2 and r3 MUST have even boundaries
;
;******************************Change History************************
;
;    DD/MM/YY     Code Ver     Description      Author(s)
;    --------     --------     -----------      ------
;    18/01/2001   0.1          Module created   Sandeep S
;    18/01/2001   1.0          Baselined        Sandeep S
;    30/11/2001   1.1          Modified         Alwin Anbu.D
;
;********************************************************************

Fdfr16Cbitrev_

    adda      #2,sp
    move.l    c2,x:(sp)+
    move.l    c10,x:(sp)+
    move.l    d2,x:(sp)+
    move.l    d10,x:(sp)

    clr.w     d                           ; d is the normal index
    move.w    #0,x0                       ; x0 is the bit reversed index
    tfra      r2,r0                       ; r0 points to input
    tfra      r3,r4                       ; r4 points to output
    move.w    y0,c
    asr       c                           ; a1=(No.of points)/2

    dec.w     y0                          ; y0=n-1

    if CODEWARRIOR_WORKAROUND==1
    do        y0,>>End_Do
    else
    do        y0,End_Do
    endif

    move.w    x:(r0)+,a0                  ; Move real part to a0
    move.w    x:(r0)+,a1                  ; Move imaginary part to a1
    cmp.w     x0,d                        ; Check if d < x0
    blt       elsepart                    ; if yes,bit reversal
                                          ; already done.Jump to elsepart
    moveu.w   d,n                         ; Move bit reversed index to n
    asla      n
    move.w    x:(r2+n),b0                 ; Move real parts at bit
                                          ; reversed locations to
                                          ; normal location
    adda      #1,n
    move.w    x:(r2+n),b1                 ; Move imaginary parts at bit
                                          ; reversed locations to
                                          ; normal location
    move.w    a1,x:(r4+n)
    adda      #-1,n
    move.w    a0,x:(r4+n)

    move.w    b0,x:(r3)+                  ; Move imaginary part
    move.w    b1,x:(r3)-                  ; Move imaginary part at bit
                                          ; reversed location to
                                          ; normal location
elsepart

    move.w    c1,y0                       ; y0=N/2
    cmp       y0,d                        ; Check if d < N/2 .Update r3
    move.w    x:(r3)+,y1
    blt       skip_change                 ; If yes,skip change

chk_again
    sub       y0,d                        ; d=d-y0
    asr       y0                          ; y0=y0/2
    cmp.w     y0,d                        ; Check if d>=y0
    bge       chk_again                   ; If yes,check again

skip_change

    add       y0,d                        ; Update r3
    move.w    x:(r3)+,y1
    inc.w     x0                          ; Increment normal index
End_Do

    move.w    x:(r0)+,a0                  ; Move last pair to a
    move.w    x:(r0)-,a1
    move.w    a0,x:(r3)+                  ; Move a to last output location
    move.w    a1,x:(r3)-
    move.l    x:(sp)-,d
    move.l    x:(sp)-,d2
    move.l    x:(sp)-,c
    move.l    x:(sp)-,c2

    rts

    ENDSEC

;************************* End of file ********************************
