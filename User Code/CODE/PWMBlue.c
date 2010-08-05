/** ###################################################################
**     THIS BEAN MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : PWMBlue.C
**     Project   : DC17
**     Processor : MC56F8006_32_LQFP
**     Beantype  : PWM
**     Version   : Bean 02.207, Driver 02.00, CPU db: 3.00.169
**     Compiler  : Metrowerks DSP C Compiler
**     Date/Time : 3/28/2009, 9:59 PM
**     Abstract  :
**         This bean implements a pulse-width modulation generator
**         that generates signal with variable duty and fixed cycle. 
**     Settings  :
**         Used output pin             : 
**             ----------------------------------------------------
**                Number (on package)  |    Name
**             ----------------------------------------------------
**                       23            |  GPIOA2_PWM2
**             ----------------------------------------------------
**
**         Timer name                  : PWM_Timer [1-bit]
**         Counter                     : PWM_CNTR  [61476]
**         Mode register               : PWM_CTRL  [61472]
**         Run register                : PWM_CTRL  [61472]
**         Prescaler                   : PWM_CTRL  [61472]
**         Compare register            : PWM_VAL2  [61480]
**         Flip-flop register          : PWM_CNFG  [61488]
**
**         User handling procedure     : not specified
**
**         Port name                   : GPIOA
**         Bit number (in port)        : 2
**         Bit mask of the port        : 4
**         Port data register          : GPIO_A_DR [61825]
**         Port control register       : GPIO_A_DDR [61826]
**         Port function register      : GPIO_A_PER [61827]
**
**         Initialization:
**              Output level           : high
**              Timer                  : Enabled
**              Event                  : Enabled
**         High speed mode
**             Prescaler               : divide-by-1
**             Clock                   : 4000000 Hz
**           Initial value of            period     pulse width
**             Xtal ticks              : 8000       8
**             microseconds            : 1000       1
**             milliseconds            : 1          0
**             seconds (real)          : 0.001      0.000001
**
**     Contents  :
**         Enable     - byte PWMBlue_Enable(void);
**         Disable    - byte PWMBlue_Disable(void);
**         SetRatio16 - byte PWMBlue_SetRatio16(word Ratio);
**
**     (c) Copyright UNIS, a.s. 1997-2008
**     UNIS, a.s.
**     Jundrovska 33
**     624 00 Brno
**     Czech Republic
**     http      : www.processorexpert.com
**     mail      : info@processorexpert.com
** ###################################################################*/


/* MODULE PWMBlue. */

#include "PWMBlue.h"

static bool EnUser;                    /* Enable/Disable device by user */
static word RatioStore;                /* Ratio of L-level to H-level */

/* Internal method prototypes */
static void HWEnDi(void);
static void SetRatio(void);

#define SetPV(Val) setRegBitGroup(PWM_CTRL,PRSC,(Val))

/*
** ===================================================================
**     Method      :  SetRatio (bean PWM)
**
**     Description :
**         Calculates and sets new duty ratio. The method is called 
**         automatically as a part of several internal methods.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void SetRatio(void)
{
  setReg(PWM_VAL2,((getReg(PWM_CMOD)+1) * (dword)RatioStore) >> 16); /* Calculate new value according to the given ratio */
  setRegBit(PWM_CTRL,LDOK);            /* Load counter and modulo registers into buffers */
}
/*
** ===================================================================
**     Method      :  HWEnDi (bean PWM)
**
**     Description :
**         Enables or disables the peripheral(s) associated with the bean.
**         The method is called automatically as a part of the Enable and 
**         Disable methods and several internal methods.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void HWEnDi(void)
{
  if (EnUser) {                        /* Enable device? */
    setRegBit(PWM_CTRL,LDOK);          /* Load counter and modulo registers into buffers */
    clrRegBits(PWM_OUT,1024);          /* Enable channel output */
  }
  else {                               /* Disable device? */
    setRegBits(PWM_OUT,1024);          /* Disable channel output */
  }
}

/*
** ===================================================================
**     Method      :  PWMBlue_Enable (bean PWM)
**
**     Description :
**         This method enables the bean - it starts the signal
**         generation. Events may be generated (<DisableEvent>
**         /<EnableEvent>).
**     Parameters  : None
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/
byte PWMBlue_Enable(void)
{
  if (!EnUser) {                       /* Is the device disabled by user? */
    EnUser = TRUE;                     /* If yes then set the flag "device enabled" */
    HWEnDi();                          /* Enable the device */
  }
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  PWMBlue_Disable (bean PWM)
**
**     Description :
**         This method disables the bean - it stops the signal
**         generation and events calling. When the timer is disabled,
**         it is possible to call <ClrValue> and <SetValue> methods.
**     Parameters  : None
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/
byte PWMBlue_Disable(void)
{
  if (EnUser) {                        /* Is the device enabled by user? */
    EnUser = FALSE;                    /* If yes then set the flag "device disabled" */
    HWEnDi();                          /* Disable the device */
  }
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  PWMBlue_SetRatio16 (bean PWM)
**
**     Description :
**         This method sets a new duty-cycle ratio.
**     Parameters  :
**         NAME       - DESCRIPTION
**         Ratio      - Ratio is expressed as an 16-bit unsigned integer
**                      number. 0 - 65535 value is proportional
**                      to ratio 0 - 100%
**         Note: Calculated duty depends on the timer possibilities
**               and on the selected period.
**     Returns     :
**         ---        - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/
byte PWMBlue_SetRatio16(word Ratio)
{
  RatioStore = Ratio;                  /* Store new value of the ratio */
  SetRatio();                          /* Calculate and set up new appropriate duty value */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  PWMBlue_Init (bean PWM)
**
**     Description :
**         Initializes the associated peripheral(s) and the beans 
**         internal variables. The method is called automatically as a 
**         part of the application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void PWMBlue_Init(void)
{
  RatioStore = (word)65U;              /* Store initial value of the ratio */
  EnUser = TRUE;                       /* Enable device */
  setReg(PWM_VAL2,4);                  /* Store initial value to the duty-compare register */
  SetPV((byte)0);                      /* Set prescaler register according to the selected high speed CPU mode */
}

/* END PWMBlue. */

/*
** ###################################################################
**
**     This file was created by UNIS Processor Expert 2.99 [04.17]
**     for the Freescale 56800 series of microcontrollers.
**
** ###################################################################
*/
