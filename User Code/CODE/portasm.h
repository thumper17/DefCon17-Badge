;/** ###################################################################
;**
;**     (c) Freescale Semiconductor
;**     2004 All Rights Reserved
;**
;**
;**     (c) Copyright UNIS, a.s. 1997-2008
;**     UNIS, a.s.
;**     Jundrovska 33
;**     624 00 Brno
;**     Czech Republic
;**     http      : www.processorexpert.com
;**     mail      : info@processorexpert.com
;** ###################################################################*/

; File: port.asm

;/*******************************************************
;* Conditional assembly
;*******************************************************/

;/* Change the following define to '0' to eliminate asserts */
  define  ASSERT_ON_INVALID_PARAMETER   '1'

;/* For V1 compatibility */
   define debug    'debughlt'

;/*
;   These defines permit the same ASM source code to be
;   used for Large and Small Memory Models
;*/
    define PORT_LARGE_MEMORY_MODEL '1'

    define CODEWARRIOR_WORKAROUND '0'

;   IF @DEF(F__CW_M56800E_LMM)
;   Small Memory Model defines

   define LoadRx   'moveu.w'
   define StoreRx  'move.w'
   define TestRx   'tst.w'

   define PTR_SIZE  '1'

;  endif

;/*******************************************************
;* Constants
;*******************************************************/

;/* Function Result Values */
PASS      equ     0
FAIL      equ     -1

true      equ     1
false     equ     0

;/*******************************************************
;* Implementation Limits
;*******************************************************/

PORT_MAX_VECTOR_LEN  equ 65535

