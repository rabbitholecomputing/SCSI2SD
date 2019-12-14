/*******************************************************************************
* File Name: WiFiMISO.h  
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

#if !defined(CY_PINS_WiFiMISO_H) /* Pins WiFiMISO_H */
#define CY_PINS_WiFiMISO_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "WiFiMISO_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 WiFiMISO__PORT == 15 && ((WiFiMISO__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    WiFiMISO_Write(uint8 value);
void    WiFiMISO_SetDriveMode(uint8 mode);
uint8   WiFiMISO_ReadDataReg(void);
uint8   WiFiMISO_Read(void);
void    WiFiMISO_SetInterruptMode(uint16 position, uint16 mode);
uint8   WiFiMISO_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the WiFiMISO_SetDriveMode() function.
     *  @{
     */
        #define WiFiMISO_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define WiFiMISO_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define WiFiMISO_DM_RES_UP          PIN_DM_RES_UP
        #define WiFiMISO_DM_RES_DWN         PIN_DM_RES_DWN
        #define WiFiMISO_DM_OD_LO           PIN_DM_OD_LO
        #define WiFiMISO_DM_OD_HI           PIN_DM_OD_HI
        #define WiFiMISO_DM_STRONG          PIN_DM_STRONG
        #define WiFiMISO_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define WiFiMISO_MASK               WiFiMISO__MASK
#define WiFiMISO_SHIFT              WiFiMISO__SHIFT
#define WiFiMISO_WIDTH              1u

/* Interrupt constants */
#if defined(WiFiMISO__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in WiFiMISO_SetInterruptMode() function.
     *  @{
     */
        #define WiFiMISO_INTR_NONE      (uint16)(0x0000u)
        #define WiFiMISO_INTR_RISING    (uint16)(0x0001u)
        #define WiFiMISO_INTR_FALLING   (uint16)(0x0002u)
        #define WiFiMISO_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define WiFiMISO_INTR_MASK      (0x01u) 
#endif /* (WiFiMISO__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define WiFiMISO_PS                     (* (reg8 *) WiFiMISO__PS)
/* Data Register */
#define WiFiMISO_DR                     (* (reg8 *) WiFiMISO__DR)
/* Port Number */
#define WiFiMISO_PRT_NUM                (* (reg8 *) WiFiMISO__PRT) 
/* Connect to Analog Globals */                                                  
#define WiFiMISO_AG                     (* (reg8 *) WiFiMISO__AG)                       
/* Analog MUX bux enable */
#define WiFiMISO_AMUX                   (* (reg8 *) WiFiMISO__AMUX) 
/* Bidirectional Enable */                                                        
#define WiFiMISO_BIE                    (* (reg8 *) WiFiMISO__BIE)
/* Bit-mask for Aliased Register Access */
#define WiFiMISO_BIT_MASK               (* (reg8 *) WiFiMISO__BIT_MASK)
/* Bypass Enable */
#define WiFiMISO_BYP                    (* (reg8 *) WiFiMISO__BYP)
/* Port wide control signals */                                                   
#define WiFiMISO_CTL                    (* (reg8 *) WiFiMISO__CTL)
/* Drive Modes */
#define WiFiMISO_DM0                    (* (reg8 *) WiFiMISO__DM0) 
#define WiFiMISO_DM1                    (* (reg8 *) WiFiMISO__DM1)
#define WiFiMISO_DM2                    (* (reg8 *) WiFiMISO__DM2) 
/* Input Buffer Disable Override */
#define WiFiMISO_INP_DIS                (* (reg8 *) WiFiMISO__INP_DIS)
/* LCD Common or Segment Drive */
#define WiFiMISO_LCD_COM_SEG            (* (reg8 *) WiFiMISO__LCD_COM_SEG)
/* Enable Segment LCD */
#define WiFiMISO_LCD_EN                 (* (reg8 *) WiFiMISO__LCD_EN)
/* Slew Rate Control */
#define WiFiMISO_SLW                    (* (reg8 *) WiFiMISO__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define WiFiMISO_PRTDSI__CAPS_SEL       (* (reg8 *) WiFiMISO__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define WiFiMISO_PRTDSI__DBL_SYNC_IN    (* (reg8 *) WiFiMISO__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define WiFiMISO_PRTDSI__OE_SEL0        (* (reg8 *) WiFiMISO__PRTDSI__OE_SEL0) 
#define WiFiMISO_PRTDSI__OE_SEL1        (* (reg8 *) WiFiMISO__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define WiFiMISO_PRTDSI__OUT_SEL0       (* (reg8 *) WiFiMISO__PRTDSI__OUT_SEL0) 
#define WiFiMISO_PRTDSI__OUT_SEL1       (* (reg8 *) WiFiMISO__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define WiFiMISO_PRTDSI__SYNC_OUT       (* (reg8 *) WiFiMISO__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(WiFiMISO__SIO_CFG)
    #define WiFiMISO_SIO_HYST_EN        (* (reg8 *) WiFiMISO__SIO_HYST_EN)
    #define WiFiMISO_SIO_REG_HIFREQ     (* (reg8 *) WiFiMISO__SIO_REG_HIFREQ)
    #define WiFiMISO_SIO_CFG            (* (reg8 *) WiFiMISO__SIO_CFG)
    #define WiFiMISO_SIO_DIFF           (* (reg8 *) WiFiMISO__SIO_DIFF)
#endif /* (WiFiMISO__SIO_CFG) */

/* Interrupt Registers */
#if defined(WiFiMISO__INTSTAT)
    #define WiFiMISO_INTSTAT            (* (reg8 *) WiFiMISO__INTSTAT)
    #define WiFiMISO_SNAP               (* (reg8 *) WiFiMISO__SNAP)
    
	#define WiFiMISO_0_INTTYPE_REG 		(* (reg8 *) WiFiMISO__0__INTTYPE)
#endif /* (WiFiMISO__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_WiFiMISO_H */


/* [] END OF FILE */
