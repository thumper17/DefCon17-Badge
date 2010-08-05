/** ###################################################################
**     THIS BEAN MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : Term1.C
**     Project   : DC17
**     Processor : MC56F8006_32_LQFP
**     Beantype  : Term
**     Version   : Bean 02.028, Driver 03.18, CPU db: 3.00.169
**     Compiler  : Metrowerks DSP C Compiler
**     Date/Time : 3/21/2009, 1:50 PM
**     Abstract  :
**
**     Settings  :
**
**     Contents  :
**         SendStr  - void Term1_SendStr(uint8_t *str);
**         SendNum  - void Term1_SendNum(int32_t number);
**         SendChar - void Term1_SendChar(uint8_t Val);
**         MoveTo   - void Term1_MoveTo(uint8_t x, uint8_t y);
**
**     (c) Copyright UNIS, a.s. 1997-2008
**     UNIS, a.s.
**     Jundrovska 33
**     624 00 Brno
**     Czech Republic
**     http      : www.processorexpert.com
**     mail      : info@processorexpert.com
** ###################################################################*/


/* MODULE Term1. */
#include "Term1.h"
#include "Inhr6.h"


#define COLUMNS  80                    /* Number of columns */
#define ROWS     24                    /* Number of rows */


/* Internal method prototypes */
static void SendESCPrefix(void);
static void LongToStr(void *s, int32_t n);

/*
** ===================================================================
**     Method      :  Term1_SendChar (bean Term)
**
**     Description :
**         Send char to terminal.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Val             - Char to send
**     Returns     : Nothing
** ===================================================================
*/
void Term1_SendChar(uint8_t Val)
{
  while (Inhr6_SendChar(Val) == ERR_TXFULL){} /* Send char */
}

/*
** ===================================================================
**     Method      :  Term1_SendESCPrefix (bean Term)
**
**     Description :
**         The method sends first (common) part of an excape sequence
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void SendESCPrefix(void)
{
  while (Inhr6_SendChar((uint8_t) 0x1B) == ERR_TXFULL){} /* Send the part of the escape sequence */
  while (Inhr6_SendChar((uint8_t) 0x5B) == ERR_TXFULL){} /* Send the part of the escape sequence */
}

/*
** ===================================================================
**     Method      :  Term1_SendStr (bean Term)
**
**     Description :
**         Send string to terminal.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * str             - Pointer to a string variable
**     Returns     : Nothing
** ===================================================================
*/
void Term1_SendStr(void *str)
{
  uint8_t *ptr =  ((uint8_t *) str);
  while (*ptr != '\0') {
    while (Inhr6_SendChar(*ptr) == ERR_TXFULL){} /* Send char */
    ptr++;                             /* Increment the pointer */
  }
}

/*
** ===================================================================
**     Method      :  Term1_LongToStr (bean Term)
**
**     Description :
**         The method converts long int number to string.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void LongToStr(void *s, int32_t n)
{
  uint8_t *ptr =  ((uint8_t *) s);
  uint8_t i=0, j;
  uint8_t tmp;
  bool sign = (bool)(n < 0);

  if (sign){
    n *= -1;
  }
  if (n == 0){
    ptr[i++] = '0';
  }
  while (n > 0) {
    ptr[i++] = (uint8_t)((n % 10) + '0');
    n /= 10;
  }
  if (sign){
    ptr[i++] = '-';
  }
  for(j=0; j<(i/2); j++) {
    tmp = ptr[j];
    ptr[j] = ptr[(i-j)-1];
    ptr[(i-j)-1] = tmp;
  }
  ptr[i] = '\0';
}

/*
** ===================================================================
**     Method      :  Term1_SendNum (bean Term)
**
**     Description :
**         Send number to terminal.
**     Parameters  :
**         NAME            - DESCRIPTION
**         number          - Long number
**     Returns     : Nothing
** ===================================================================
*/
void Term1_SendNum(int32_t number)
{
  uint8_t str[15];                     /* Temporary variable */
  uint8_t i=0;                         /* Temporary variable */

  LongToStr(str, number);              /* Conversion number to the string */
  while (str[i] != '\0') {
    while (Inhr6_SendChar((uint8_t)str[i]) == ERR_TXFULL){} /* Send char */
    i++;                               /* Increase the variable */
  }
}

/*
** ===================================================================
**     Method      :  Term1_MoveTo (bean Term)
**
**     Description :
**         Move cursor to position x, y. Range of coordinates can be
**         from 1 to the size, which depends on the used terminal.
**         Position [1,1] represents the upper left corner.
**     Parameters  :
**         NAME            - DESCRIPTION
**         x               - Column number
**         y               - Row number
**     Returns     : Nothing
** ===================================================================
*/
void Term1_MoveTo(uint8_t x,uint8_t y)
{
  SendESCPrefix();                     /* Send the escape prefix */
  Term1_SendNum((int32_t) y);          /* Send number of line */
  while (Inhr6_SendChar((uint8_t) ';') == ERR_TXFULL){} /* Send the part of the escape sequence */
  Term1_SendNum((int32_t) x);          /* Send number of column */
  while (Inhr6_SendChar((uint8_t) 'H') == ERR_TXFULL){} /* Send the part of the escape sequence */
}


/* END Term1. */

/*
** ###################################################################
**
**     This file was created by UNIS Processor Expert 2.99 [04.17]
**     for the Freescale 56800 series of microcontrollers.
**
** ###################################################################
*/

