/** ###################################################################
**     THIS BEAN MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : Vectors.C
**     Project   : DC17
**     Processor : MC56F8006_32_LQFP
**     Version   : Bean 01.013, Driver 02.01, CPU db: 3.00.169
**     Compiler  : Metrowerks DSP C Compiler
**     Date/Time : 4/7/2009, 10:11 AM
**     Abstract  :
**
**     Comment   :
**         If you are hacking the DC17 firmware, you will need to do the following things:
**         
**         1) After re-compiling, open Vector.c (in the Generated Code project folder) and change first two lines of _vect table from _EntryPoint to BOOTLOADER_ADDR. 
**         This ensures that the bootloader will re-launch at power-up, else you are stuck with your user code unless you re-flash the whole device. 
**         
**         2) If the linker file (file ending in .cmd) changes, patch the following areas:
**         
**         a) Paste this code block to the end of the SECTIONS area
**         
**         .ApplicationConfiguration :
**         {
**         # Set the start address of the application
**         WRITEH(F_EntryPoint);
**         # Bootloader start delay in seconds
**         WRITEH(10);
**         } > .xBootCfg
**         
**         b) Add 
**              * (.const.data.pmem)
**         underneath  
**              * (.data.pmem) 
**         in the .ApplicationCode section to enable storage of constants within program area of flash
**         
**         c) Add 
**              F_vba = .;
**         underneath
**              # interrupt vectors
**         
**         ---
**     Settings  :
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

#include "Cpu.h"
#include "PWMRed.h"
#include "PWMGreen.h"
#include "PWMBlue.h"
#include "MICOUT.h"
#include "MICPWR.h"
#include "TI0.h"
#include "TI1.h"
#include "TIPIT.h"
#include "DFR1.h"
#include "MEM1.h"
#include "BADGEI2C.h"
#include "A0.h"
#include "A1.h"
#include "A2.h"
#include "Term1.h"
#include "Inhr6.h"
#include "PORTAOUT.h"
#include "PORTB1OUT.h"
#include "PORTB5OUT.h"
#include "PORTCIN.h"
#include "PORTCOUT.h"
#include "PORTFOUT.h"
#include "Events.h"

/*lint -save -e40 -e718 -e746 -e745 -e939 -e937 -e957 -e505 -e522 -e533 Disable MISRA rule (20,75,71,53,83) checking. */

extern void _EntryPoint(void);         /* Startup routine */

volatile asm void _vectboot(void);
#pragma define_section interrupt_vectorsboot "interrupt_vectorsboot.text"  RX
#pragma section interrupt_vectorsboot begin
volatile asm void _vectboot(void) {
  JMP  _EntryPoint                     /* Reset vector (Used) */
  JMP  _EntryPoint                     /* COP reset vector (Used) */
}
#pragma section interrupt_vectorsboot end

volatile asm void _vect(void);
#pragma define_section interrupt_vectors "interrupt_vectors.text"  RX
#pragma section interrupt_vectors begin
volatile asm void _vect(void) {
  JMP  BOOTLOADER_ADDR                     /* Interrupt no. 0 (Used)   - ivINT_Reset */
  JMP  BOOTLOADER_ADDR                     /* Interrupt no. 1 (Used)   - ivINT_COPReset  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 2 (Unused) - ivINT_Illegal_Instruction  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 3 (Unused) - ivINT_HWStackOverflow  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 4 (Unused) - ivINT_MisalignedLongWordAccess  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 5 (Unused) - ivINT_OnCE_StepCounter  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 6 (Unused) - ivINT_OnCE_BreakpointUnit  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 7 (Unused) - ivINT_OnCE_TraceBuffer  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 8 (Unused) - ivINT_OnCE_TxREmpty  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 9 (Unused) - ivINT_OnCE_RxRFull  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 10 (Unused) - ivINT_PMC_OutOfReg_LVI  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 11 (Unused) - ivINT_PLL_LossOfLock_LossOfRefClk  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 12 (Unused) - ivINT_ADC0_Complete  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 13 (Unused) - ivINT_ADC1_Complete  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 14 (Unused) - ivINT_PWM_Reload_Fault  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 15 (Unused) - ivINT_CMP0_FlagRisingFalling  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 16 (Unused) - ivINT_CMP1_FlagRisingFalling  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 17 (Unused) - ivINT_CMP2_FlagRisingFalling  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 18 (Unused) - ivINT_HFM_ERR_CC_CBE  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 19 (Unused) - ivINT_SPI_Rx_Full_Over_ModeFault  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 20 (Unused) - ivINT_SPI_Tx_Empty  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 21 (Unused) - ivINT_SCI_Tx_Empty_Idle  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 22 (Unused) - ivINT_SCI_Rx_Full_Over_Error  */
  JSR  BADGEI2C_Interrupt              /* Interrupt no. 23 (Used)   - ivINT_I2C  */
  JSR  TIPIT_Interrupt                 /* Interrupt no. 24 (Used)   - ivINT_PIT  */
  JSR  TI0_Interrupt                   /* Interrupt no. 25 (Used)   - ivINT_TMR0  */
  JSR  TI1_Interrupt                   /* Interrupt no. 26 (Used)   - ivINT_TMR1  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 27 (Unused) - ivINT_GPIO_A  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 28 (Unused) - ivINT_GPIO_B  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 29 (Unused) - ivINT_GPIO_C  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 30 (Unused) - ivINT_GPIO_D  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 31 (Unused) - ivINT_GPIO_E  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 32 (Unused) - ivINT_GPIO_F  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 33 (Unused) - ivINT_RTC  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 34 (Unused) - ivReserved0  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 35 (Unused) - ivReserved1  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 36 (Unused) - ivReserved2  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 37 (Unused) - ivReserved3  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 38 (Unused) - ivReserved4  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 39 (Unused) - ivReserved5  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 40 (Unused) - ivINT_SW0  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 41 (Unused) - ivINT_SW1  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 42 (Unused) - ivINT_SW2  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 43 (Unused) - ivINT_SW3  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 44 (Unused) - ivINT_LP  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 45 (Unused) - ivINT_USER1  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 46 (Unused) - ivINT_USER2  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 47 (Unused) - ivINT_USER3  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 48 (Unused) - ivINT_USER4  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 49 (Unused) - ivINT_USER5  */
  JSR  Cpu_Interrupt                   /* Interrupt no. 50 (Unused) - ivINT_USER6  */
}
#pragma section interrupt_vectors end

/*lint -restore */
/*
** ###################################################################
**
**     This file was created by UNIS Processor Expert 2.99 [04.17]
**     for the Freescale 56800 series of microcontrollers.
**
** ###################################################################
*/
