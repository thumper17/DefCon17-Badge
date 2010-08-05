/** ###################################################################
**     THIS BEAN MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : BADGEI2C.H
**     Project   : DC17
**     Processor : MC56F8006_32_LQFP
**     Beantype  : InternalI2C
**     Version   : Bean 01.225, Driver 02.01, CPU db: 3.00.169
**     Compiler  : Metrowerks DSP C Compiler
**     Date/Time : 4/21/2009, 2:29 PM
**     Abstract  :
**          This bean encapsulates the internal I2C communication 
**          interface. The implementation of the interface is based 
**          on the Philips I2C-bus specification version 2.0. 
**          Interface features:
**          MASTER mode
**            - Multi master communication
**            - The combined format of communication possible 
**              (see "Automatic stop condition" property)
**            - 7-bit slave addressing (10-bit addressing can be made as well)
**            - Acknowledge polling provided
**            - No wait state initiated when a slave device holds the SCL line low
**            - Holding of the SCL line low by slave device recognized as 'not available bus'
**            - Invalid start/stop condition detection provided
**          SLAVE mode
**            - 7-bit slave addressing
**            - General call address detection provided
**     Settings  :
**         Serial channel              : I2C
**
**         Protocol
**             Mode                    : MASTER - SLAVE
**             Slave address           : 0
**             Empty character         : 0
**             Auto stop condition     : yes
**             SCL frequency           : 50 kHz
**
**         Initialization
**
**             Target slave address    : 0
**             Bean function           : Enabled
**             Events                  : Enabled
**
**         Registers
**             Input buffer            : I2C_DATA  [61732]
**             Output buffer           : I2C_DATA  [61732]
**             Control register        : I2C_CR1   [61730]
**             Status register         : I2C_SR    [61731]
**             Baud setting reg.       : I2C_FREQDIV [61729]
**             Address register        : I2C_ADDR  [61728]
**
**         Interrupt
**             Vector name             : INT_I2C
**             Priority                : 0
**
**         Used pins                   :
**       ----------------------------------------------------------
**            Function    | On package |    Name
**       ----------------------------------------------------------
**              SDA       |     19     |  GPIOB4_ANA0_ANB0_MISO_SDA_RXD_T0_CLKO_0
**              SCL       |     21     |  GPIOB0_ANB13_PWM3_SCLK_SCL_T1
**       ----------------------------------------------------------
**     Contents  :
**         Enable               - byte BADGEI2C_Enable(void);
**         Disable              - byte BADGEI2C_Disable(void);
**         SendBlock            - byte BADGEI2C_SendBlock(void* Ptr, word Siz, word *Snt);
**         RecvBlock            - byte BADGEI2C_RecvBlock(void* Ptr, word Siz, word *Rcv);
**         ClearRxBuf           - byte BADGEI2C_ClearRxBuf(void);
**         SelectSlave          - byte BADGEI2C_SelectSlave(byte Slv);
**         SelectSpecialCommand - byte BADGEI2C_SelectSpecialCommand(byte Cmd);
**         SetMode              - byte BADGEI2C_SetMode(bool MasterMode);
**
**     (c) Copyright UNIS, a.s. 1997-2008
**     UNIS, a.s.
**     Jundrovska 33
**     624 00 Brno
**     Czech Republic
**     http      : www.processorexpert.com
**     mail      : info@processorexpert.com
** ###################################################################*/

#ifndef __BADGEI2C
#define __BADGEI2C

/* MODULE BADGEI2C. */

/*Include shared modules, which are used for whole project*/
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* Include inherited beans */
#include "Cpu.h"

#define BADGEI2C_EOF   0               /* Empty character*/

/* MODULE BADGEI2C. */
extern volatile byte BADGEI2C_SerFlag; /* Flags for serial communication */


void BADGEI2C_Interrupt(void);
/*
** ===================================================================
**     Method      :  BADGEI2C_Interrupt (bean InternalI2C)
**
**     Description :
**         The method services the interrupt of the selected peripheral(s)
**         and eventually invokes the beans event(s).
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

byte BADGEI2C_Enable(void);
/*
** ===================================================================
**     Method      :  BADGEI2C_Enable (bean InternalI2C)
**
**     Description :
**         Enables I2C bean. Events may be generated
**         ("DisableEvent"/"EnableEvent").
**     Parameters  : None
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/

byte BADGEI2C_Disable(void);
/*
** ===================================================================
**     Method      :  BADGEI2C_Disable (bean InternalI2C)
**
**     Description :
**         Disables I2C bean. No events will be generated.
**     Parameters  : None
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/

byte BADGEI2C_SendBlock(void* Ptr,word Siz,word *Snt);
/*
** ===================================================================
**     Method      :  BADGEI2C_SendBlock (bean InternalI2C)
**
**     Description :
**         When working as a MASTER, this method writes one (7-bit
**         addressing) or two (10-bit addressing) slave address
**         bytes inclusive of R/W bit = 0 to the I2C bus and then
**         writes the block of characters to the bus. The slave
**         address must be specified before, by the "SelectSlave" or
**         "SlaveSelect10" method or in bean initialization section,
**         "Target slave address init" property. If interrupt
**         service is enabled and the method returns ERR_OK, it
**         doesn't mean that transmission was successful. The state
**         of transmission is detectable by means of events
**         (OnTransmitData, OnError or OnArbitLost). Data to be send
**         is not copied to an internal buffer and remains in the
**         original location. Therefore the content of the buffer
**         should not be changed until the transmission is complete.
**         Event OnTransmitData can be used to detect the end of the
**         transmission.
**         When working as a SLAVE, this method writes a block of
**         characters to the internal output slave buffer and then,
**         after the master starts the communication, to the I2C bus.
**         If no character is ready for a transmission (internal
**         output slave buffer is empty), the "Empty character" will
**         be sent (see "Empty character" property). In SLAVE mode
**         the data are copied to an internal buffer, if specified
**         by "Output buffer size" property.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Ptr             - Pointer to the block of data to send.
**         Siz             - Size of the block.
**       * Snt             - Amount of data sent (moved to a buffer).
**                           In master mode, if interrupt support is
**                           enabled, the parameter always returns
**                           the same value as the parameter 'Siz' of
**                           this method.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_DISABLED -  Device is disabled
**                           ERR_BUSY - The slave device is busy, it
**                           does not respond by the acknowledge
**                           (only in master mode and when interrupt
**                           service is disabled)
**                           ERR_BUSOFF - Clock timeout elapsed or
**                           device cannot transmit data
**                           ERR_TXFULL - Transmitter is full. Some
**                           data has not been sent. (slave mode only)
**                           ERR_ARBITR - Arbitration lost (only when
**                           interrupt service is disabled and in
**                           master mode)
** ===================================================================
*/

byte BADGEI2C_RecvBlock(void* Ptr,word Siz,word *Rcv);
/*
** ===================================================================
**     Method      :  BADGEI2C_RecvBlock (bean InternalI2C)
**
**     Description :
**         When working as a MASTER, this method writes one (7-bit
**         addressing) or two (10-bit addressing) slave address
**         bytes inclusive of R/W bit = 1 to the I2C bus, then reads
**         the block of characters from the bus and then sends the
**         stop condition. The slave address must be specified
**         before, by the "SelectSlave" or "SelectSlave10" method or
**         in bean initialization section, "Target slave address
**         init" property. If interrupt service is enabled and the
**         method returns ERR_OK, it doesn't mean that transmission
**         was finished successfully. The state of transmission must
**         be tested by means of events (OnReceiveData, OnError or
**         OnArbitLost). In case of successful transmission,
**         received data is ready after OnReceiveData event is
**         called. 
**         When working as a SLAVE, this method reads a block of
**         characters from the input slave buffer.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Ptr             - A pointer to the block space for
**                           received data.
**         Siz             - The size of the block.
**       * Rcv             - Amount of received data. In master mode,
**                           if interrupt support is enabled, the
**                           parameter always returns the same value
**                           as the parameter 'Siz' of this method.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_DISABLED -  Device is disabled
**                           ERR_BUSY - The slave device is busy, it
**                           does not respond by an acknowledge (only
**                           in master mode and when interrupt
**                           service is disabled)
**                           ERR_BUSOFF - Clock timeout elapsed or
**                           device cannot receive data
**                           ERR_RXEMPTY - The receive buffer didn't
**                           contain the requested number of data.
**                           Only available data (or no data) has
**                           been returned  (slave mode only).
**                           ERR_OVERRUN - Overrun error was detected
**                           from last character or block receiving
**                           (slave mode only)
**                           ERR_ARBITR - Arbitration lost (only when
**                           interrupt service is disabled and in
**                           master mode)
** ===================================================================
*/

byte BADGEI2C_ClearRxBuf(void);
/*
** ===================================================================
**     Method      :  BADGEI2C_ClearRxBuf (bean InternalI2C)
**
**     Description :
**         This method clears all characters in internal slave input
**         buffer. This method is not available for the MASTER mode.
**     Parameters  : None
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/

byte BADGEI2C_SelectSlave(byte Slv);
/*
** ===================================================================
**     Method      :  BADGEI2C_SelectSlave (bean InternalI2C)
**
**     Description :
**         This method selects a new slave for communication by its
**         7-bit slave address value. Any send or receive method
**         directs to or from selected device, until a new slave
**         device is selected by this method. This method is not
**         available for the SLAVE mode.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Slv             - 7-bit slave address value.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_BUSY - The device is busy, wait
**                           until the current operation is finished.
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_DISABLED -  The device is disabled
** ===================================================================
*/

byte BADGEI2C_SelectSpecialCommand(byte Cmd);
/*
** ===================================================================
**     Method      :  BADGEI2C_SelectSpecialCommand (bean InternalI2C)
**
**     Description :
**         This method selects a special command. Any send or receive
**         method directs to or from selected device, until a new slave
**         device is selected by this method. This method is not
**         available for the SLAVE mode. Certain commands may not be
**         supported on a specific derivative, if IIC module does not
**         have the capability.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Cmd             - Special command. Possible values:
**                           0 - General call address
**                           1 - Start byte
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_BUSY - The device is busy, wait until
**                           the current operation is finished.
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_DISABLED -  The device is disabled
**                           ERR_RANGE -  Requested command is not
**                           supported.
** ===================================================================
*/

byte BADGEI2C_SetMode(bool MasterMode);
/*
** ===================================================================
**     Method      :  BADGEI2C_SetMode (bean InternalI2C)
**
**     Description :
**         This method sets actual operating mode of this bean. This
**         method is available only for the MASTER - SLAVE mode.
**         Note: If MASTER mode is selected as an operating mode,
**         the device can still be selected as a slave and
**         receive/transmit data. However, it's necessary to select
**         slave operating mode using SetMode( FALSE ) to access
**         this data.
**     Parameters  :
**         NAME            - DESCRIPTION
**         MasterMode      - Operating mode value:
**                           TRUE - Master
**                           FALSE - Slave
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/

void BADGEI2C_Init(void);
/*
** ===================================================================
**     Method      :  BADGEI2C_Init (bean InternalI2C)
**
**     Description :
**         Initializes the associated peripheral(s) and the beans 
**         internal variables. The method is called automatically as a 
**         part of the application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/


/* END BADGEI2C. */

#endif /* ifndef __BADGEI2C */
/*
** ###################################################################
**
**     This file was created by UNIS Processor Expert 2.99 [04.17]
**     for the Freescale 56800 series of microcontrollers.
**
** ###################################################################
*/
