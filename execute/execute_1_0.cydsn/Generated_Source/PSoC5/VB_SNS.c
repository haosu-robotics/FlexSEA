/*******************************************************************************
* File Name: VB_SNS.c  
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
#include "VB_SNS.h"

/* APIs are not generated for P15[7:6] on PSoC 5 */
#if !(CY_PSOC5A &&\
	 VB_SNS__PORT == 15 && ((VB_SNS__MASK & 0xC0) != 0))


/*******************************************************************************
* Function Name: VB_SNS_Write
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
void VB_SNS_Write(uint8 value) 
{
    uint8 staticBits = (VB_SNS_DR & (uint8)(~VB_SNS_MASK));
    VB_SNS_DR = staticBits | ((uint8)(value << VB_SNS_SHIFT) & VB_SNS_MASK);
}


/*******************************************************************************
* Function Name: VB_SNS_SetDriveMode
********************************************************************************
*
* Summary:
*  Change the drive mode on the pins of the port.
* 
* Parameters:  
*  mode:  Change the pins to one of the following drive modes.
*
*  VB_SNS_DM_STRONG     Strong Drive 
*  VB_SNS_DM_OD_HI      Open Drain, Drives High 
*  VB_SNS_DM_OD_LO      Open Drain, Drives Low 
*  VB_SNS_DM_RES_UP     Resistive Pull Up 
*  VB_SNS_DM_RES_DWN    Resistive Pull Down 
*  VB_SNS_DM_RES_UPDWN  Resistive Pull Up/Down 
*  VB_SNS_DM_DIG_HIZ    High Impedance Digital 
*  VB_SNS_DM_ALG_HIZ    High Impedance Analog 
*
* Return: 
*  None
*
*******************************************************************************/
void VB_SNS_SetDriveMode(uint8 mode) 
{
	CyPins_SetPinDriveMode(VB_SNS_0, mode);
}


/*******************************************************************************
* Function Name: VB_SNS_Read
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
*  Macro VB_SNS_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 VB_SNS_Read(void) 
{
    return (VB_SNS_PS & VB_SNS_MASK) >> VB_SNS_SHIFT;
}


/*******************************************************************************
* Function Name: VB_SNS_ReadDataReg
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
uint8 VB_SNS_ReadDataReg(void) 
{
    return (VB_SNS_DR & VB_SNS_MASK) >> VB_SNS_SHIFT;
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(VB_SNS_INTSTAT) 

    /*******************************************************************************
    * Function Name: VB_SNS_ClearInterrupt
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
    uint8 VB_SNS_ClearInterrupt(void) 
    {
        return (VB_SNS_INTSTAT & VB_SNS_MASK) >> VB_SNS_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 

#endif /* CY_PSOC5A... */

    
/* [] END OF FILE */
