/*******************************************************************************
* File Name: SPI_PULLUP_1.c  
* Version 2.10
*
* Description:
*  This file contains API to enable firmware control of a Pins component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "SPI_PULLUP_1.h"

/* APIs are not generated for P15[7:6] on PSoC 5 */
#if !(CY_PSOC5A &&\
	 SPI_PULLUP_1__PORT == 15 && ((SPI_PULLUP_1__MASK & 0xC0) != 0))


/*******************************************************************************
* Function Name: SPI_PULLUP_1_Write
********************************************************************************
*
* Summary:
*  Assign a new value to the digital port's data output register.  
*
* Parameters:  
*  prtValue:  The value to be assigned to the Digital Port. 
*
* Return: 
*  None
*  
*******************************************************************************/
void SPI_PULLUP_1_Write(uint8 value) 
{
    uint8 staticBits = (SPI_PULLUP_1_DR & (uint8)(~SPI_PULLUP_1_MASK));
    SPI_PULLUP_1_DR = staticBits | ((uint8)(value << SPI_PULLUP_1_SHIFT) & SPI_PULLUP_1_MASK);
}


/*******************************************************************************
* Function Name: SPI_PULLUP_1_SetDriveMode
********************************************************************************
*
* Summary:
*  Change the drive mode on the pins of the port.
* 
* Parameters:  
*  mode:  Change the pins to one of the following drive modes.
*
*  SPI_PULLUP_1_DM_STRONG     Strong Drive 
*  SPI_PULLUP_1_DM_OD_HI      Open Drain, Drives High 
*  SPI_PULLUP_1_DM_OD_LO      Open Drain, Drives Low 
*  SPI_PULLUP_1_DM_RES_UP     Resistive Pull Up 
*  SPI_PULLUP_1_DM_RES_DWN    Resistive Pull Down 
*  SPI_PULLUP_1_DM_RES_UPDWN  Resistive Pull Up/Down 
*  SPI_PULLUP_1_DM_DIG_HIZ    High Impedance Digital 
*  SPI_PULLUP_1_DM_ALG_HIZ    High Impedance Analog 
*
* Return: 
*  None
*
*******************************************************************************/
void SPI_PULLUP_1_SetDriveMode(uint8 mode) 
{
	CyPins_SetPinDriveMode(SPI_PULLUP_1_0, mode);
	CyPins_SetPinDriveMode(SPI_PULLUP_1_1, mode);
}


/*******************************************************************************
* Function Name: SPI_PULLUP_1_Read
********************************************************************************
*
* Summary:
*  Read the current value on the pins of the Digital Port in right justified 
*  form.
*
* Parameters:  
*  None
*
* Return: 
*  Returns the current value of the Digital Port as a right justified number
*  
* Note:
*  Macro SPI_PULLUP_1_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 SPI_PULLUP_1_Read(void) 
{
    return (SPI_PULLUP_1_PS & SPI_PULLUP_1_MASK) >> SPI_PULLUP_1_SHIFT;
}


/*******************************************************************************
* Function Name: SPI_PULLUP_1_ReadDataReg
********************************************************************************
*
* Summary:
*  Read the current value assigned to a Digital Port's data output register
*
* Parameters:  
*  None 
*
* Return: 
*  Returns the current value assigned to the Digital Port's data output register
*  
*******************************************************************************/
uint8 SPI_PULLUP_1_ReadDataReg(void) 
{
    return (SPI_PULLUP_1_DR & SPI_PULLUP_1_MASK) >> SPI_PULLUP_1_SHIFT;
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(SPI_PULLUP_1_INTSTAT) 

    /*******************************************************************************
    * Function Name: SPI_PULLUP_1_ClearInterrupt
    ********************************************************************************
    * Summary:
    *  Clears any active interrupts attached to port and returns the value of the 
    *  interrupt status register.
    *
    * Parameters:  
    *  None 
    *
    * Return: 
    *  Returns the value of the interrupt status register
    *  
    *******************************************************************************/
    uint8 SPI_PULLUP_1_ClearInterrupt(void) 
    {
        return (SPI_PULLUP_1_INTSTAT & SPI_PULLUP_1_MASK) >> SPI_PULLUP_1_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 

#endif /* CY_PSOC5A... */

    
/* [] END OF FILE */
