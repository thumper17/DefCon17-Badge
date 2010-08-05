;=========================================================================
;           FREESCALE CONFIDENTIAL PROPRIETARY
;
;        (C) 2000 (c) Freescale Semiconductor
;=========================================================================

;=========================================================================
; Revision History:
;
; VERSION    CREATED BY    MODIFIED BY      DATE       COMMENTS
; -------    ----------    -----------      -----      --------
;   0.1      Meera S. P.        -          02-03-2000  Reviewed.
;=========================================================================

;=========================================================================
;asm Result dfr16IIR ( tIirStruct * pIIR,
;                      Frac16     * pX,
;                      Frac16     * pZ,
;                      UInt16       n)
;{
; See C implementation in dfr16.c for pseudo code model
;
; Register utilization upon entry:
;    R2    - pIIR
;    R3    - pX
;    Y0    - n
;    R4    - pZ
;
;  Register utilization during execution:
;     R0    - Pointer to filter states
;     R1    - Pointer to input buffer
;     R2    - Pointer to pIIR
;     R3    - Pointer to filter coefficients
;     Y0    - Length of input buffer
;     X0,Y1 - temp registers for intermediate calculations
;     A     - Acc
;     B1    - pIIR -> nbiq
;     R4    - pZ
;=========================================================================

  SECTION rtlib

  include "portasm.h"

  GLOBAL  Fdfr16IIR
;
; Define tFirStruct offsets
;
Offset_pC                equ 0
Offset_pHistory          equ Offset_pC+PTR_SIZE
Offset_pNextHistoryAddr  equ Offset_pHistory+PTR_SIZE
Offset_nbiq              equ Offset_pNextHistoryAddr+PTR_SIZE
Offset_bCanUseModAddr    equ Offset_nbiq+1
Offset_bCanUseDualMAC    equ Offset_bCanUseModAddr+1

    DEFINE    old_omr 'x:(SP)'


Fdfr16IIR:
  cmp.w     #PORT_MAX_VECTOR_LEN,Y0             ; Q: n <= 8191?
  bls       LengthOK
IIR_Fail:
  move.w    #FAIL,Y0                            ; n > 8191 so return FAIL
  rts

LengthOK:
  cmp.w     #0,Y0                               ; Ensure n > 0
  ble       IIR_Fail                            ; Return FAIL, if length is negative
    adda      #2,SP                               ; Save c2, c1 and c0 regs.
    move.l    c2,x:(sp)+
    move.l    c10,x:(sp)+

    move.w    omr,old_omr

    bfclr     #$10,OMR                            ; set saturation mode off
    move.w    X:(R2+Offset_nbiq),B                ; set B1 to pIIR->nbiq
    asl       B                                   ; B = nbiq * 2
    tfra      R3,R1                               ; R1 = *pX (pointer to the input buffer)
  LoadRx    X:(R2+Offset_pHistory),R0           ; R0 = *pHistory (pointer to the filter states buffer)
  tst.w     X:(R2+Offset_bCanUseDualMAC)        ; Q: Can dual parallel moves be used?
  beq  TryJustModulo
;
;  Case 1: Dual MAC code
;
  dec.w     B                                   ; Set M01 for modulo addr
  moveu.w   B1,M01                              ; M01 = (nbiq*2 -1)
  move.w    X:(R0)+,Y1                          ; Y1 = state(w1)
	
  if CODEWARRIOR_WORKAROUND==1
  do        Y0,>>_endl1                         ; for loop (for no. of inputs samples)
  else
  do        Y0,_endl1                           ; for loop (for no. of inputs samples)
  endif

  LoadRx    X:(R2+Offset_pC),R3                 ; R3 - points to filter coeff. buffer
  moveu.w   X:(R2+Offset_nbiq),N                ; N = No. of biquads
  move.w    x:(R1)+,A                           ; A = input sample
  move.w    X:(R3)+,X0                          ; X0 = filter coef(a1)
;=====================================================================================
; Here  the power of modulo is not utilized; however we are saving some cycles by
; re-initializing the States pointer and fetching the first state in parallel with
; first coefficient. By doing this we can avoid following four statements, which will
; require more cycles than re-initializing states pointer.
;=====================================================================================

;  move    X:(SP),B1                             ; store loop count in B1
;  cmp     B1,Y0                                 ; compare current loop count with total loop count
;  bne     _start_loop                           ; for first loop fetch history data, then modulo will take care of it
;  move    X:(R0)+,Y1                   	

_start_loop:
  if CODEWARRIOR_WORKAROUND==1
  do        N,>>_endl2                          ; for loop no. of biquad times
  else
  do        N,_endl2                            ; for loop no. of biquad times
    endif

  mac       X0,Y1,A    X:(R0)+,X0               ; a1 * w1 + A and X0 = filter state(w2)
  move.w    X:(R3)+,Y0                          ; Y0 = filter coef(a2)
  macr      Y0,X0,A    X:(R3)+,Y0               ; round(a2 * w2 + A) and  Y0 = filter coef(b0)

;
; LS010807 -- changed next two instructions to V2 instructions to
;             circumvent suspected V2 core problem with modulo addressing
;             of negative offsets
;
  move.w    A,X:(R0-2)                          ; move w0 to nextHistoryAdd
  move.w    Y1,X:(R0-1)                         ; move w1 to nextHistoryAdd

  mpy       A1,Y0,A    X:(R3)+,Y0               ; b0 * w0 and Y0 = filter coef(b1)
  mac       Y0,Y1,A    X:(R3)+,Y0               ; b1 * w1 + A and Y0 = filter coef(b2)
  macr      X0,Y0,A    X:(R0)+,Y1 X:(R3)+,X0    ; round(b2 * w2 + A), for next biquad: Y1 = filter state(w1) 
                                                                                 ; and X0 = filter coef(a1)
_endl2
    move.w    A,X:(R4)+                         ; move output to *pZ
_endl1
ExitIIR:
  moveu.w   #-1,M01                             ; Restore M01 reg
  move.w    #PASS,Y0                            ; return PASS

    moveu.w  old_omr,omr                          ; Restore the OMR from

    suba     #2,sp                                ; Restore the stack Pointer
    move.l    x:(sp)-,c
    move.l    x:(sp)-,c2


  rts

TryJustModulo:
  tst.w     x:(R2+Offset_bCanUseModAddr)        ; Q: Can modulo addressing be used?
  beq       TheHardWay
;
;  Case 2: Use modulo addressing for partial optimization
;
  dec.w     B                                   ; Set M01 for modulo addr
  moveu.w   B1,M01
  move.w    Y0,B1
TryModLoop:
  LoadRx    X:(R2+Offset_pC),R3                 ; R3 - points to filter coeff. buffer
  moveu.w   X:(R2+Offset_nbiq),N                ; N = No. of biquads
  move.w    x:(R1)+,A                           ; A = input sample
  move.w    X:(R3)+,X0                          ; X0 = filter coef(a1)

  if CODEWARRIOR_WORKAROUND==1
  do        N,>>_endModDo
  else
  do        N,_endModDo
    endif

  move.w    X:(R0)+,Y1                          ; Y1 = filter state(w1)
  mac       X0,Y1,A    X:(R3)+,Y0               ; A = a1 * w1 + A and Y1 = coef(a2)
  move.w    X:(R0)-,X0                          ; X0 = w2
  macr      X0,Y0,A    X:(R3)+,Y0               ; round(A = a2 * w2 + A) and Y0 = coef(b0)

  move.w    A1,X:(R0)+                          ; move w0 to nextHistoryAdd
  move.w    Y1,X:(R0)+                          ; move w1 to nextHistoryAdd
  
  mpy       A1,Y0,A    X:(R3)+,Y0               ; b0 * w0 and Y0 = coef(b1)
  mac       Y0,Y1,A    X:(R3)+,Y0               ; A = b1 * w1 + A and Y0 = coef(b2)
  macr      X0,Y0,A    X:(R3)+,X0               ; round(A = b2 * w2 + A) and X0 = coef(a1) - next biquad 
_endModDo
  move.w    A,X:(R4)+                           ; *pZ = output value
  dec.w     B                                   ; (No. of input sample)--
  bgt  TryModLoop                               ; check for end of loop

 bra ExitIIR

;
;  Case 3: Cannot use modulo addressing or dual-parallel MAC
;
TheHardWay:
  moveu.w   #-1,M01                             ; Set M01 for linear addressing
    move.w    Y0,B1                             ; No. of input samples
HardWayLoop:
  LoadRx    X:(R2+Offset_pC),R3                 ; R3 = points to filter coeff. buffer
  LoadRx    X:(R2+Offset_pHistory),R0           ; R0 = pointer to filter state buffer
  move.w    X:(R2+Offset_nbiq),X0               ; X0 = No. of biquads
  move.w    x:(R1)+,A                           ; A  = input sample
  move.w    X:(R0)+,Y1                          ; Y1 = filter state(w1)

  if CODEWARRIOR_WORKAROUND==1
  do        X0,>>_endHardDo
  else
  do        X0,_endHardDo
  endif

  move.w    X:(R3)+,X0                          ; X0 = filter coef(a1)
  mac       X0,Y1,A    X:(R3)+,Y0               ; a1 * w1 and Y0 = filter coef(a2)
  move.w    X:(R0)-,X0                          ; X0 = filter filter state(w2)
  macr      X0,Y0,A    X:(R3)+,Y0               ; a2 * w2 and Y0 = filter coef(b0)
   
  move.w    A1,X:(R0)+                          ; move w0 to nextHistoryAdd
  move.w    Y1,X:(R0)+                          ; move w1 to nextHistoryAdd

  mpy       A1,Y0,A    X:(R3)+,Y0               ; b0 * w0 and Y0 = filter coef(b1)
  mac       Y0,Y1,A    X:(R3)+,Y0               ; b1 * w1 and Y0 = filter coef(b2)
  macr      X0,Y0,A    X:(R0)+,Y1               ; b2 * w2 and Y1 = filter state(w1) - next biquad
_endHardDo
  move.w    A,X:(R4)+                           ; *pZ = output
  dec.w     B                                   ; (No. of input sample)--
  bgt  HardWayLoop                              ; check for end of loop

  jmp       ExitIIR

  ENDSEC
