/*******************************************************************************
* File Name: SD_PULLUP.h  
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

#if !defined(CY_PINS_SD_PULLUP_H) /* Pins SD_PULLUP_H */
#define CY_PINS_SD_PULLUP_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "SD_PULLUP_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 SD_PULLUP__PORT == 15 && ((SD_PULLUP__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    SD_PULLUP_Write(uint8 value);
void    SD_PULLUP_SetDriveMode(uint8 mode);
uint8   SD_PULLUP_ReadDataReg(void);
uint8   SD_PULLUP_Read(void);
void    SD_PULLUP_SetInterruptMode(uint16 position, uint16 mode);
uint8   SD_PULLUP_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the SD_PULLUP_SetDriveMode() function.
     *  @{
     */
        #define SD_PULLUP_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define SD_PULLUP_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define SD_PULLUP_DM_RES_UP          PIN_DM_RES_UP
        #define SD_PULLUP_DM_RES_DWN         PIN_DM_RES_DWN
        #define SD_PULLUP_DM_OD_LO           PIN_DM_OD_LO
        #define SD_PULLUP_DM_OD_HI           PIN_DM_OD_HI
        #define SD_PULLUP_DM_STRONG          PIN_DM_STRONG
        #define SD_PULLUP_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define SD_PULLUP_MASK               SD_PULLUP__MASK
#define SD_PULLUP_SHIFT              SD_PULLUP__SHIFT
#define SD_PULLUP_WIDTH              5u

/* Interrupt constants */
#if defined(SD_PULLUP__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in SD_PULLUP_SetInterruptMode() function.
     *  @{
     */
        #define SD_PULLUP_INTR_NONE      (uint16)(0x0000u)
        #define SD_PULLUP_INTR_RISING    (uint16)(0x0001u)
        #define SD_PULLUP_INTR_FALLING   (uint16)(0x0002u)
        #define SD_PULLUP_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define SD_PULLUP_INTR_MASK      (0x01u) 
#endif /* (SD_PULLUP__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define SD_PULLUP_PS                     (* (reg8 *) SD_PULLUP__PS)
/* Data Register */
#define SD_PULLUP_DR                     (* (reg8 *) SD_PULLUP__DR)
/* Port Number */
#define SD_PULLUP_PRT_NUM                (* (reg8 *) SD_PULLUP__PRT) 
/* Connect to Analog Globals */                                                  
#define SD_PULLUP_AG                     (* (reg8 *) SD_PULLUP__AG)                       
/* Analog MUX bux enable */
#define SD_PULLUP_AMUX                   (* (reg8 *) SD_PULLUP__AMUX) 
/* Bidirectional Enable */                                                        
#define SD_PULLUP_BIE                    (* (reg8 *) SD_PULLUP__BIE)
/* Bit-mask for Aliased Register Access */
#define SD_PULLUP_BIT_MASK               (* (reg8 *) SD_PULLUP__BIT_MASK)
/* Bypass Enable */
#define SD_PULLUP_BYP                    (* (reg8 *) SD_PULLUP__BYP)
/* Port wide control signals */                                                   
#define SD_PULLUP_CTL                    (* (reg8 *) SD_PULLUP__CTL)
/* Drive Modes */
#define SD_PULLUP_DM0                    (* (reg8 *) SD_PULLUP__DM0) 
#define SD_PULLUP_DM1                    (* (reg8 *) SD_PULLUP__DM1)
#define SD_PULLUP_DM2                    (* (reg8 *) SD_PULLUP__DM2) 
/* Input Buffer Disable Override */
#define SD_PULLUP_INP_DIS                (* (reg8 *) SD_PULLUP__INP_DIS)
/* LCD Common or Segment Drive */
#define SD_PULLUP_LCD_COM_SEG            (* (reg8 *) SD_PULLUP__LCD_COM_SEG)
/* Enable Segment LCD */
#define SD_PULLUP_LCD_EN                 (* (reg8 *) SD_PULLUP__LCD_EN)
/* Slew Rate Control */
#define SD_PULLUP_SLW                    (* (reg8 *) SD_PULLUP__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define SD_PULLUP_PRTDSI__CAPS_SEL       (* (reg8 *) SD_PULLUP__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define SD_PULLUP_PRTDSI__DBL_SYNC_IN    (* (reg8 *) SD_PULLUP__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define SD_PULLUP_PRTDSI__OE_SEL0        (* (reg8 *) SD_PULLUP__PRTDSI__OE_SEL0) 
#define SD_PULLUP_PRTDSI__OE_SEL1        (* (reg8 *) SD_PULLUP__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define SD_PULLUP_PRTDSI__OUT_SEL0       (* (reg8 *) SD_PULLUP__PRTDSI__OUT_SEL0) 
#define SD_PULLUP_PRTDSI__OUT_SEL1       (* (reg8 *) SD_PULLUP__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define SD_PULLUP_PRTDSI__SYNC_OUT       (* (reg8 *) SD_PULLUP__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(SD_PULLUP__SIO_CFG)
    #define SD_PULLUP_SIO_HYST_EN        (* (reg8 *) SD_PULLUP__SIO_HYST_EN)
    #define SD_PULLUP_SIO_REG_HIFREQ     (* (reg8 *) SD_PULLUP__SIO_REG_HIFREQ)
    #define SD_PULLUP_SIO_CFG            (* (reg8 *) SD_PULLUP__SIO_CFG)
    #define SD_PULLUP_SIO_DIFF           (* (reg8 *) SD_PULLUP__SIO_DIFF)
#endif /* (SD_PULLUP__SIO_CFG) */

/* Interrupt Registers */
#if defined(SD_PULLUP__INTSTAT)
    #define SD_PULLUP_INTSTAT            (* (reg8 *) SD_PULLUP__INTSTAT)
    #define SD_PULLUP_SNAP               (* (reg8 *) SD_PULLUP__SNAP)
    
	#define SD_PULLUP_0_INTTYPE_REG 		(* (reg8 *) SD_PULLUP__0__INTTYPE)
	#define SD_PULLUP_1_INTTYPE_REG 		(* (reg8 *) SD_PULLUP__1__INTTYPE)
	#define SD_PULLUP_2_INTTYPE_REG 		(* (reg8 *) SD_PULLUP__2__INTTYPE)
	#define SD_PULLUP_3_INTTYPE_REG 		(* (reg8 *) SD_PULLUP__3__INTTYPE)
	#define SD_PULLUP_4_INTTYPE_REG 		(* (reg8 *) SD_PULLUP__4__INTTYPE)
#endif /* (SD_PULLUP__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_SD_PULLUP_H */


/* [] END OF FILE */
