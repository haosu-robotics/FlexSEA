/*******************************************************************************
* File Name: PWMH3.h  
* Version 2.10
*
* Description:
*  This file containts Control Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_PWMH3_H) /* Pins PWMH3_H */
#define CY_PINS_PWMH3_H

#include "cytypes.h"
#include "cyfitter.h"
#include "PWMH3_aliases.h"


/***************************************
*        Function Prototypes             
***************************************/    

void    PWMH3_Write(uint8 value) ;
void    PWMH3_SetDriveMode(uint8 mode) ;
uint8   PWMH3_ReadDataReg(void) ;
uint8   PWMH3_Read(void) ;
uint8   PWMH3_ClearInterrupt(void) ;


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define PWMH3_DRIVE_MODE_BITS        (3)
#define PWMH3_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - PWMH3_DRIVE_MODE_BITS))

#define PWMH3_DM_ALG_HIZ         (0x00u)
#define PWMH3_DM_DIG_HIZ         (0x01u)
#define PWMH3_DM_RES_UP          (0x02u)
#define PWMH3_DM_RES_DWN         (0x03u)
#define PWMH3_DM_OD_LO           (0x04u)
#define PWMH3_DM_OD_HI           (0x05u)
#define PWMH3_DM_STRONG          (0x06u)
#define PWMH3_DM_RES_UPDWN       (0x07u)

/* Digital Port Constants */
#define PWMH3_MASK               PWMH3__MASK
#define PWMH3_SHIFT              PWMH3__SHIFT
#define PWMH3_WIDTH              1u


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define PWMH3_PS                     (* (reg32 *) PWMH3__PS)
/* Port Configuration */
#define PWMH3_PC                     (* (reg32 *) PWMH3__PC)
/* Data Register */
#define PWMH3_DR                     (* (reg32 *) PWMH3__DR)
/* Input Buffer Disable Override */
#define PWMH3_INP_DIS                (* (reg32 *) PWMH3__PC2)


#if defined(PWMH3__INTSTAT)  /* Interrupt Registers */

    #define PWMH3_INTSTAT                (* (reg32 *) PWMH3__INTSTAT)

#endif /* Interrupt Registers */


/***************************************
* The following code is DEPRECATED and 
* must not be used.
***************************************/

#define PWMH3_DRIVE_MODE_SHIFT       (0x00u)
#define PWMH3_DRIVE_MODE_MASK        (0x07u << PWMH3_DRIVE_MODE_SHIFT)


#endif /* End Pins PWMH3_H */


/* [] END OF FILE */
