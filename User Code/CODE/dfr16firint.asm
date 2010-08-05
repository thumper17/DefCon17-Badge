;********************************************************************
;
; (c) Freescale Semiconductor
; 2004 All Rights Reserved
;
;********************************************************************
  SECTION rtlib

  include "portasm.h"

  GLOBAL  Fdfr16FIRInt

;
; The following symbols can be used to exclude portions (using '0') of
; the FIR implementation in order to save program memory;  if you do so,
; however, please make sure that EVERY call to FIR will satisfy the
; constraints placed upon the limited implementation.
;
  define  FIRINT_USE_DUAL_MAC_OPT   '1'
  define  FIRINT_USE_MODULO_OPT     '1'
  define  FIRINT_USE_NO_OPT         '1'
;
; Define dfr16_tFirIntStruct offsets
;
Offset_pC                equ 0
Offset_pHistory          equ (Offset_pC+PTR_SIZE)
Offset_pNextHistoryAddr  equ (Offset_pHistory+PTR_SIZE)
Offset_n                 equ (Offset_pNextHistoryAddr+PTR_SIZE)
Offset_bCanUseModAddr    equ (Offset_n+1)
Offset_bCanUseDualMAC    equ (Offset_bCanUseModAddr+1)
Offset_Factor            equ (Offset_bCanUseDualMAC+1)
Offset_Count             equ (Offset_Factor+1)



;asm UInt16 dfr16FIRInt (dfr16_tFirIntStruct *pFIRINT,
;                        Frac16              *pX,
;                        Frac16              *pZ,
;                        UInt16               n);
;{
; See C implementation in dfr16.c for pseudo code model
;
; Register utilization upon entry:
;    R2    - pFIRINT
;    R3    - pX
;    Y0    - n
;    R4    - pZ
;
; Register utilization upon entry:
;    Y0    - (n * pFirInt->Factor) = Number of outputs
;
;  Register utilization during execution:
;     R0      - pMem
;     R1      - pX
;     R2      - pFIR
;     R3      - pCoefs
;     R4      - pZ
;     Y0,N3   - n
;     X0,Y1   - temp regs for MAC
;     A       - total32
;     B1      - pFirInt->Count
;     N, D    - pFirInt->Factor
;     C       - temp;  modulo count in Case 3
;     X:(SP)  - pX temporary

Fdfr16FIRInt:

  moveu.w  Y0,N3                               ; Save n in N3 for retrieval
  LoadRx   x:(R2+Offset_pNextHistoryAddr),R0   ; Set R0 to pMem
  cmp.w    #0,Y0                               ; Ensure n > 0
  jle      ExitFIRInt

  tfra     R3,R1                               ; move pX to R1
  move.w   x:(R2+Offset_Count),B               ; Set B1 to pFirInt->Count

 if FIRINT_USE_DUAL_MAC_OPT==1

  tst.w    x:(R2+Offset_bCanUseDualMAC)        ; Q: Dual MAC be used?
  beq      TryJustModulo
;
;  Case 1: Dual MAC code
;
  dec.w    B                                   ; Set M01 for modulo addr
  moveu.w  B1,M01

  if CODEWARRIOR_WORKAROUND==1
  do       Y0,>>endloopA                       ; for loop (n times)
  else
  do       Y0,endloopA                         ; for loop (n times)
  endif

  move.w   X:(R1)+,A0                          ; Move input value to history buf
  move.w   A0,X:(R0)+
  LoadRx   X:(R2+Offset_pC),R3                 ; pCoefs = pFIRINT->pC
  move.w   X:(R2+Offset_Factor),X0

  if CODEWARRIOR_WORKAROUND==1
  do       X0,>>endloop
  else
  do       X0,endloop
  endif

  clr      A                                   ; total32 = 0
  move.w   X:(R0)+,Y1   X:(R3)+,X0             ; Pre-load mac registers
  rep      B1                                  ; Rep MAC pFirInt->Count-1 times
  mac      Y1,X0,A   X:(R0)+,Y1   X:(R3)+,X0
  macr     Y1,X0,A                             ; Last MAC to get addresses right
                                                 ; round(A)
  move.w   A,X:(R4)+                           ; *pZ++ = round(A)
endloop:
  nop
endloopA:
  bra      ExitFIRInt

  endif

TryJustModulo:

  if FIRINT_USE_MODULO_OPT==1

  tst.w    x:(R2+Offset_bCanUseModAddr)        ; Q: Can modulo addressing be used?
  beq      TheHardWay
;
;  Case 2: Use modulo addressing for partial optimization
;
  dec.w    B                                   ; Set M01 for modulo addr
  moveu.w  B1,M01
  nop
  nop
TryModLoop:
  move.w   X:(R1)+,A0                          ; Move input value to history buf
  move.w   A0,X:(R0)+
  LoadRx   X:(R2+Offset_pC),R3                 ; pCoefs = pFIRINT->pC
  move.w   X:(R2+Offset_Factor),X0

  if CODEWARRIOR_WORKAROUND==1
  do       X0,>>endloop2A                      ; do X0,endloop2
  else
  do       X0,endloop2A                        ; do X0,endloop2
  endif

  clr      A            X:(R0)+,Y1             ; total32 = 0
  move.w   X:(R3)+,X0                          ; Pre-load mac registers

  if CODEWARRIOR_WORKAROUND==1
  do       B1,>>EndModDo                       ; Rep MAC pFirInt->Count-1 times
  else
  do       B1,EndModDo                         ; Rep MAC pFirInt->Count-1 times
  endif

  mac      Y1,X0,A   X:(R0)+,Y1
  move.w   X:(R3)+,X0
EndModDo:
  macr     Y1,X0,A                             ; Last MAC to get addresses right
                                                 ; round(A)
  move.w   A,X:(R4)+                           ; *pZ++ = round(A)
endloop2:
endloop2A:

  dec.w    Y0                                  ; n -= 1
  bgt      TryModLoop

;  bra      ExitFIRInt                          ; Fall thru
  else
  bra      TheHardWay
  endif

ExitFIRInt:
  StoreRx  R0,X:(R2+Offset_pNextHistoryAddr)   ; Preserve history buf pointer

  move.w   X:(R2+Offset_Factor),X0             ; return n*factor
  move.w   N3,Y0
  impy.w   X0,Y0,A
  move.w   A1,Y0

  moveu.w  #-1,M01                             ; Restore M01 reg due to CW bug
  rts


;
;  Case 3: Cannot use modulo addressing or dual-parallel MAC
;
TheHardWay:
  if FIRINT_USE_NO_OPT==1

  adda     #2,SP                               ; Save permanent C & D registers
  move.l   C2,x:(SP)+
  move.l   C10,x:(SP)

  moveu.w  #-1,M01                             ; Set M01 for linear addressing
HardWayLoop:
  LoadRx   X:(R2+Offset_pHistory),R3           ; Calculate length before buffer wrap
  move.l   R3,C                                ; The calculation is done using longs
  clr      A                                   ; in order to support Large Memory Model
  move.w   B1,A0
  add      A,C
  move.l   R0,A
  sub      A,C
  tfr      C,A
  move.w   A0,C

  move.w   X:(R1)+,A0                          ; Move input value to history buf
  move.w   A0,X:(R0)+
  dec.w    C                                   ; Q: wrapped
  bgt      HardWay1
  move.w   B,C
  LoadRx   X:(R2+Offset_pHistory),R0
HardWay1:
  LoadRx   X:(R2+Offset_pC),R3                 ; pCoefs = pFIRINT->pC
  move.w   X:(R2+Offset_Factor),X0

  if CODEWARRIOR_WORKAROUND==1
  do       X0,>>endloop3
  else
  do       X0,endloop3
  endif

  clr      A            X:(R0)+,Y1             ; total32 = 0
                                               ; Pre-load mac registers
  dec.w    C                                   ; Q: wrapped
  bgt      HardWay2
  move.w   B,C
  LoadRx   X:(R2+Offset_pHistory),R0
HardWay2:
  move.w   X:(R3)+,X0
  dec.w    B                                   ; Subtract one from pFirInt->Count

  if CODEWARRIOR_WORKAROUND==1
  do       B1,>>EndHardDo                      ; Rep MAC pFirInt->Count-1 times
  else
  do       B1,EndHardDo                        ; Rep MAC pFirInt->Count-1 times
  endif

  mac      Y1,X0,A   X:(R0)+,Y1
  dec.w    C                                   ; Q: wrapped
  nop
  bgt      HardWay3
  move.w   B,C                                 ; restore Count of coefficients
  inc.w    C                                   ; add 1 to restore pFirInt->Count
  LoadRx   X:(R2+Offset_pHistory),R0
HardWay3:
  move.w   X:(R3)+,X0
  nop
  nop
EndHardDo:
  inc.w    B                                   ; restore B to pFirInt->Count
  macr     Y1,X0,A                             ; Last MAC to get addresses right
                                                 ; round(A)
  move.w   A,X:(R4)+                           ; *pZ++ = round(A)

endloop3:

    dec.w    Y0                                  ; n -= 1
    bgt      HardWayLoop

  move.l   x:(SP)-,C                         ; Restore permanent C & D registers
  move.l   x:(SP)-,C2


  endif

    bra      >ExitFIRInt

  ENDSEC
