/*******************************************************************************
* File Name: nNOR_WP.h  
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

#if !defined(CY_PINS_nNOR_WP_H) /* Pins nNOR_WP_H */
#define CY_PINS_nNOR_WP_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "nNOR_WP_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 nNOR_WP__PORT == 15 && ((nNOR_WP__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    nNOR_WP_Write(uint8 value);
void    nNOR_WP_SetDriveMode(uint8 mode);
uint8   nNOR_WP_ReadDataReg(void);
uint8   nNOR_WP_Read(void);
void    nNOR_WP_SetInterruptMode(uint16 position, uint16 mode);
uint8   nNOR_WP_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the nNOR_WP_SetDriveMode() function.
     *  @{
     */
        #define nNOR_WP_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define nNOR_WP_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define nNOR_WP_DM_RES_UP          PIN_DM_RES_UP
        #define nNOR_WP_DM_RES_DWN         PIN_DM_RES_DWN
        #define nNOR_WP_DM_OD_LO           PIN_DM_OD_LO
        #define nNOR_WP_DM_OD_HI           PIN_DM_OD_HI
        #define nNOR_WP_DM_STRONG          PIN_DM_STRONG
        #define nNOR_WP_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define nNOR_WP_MASK               nNOR_WP__MASK
#define nNOR_WP_SHIFT              nNOR_WP__SHIFT
#define nNOR_WP_WIDTH              1u

/* Interrupt constants */
#if defined(nNOR_WP__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in nNOR_WP_SetInterruptMode() function.
     *  @{
     */
        #define nNOR_WP_INTR_NONE      (uint16)(0x0000u)
        #define nNOR_WP_INTR_RISING    (uint16)(0x0001u)
        #define nNOR_WP_INTR_FALLING   (uint16)(0x0002u)
        #define nNOR_WP_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define nNOR_WP_INTR_MASK      (0x01u) 
#endif /* (nNOR_WP__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define nNOR_WP_PS                     (* (reg8 *) nNOR_WP__PS)
/* Data Register */
#define nNOR_WP_DR                     (* (reg8 *) nNOR_WP__DR)
/* Port Number */
#define nNOR_WP_PRT_NUM                (* (reg8 *) nNOR_WP__PRT) 
/* Connect to Analog Globals */                                                  
#define nNOR_WP_AG                     (* (reg8 *) nNOR_WP__AG)                       
/* Analog MUX bux enable */
#define nNOR_WP_AMUX                   (* (reg8 *) nNOR_WP__AMUX) 
/* Bidirectional Enable */                                                        
#define nNOR_WP_BIE                    (* (reg8 *) nNOR_WP__BIE)
/* Bit-mask for Aliased Register Access */
#define nNOR_WP_BIT_MASK               (* (reg8 *) nNOR_WP__BIT_MASK)
/* Bypass Enable */
#define nNOR_WP_BYP                    (* (reg8 *) nNOR_WP__BYP)
/* Port wide control signals */                                                   
#define nNOR_WP_CTL                    (* (reg8 *) nNOR_WP__CTL)
/* Drive Modes */
#define nNOR_WP_DM0                    (* (reg8 *) nNOR_WP__DM0) 
#define nNOR_WP_DM1                    (* (reg8 *) nNOR_WP__DM1)
#define nNOR_WP_DM2                    (* (reg8 *) nNOR_WP__DM2) 
/* Input Buffer Disable Override */
#define nNOR_WP_INP_DIS                (* (reg8 *) nNOR_WP__INP_DIS)
/* LCD Common or Segment Drive */
#define nNOR_WP_LCD_COM_SEG            (* (reg8 *) nNOR_WP__LCD_COM_SEG)
/* Enable Segment LCD */
#define nNOR_WP_LCD_EN                 (* (reg8 *) nNOR_WP__LCD_EN)
/* Slew Rate Control */
#define nNOR_WP_SLW                    (* (reg8 *) nNOR_WP__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define nNOR_WP_PRTDSI__CAPS_SEL       (* (reg8 *) nNOR_WP__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define nNOR_WP_PRTDSI__DBL_SYNC_IN    (* (reg8 *) nNOR_WP__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define nNOR_WP_PRTDSI__OE_SEL0        (* (reg8 *) nNOR_WP__PRTDSI__OE_SEL0) 
#define nNOR_WP_PRTDSI__OE_SEL1        (* (reg8 *) nNOR_WP__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define nNOR_WP_PRTDSI__OUT_SEL0       (* (reg8 *) nNOR_WP__PRTDSI__OUT_SEL0) 
#define nNOR_WP_PRTDSI__OUT_SEL1       (* (reg8 *) nNOR_WP__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define nNOR_WP_PRTDSI__SYNC_OUT       (* (reg8 *) nNOR_WP__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(nNOR_WP__SIO_CFG)
    #define nNOR_WP_SIO_HYST_EN        (* (reg8 *) nNOR_WP__SIO_HYST_EN)
    #define nNOR_WP_SIO_REG_HIFREQ     (* (reg8 *) nNOR_WP__SIO_REG_HIFREQ)
    #define nNOR_WP_SIO_CFG            (* (reg8 *) nNOR_WP__SIO_CFG)
    #define nNOR_WP_SIO_DIFF           (* (reg8 *) nNOR_WP__SIO_DIFF)
#endif /* (nNOR_WP__SIO_CFG) */

/* Interrupt Registers */
#if defined(nNOR_WP__INTSTAT)
    #define nNOR_WP_INTSTAT            (* (reg8 *) nNOR_WP__INTSTAT)
    #define nNOR_WP_SNAP               (* (reg8 *) nNOR_WP__SNAP)
    
	#define nNOR_WP_0_INTTYPE_REG 		(* (reg8 *) nNOR_WP__0__INTTYPE)
#endif /* (nNOR_WP__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_nNOR_WP_H */


/* [] END OF FILE */
