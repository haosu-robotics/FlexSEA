/*******************************************************************************
* File Name: EXP9.h  
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

#if !defined(CY_PINS_EXP9_H) /* Pins EXP9_H */
#define CY_PINS_EXP9_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "EXP9_aliases.h"

/* Check to see if required defines such as CY_PSOC5A are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5A)
    #error Component cy_pins_v2_10 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5A) */

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 EXP9__PORT == 15 && ((EXP9__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

void    EXP9_Write(uint8 value) ;
void    EXP9_SetDriveMode(uint8 mode) ;
uint8   EXP9_ReadDataReg(void) ;
uint8   EXP9_Read(void) ;
uint8   EXP9_ClearInterrupt(void) ;


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define EXP9_DM_ALG_HIZ         PIN_DM_ALG_HIZ
#define EXP9_DM_DIG_HIZ         PIN_DM_DIG_HIZ
#define EXP9_DM_RES_UP          PIN_DM_RES_UP
#define EXP9_DM_RES_DWN         PIN_DM_RES_DWN
#define EXP9_DM_OD_LO           PIN_DM_OD_LO
#define EXP9_DM_OD_HI           PIN_DM_OD_HI
#define EXP9_DM_STRONG          PIN_DM_STRONG
#define EXP9_DM_RES_UPDWN       PIN_DM_RES_UPDWN

/* Digital Port Constants */
#define EXP9_MASK               EXP9__MASK
#define EXP9_SHIFT              EXP9__SHIFT
#define EXP9_WIDTH              1u


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define EXP9_PS                     (* (reg8 *) EXP9__PS)
/* Data Register */
#define EXP9_DR                     (* (reg8 *) EXP9__DR)
/* Port Number */
#define EXP9_PRT_NUM                (* (reg8 *) EXP9__PRT) 
/* Connect to Analog Globals */                                                  
#define EXP9_AG                     (* (reg8 *) EXP9__AG)                       
/* Analog MUX bux enable */
#define EXP9_AMUX                   (* (reg8 *) EXP9__AMUX) 
/* Bidirectional Enable */                                                        
#define EXP9_BIE                    (* (reg8 *) EXP9__BIE)
/* Bit-mask for Aliased Register Access */
#define EXP9_BIT_MASK               (* (reg8 *) EXP9__BIT_MASK)
/* Bypass Enable */
#define EXP9_BYP                    (* (reg8 *) EXP9__BYP)
/* Port wide control signals */                                                   
#define EXP9_CTL                    (* (reg8 *) EXP9__CTL)
/* Drive Modes */
#define EXP9_DM0                    (* (reg8 *) EXP9__DM0) 
#define EXP9_DM1                    (* (reg8 *) EXP9__DM1)
#define EXP9_DM2                    (* (reg8 *) EXP9__DM2) 
/* Input Buffer Disable Override */
#define EXP9_INP_DIS                (* (reg8 *) EXP9__INP_DIS)
/* LCD Common or Segment Drive */
#define EXP9_LCD_COM_SEG            (* (reg8 *) EXP9__LCD_COM_SEG)
/* Enable Segment LCD */
#define EXP9_LCD_EN                 (* (reg8 *) EXP9__LCD_EN)
/* Slew Rate Control */
#define EXP9_SLW                    (* (reg8 *) EXP9__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define EXP9_PRTDSI__CAPS_SEL       (* (reg8 *) EXP9__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define EXP9_PRTDSI__DBL_SYNC_IN    (* (reg8 *) EXP9__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define EXP9_PRTDSI__OE_SEL0        (* (reg8 *) EXP9__PRTDSI__OE_SEL0) 
#define EXP9_PRTDSI__OE_SEL1        (* (reg8 *) EXP9__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define EXP9_PRTDSI__OUT_SEL0       (* (reg8 *) EXP9__PRTDSI__OUT_SEL0) 
#define EXP9_PRTDSI__OUT_SEL1       (* (reg8 *) EXP9__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define EXP9_PRTDSI__SYNC_OUT       (* (reg8 *) EXP9__PRTDSI__SYNC_OUT) 


#if defined(EXP9__INTSTAT)  /* Interrupt Registers */

    #define EXP9_INTSTAT                (* (reg8 *) EXP9__INTSTAT)
    #define EXP9_SNAP                   (* (reg8 *) EXP9__SNAP)

#endif /* Interrupt Registers */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_EXP9_H */


/* [] END OF FILE */
