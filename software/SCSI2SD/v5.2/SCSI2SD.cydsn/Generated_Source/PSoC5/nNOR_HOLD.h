/*******************************************************************************
* File Name: nNOR_HOLD.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_nNOR_HOLD_H) /* Pins nNOR_HOLD_H */
#define CY_PINS_nNOR_HOLD_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "nNOR_HOLD_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 nNOR_HOLD__PORT == 15 && ((nNOR_HOLD__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    nNOR_HOLD_Write(uint8 value);
void    nNOR_HOLD_SetDriveMode(uint8 mode);
uint8   nNOR_HOLD_ReadDataReg(void);
uint8   nNOR_HOLD_Read(void);
void    nNOR_HOLD_SetInterruptMode(uint16 position, uint16 mode);
uint8   nNOR_HOLD_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the nNOR_HOLD_SetDriveMode() function.
     *  @{
     */
        #define nNOR_HOLD_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define nNOR_HOLD_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define nNOR_HOLD_DM_RES_UP          PIN_DM_RES_UP
        #define nNOR_HOLD_DM_RES_DWN         PIN_DM_RES_DWN
        #define nNOR_HOLD_DM_OD_LO           PIN_DM_OD_LO
        #define nNOR_HOLD_DM_OD_HI           PIN_DM_OD_HI
        #define nNOR_HOLD_DM_STRONG          PIN_DM_STRONG
        #define nNOR_HOLD_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define nNOR_HOLD_MASK               nNOR_HOLD__MASK
#define nNOR_HOLD_SHIFT              nNOR_HOLD__SHIFT
#define nNOR_HOLD_WIDTH              1u

/* Interrupt constants */
#if defined(nNOR_HOLD__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in nNOR_HOLD_SetInterruptMode() function.
     *  @{
     */
        #define nNOR_HOLD_INTR_NONE      (uint16)(0x0000u)
        #define nNOR_HOLD_INTR_RISING    (uint16)(0x0001u)
        #define nNOR_HOLD_INTR_FALLING   (uint16)(0x0002u)
        #define nNOR_HOLD_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define nNOR_HOLD_INTR_MASK      (0x01u) 
#endif /* (nNOR_HOLD__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define nNOR_HOLD_PS                     (* (reg8 *) nNOR_HOLD__PS)
/* Data Register */
#define nNOR_HOLD_DR                     (* (reg8 *) nNOR_HOLD__DR)
/* Port Number */
#define nNOR_HOLD_PRT_NUM                (* (reg8 *) nNOR_HOLD__PRT) 
/* Connect to Analog Globals */                                                  
#define nNOR_HOLD_AG                     (* (reg8 *) nNOR_HOLD__AG)                       
/* Analog MUX bux enable */
#define nNOR_HOLD_AMUX                   (* (reg8 *) nNOR_HOLD__AMUX) 
/* Bidirectional Enable */                                                        
#define nNOR_HOLD_BIE                    (* (reg8 *) nNOR_HOLD__BIE)
/* Bit-mask for Aliased Register Access */
#define nNOR_HOLD_BIT_MASK               (* (reg8 *) nNOR_HOLD__BIT_MASK)
/* Bypass Enable */
#define nNOR_HOLD_BYP                    (* (reg8 *) nNOR_HOLD__BYP)
/* Port wide control signals */                                                   
#define nNOR_HOLD_CTL                    (* (reg8 *) nNOR_HOLD__CTL)
/* Drive Modes */
#define nNOR_HOLD_DM0                    (* (reg8 *) nNOR_HOLD__DM0) 
#define nNOR_HOLD_DM1                    (* (reg8 *) nNOR_HOLD__DM1)
#define nNOR_HOLD_DM2                    (* (reg8 *) nNOR_HOLD__DM2) 
/* Input Buffer Disable Override */
#define nNOR_HOLD_INP_DIS                (* (reg8 *) nNOR_HOLD__INP_DIS)
/* LCD Common or Segment Drive */
#define nNOR_HOLD_LCD_COM_SEG            (* (reg8 *) nNOR_HOLD__LCD_COM_SEG)
/* Enable Segment LCD */
#define nNOR_HOLD_LCD_EN                 (* (reg8 *) nNOR_HOLD__LCD_EN)
/* Slew Rate Control */
#define nNOR_HOLD_SLW                    (* (reg8 *) nNOR_HOLD__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define nNOR_HOLD_PRTDSI__CAPS_SEL       (* (reg8 *) nNOR_HOLD__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define nNOR_HOLD_PRTDSI__DBL_SYNC_IN    (* (reg8 *) nNOR_HOLD__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define nNOR_HOLD_PRTDSI__OE_SEL0        (* (reg8 *) nNOR_HOLD__PRTDSI__OE_SEL0) 
#define nNOR_HOLD_PRTDSI__OE_SEL1        (* (reg8 *) nNOR_HOLD__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define nNOR_HOLD_PRTDSI__OUT_SEL0       (* (reg8 *) nNOR_HOLD__PRTDSI__OUT_SEL0) 
#define nNOR_HOLD_PRTDSI__OUT_SEL1       (* (reg8 *) nNOR_HOLD__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define nNOR_HOLD_PRTDSI__SYNC_OUT       (* (reg8 *) nNOR_HOLD__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(nNOR_HOLD__SIO_CFG)
    #define nNOR_HOLD_SIO_HYST_EN        (* (reg8 *) nNOR_HOLD__SIO_HYST_EN)
    #define nNOR_HOLD_SIO_REG_HIFREQ     (* (reg8 *) nNOR_HOLD__SIO_REG_HIFREQ)
    #define nNOR_HOLD_SIO_CFG            (* (reg8 *) nNOR_HOLD__SIO_CFG)
    #define nNOR_HOLD_SIO_DIFF           (* (reg8 *) nNOR_HOLD__SIO_DIFF)
#endif /* (nNOR_HOLD__SIO_CFG) */

/* Interrupt Registers */
#if defined(nNOR_HOLD__INTSTAT)
    #define nNOR_HOLD_INTSTAT            (* (reg8 *) nNOR_HOLD__INTSTAT)
    #define nNOR_HOLD_SNAP               (* (reg8 *) nNOR_HOLD__SNAP)
    
	#define nNOR_HOLD_0_INTTYPE_REG 		(* (reg8 *) nNOR_HOLD__0__INTTYPE)
#endif /* (nNOR_HOLD__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_nNOR_HOLD_H */


/* [] END OF FILE */
