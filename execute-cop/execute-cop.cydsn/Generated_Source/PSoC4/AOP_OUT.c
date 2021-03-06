/*******************************************************************************
* File Name: AOP_OUT.c  
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
#include "AOP_OUT.h"

#define SetP4PinDriveMode(shift, mode)  \
    do { \
        AOP_OUT_PC =   (AOP_OUT_PC & \
                                (uint32)(~(uint32)(AOP_OUT_DRIVE_MODE_IND_MASK << (AOP_OUT_DRIVE_MODE_BITS * (shift))))) | \
                                (uint32)((uint32)(mode) << (AOP_OUT_DRIVE_MODE_BITS * (shift))); \
    } while (0)


/*******************************************************************************
* Function Name: AOP_OUT_Write
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
void AOP_OUT_Write(uint8 value) 
{
    uint8 drVal = (uint8)(AOP_OUT_DR & (uint8)(~AOP_OUT_MASK));
    drVal = (drVal | ((uint8)(value << AOP_OUT_SHIFT) & AOP_OUT_MASK));
    AOP_OUT_DR = (uint32)drVal;
}


/*******************************************************************************
* Function Name: AOP_OUT_SetDriveMode
********************************************************************************
*
* Summary:
*  Change the drive mode on the pins of the port.
* 
* Parameters:  
*  mode:  Change the pins to one of the following drive modes.
*
*  AOP_OUT_DM_STRONG     Strong Drive 
*  AOP_OUT_DM_OD_HI      Open Drain, Drives High 
*  AOP_OUT_DM_OD_LO      Open Drain, Drives Low 
*  AOP_OUT_DM_RES_UP     Resistive Pull Up 
*  AOP_OUT_DM_RES_DWN    Resistive Pull Down 
*  AOP_OUT_DM_RES_UPDWN  Resistive Pull Up/Down 
*  AOP_OUT_DM_DIG_HIZ    High Impedance Digital 
*  AOP_OUT_DM_ALG_HIZ    High Impedance Analog 
*
* Return: 
*  None
*
*******************************************************************************/
void AOP_OUT_SetDriveMode(uint8 mode) 
{
	SetP4PinDriveMode(AOP_OUT__0__SHIFT, mode);
}


/*******************************************************************************
* Function Name: AOP_OUT_Read
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
*  Macro AOP_OUT_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 AOP_OUT_Read(void) 
{
    return (uint8)((AOP_OUT_PS & AOP_OUT_MASK) >> AOP_OUT_SHIFT);
}


/*******************************************************************************
* Function Name: AOP_OUT_ReadDataReg
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
uint8 AOP_OUT_ReadDataReg(void) 
{
    return (uint8)((AOP_OUT_DR & AOP_OUT_MASK) >> AOP_OUT_SHIFT);
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(AOP_OUT_INTSTAT) 

    /*******************************************************************************
    * Function Name: AOP_OUT_ClearInterrupt
    ********************************************************************************
    *
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
    uint8 AOP_OUT_ClearInterrupt(void) 
    {
		uint8 maskedStatus = (uint8)(AOP_OUT_INTSTAT & AOP_OUT_MASK);
		AOP_OUT_INTSTAT = maskedStatus;
        return maskedStatus >> AOP_OUT_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 


/* [] END OF FILE */
