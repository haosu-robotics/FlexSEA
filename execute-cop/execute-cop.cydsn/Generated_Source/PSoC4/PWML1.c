/*******************************************************************************
* File Name: PWML1.c  
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
#include "PWML1.h"

#define SetP4PinDriveMode(shift, mode)  \
    do { \
        PWML1_PC =   (PWML1_PC & \
                                (uint32)(~(uint32)(PWML1_DRIVE_MODE_IND_MASK << (PWML1_DRIVE_MODE_BITS * (shift))))) | \
                                (uint32)((uint32)(mode) << (PWML1_DRIVE_MODE_BITS * (shift))); \
    } while (0)


/*******************************************************************************
* Function Name: PWML1_Write
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
void PWML1_Write(uint8 value) 
{
    uint8 drVal = (uint8)(PWML1_DR & (uint8)(~PWML1_MASK));
    drVal = (drVal | ((uint8)(value << PWML1_SHIFT) & PWML1_MASK));
    PWML1_DR = (uint32)drVal;
}


/*******************************************************************************
* Function Name: PWML1_SetDriveMode
********************************************************************************
*
* Summary:
*  Change the drive mode on the pins of the port.
* 
* Parameters:  
*  mode:  Change the pins to one of the following drive modes.
*
*  PWML1_DM_STRONG     Strong Drive 
*  PWML1_DM_OD_HI      Open Drain, Drives High 
*  PWML1_DM_OD_LO      Open Drain, Drives Low 
*  PWML1_DM_RES_UP     Resistive Pull Up 
*  PWML1_DM_RES_DWN    Resistive Pull Down 
*  PWML1_DM_RES_UPDWN  Resistive Pull Up/Down 
*  PWML1_DM_DIG_HIZ    High Impedance Digital 
*  PWML1_DM_ALG_HIZ    High Impedance Analog 
*
* Return: 
*  None
*
*******************************************************************************/
void PWML1_SetDriveMode(uint8 mode) 
{
	SetP4PinDriveMode(PWML1__0__SHIFT, mode);
}


/*******************************************************************************
* Function Name: PWML1_Read
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
*  Macro PWML1_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 PWML1_Read(void) 
{
    return (uint8)((PWML1_PS & PWML1_MASK) >> PWML1_SHIFT);
}


/*******************************************************************************
* Function Name: PWML1_ReadDataReg
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
uint8 PWML1_ReadDataReg(void) 
{
    return (uint8)((PWML1_DR & PWML1_MASK) >> PWML1_SHIFT);
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(PWML1_INTSTAT) 

    /*******************************************************************************
    * Function Name: PWML1_ClearInterrupt
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
    uint8 PWML1_ClearInterrupt(void) 
    {
		uint8 maskedStatus = (uint8)(PWML1_INTSTAT & PWML1_MASK);
		PWML1_INTSTAT = maskedStatus;
        return maskedStatus >> PWML1_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 


/* [] END OF FILE */
