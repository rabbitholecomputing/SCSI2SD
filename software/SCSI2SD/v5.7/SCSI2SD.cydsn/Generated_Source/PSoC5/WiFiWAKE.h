/*******************************************************************************
* File Name: WiFiWAKE.h  
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

#if !defined(CY_PINS_WiFiWAKE_H) /* Pins WiFiWAKE_H */
#define CY_PINS_WiFiWAKE_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "WiFiWAKE_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 WiFiWAKE__PORT == 15 && ((WiFiWAKE__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    WiFiWAKE_Write(uint8 value);
void    WiFiWAKE_SetDriveMode(uint8 mode);
uint8   WiFiWAKE_ReadDataReg(void);
uint8   WiFiWAKE_Read(void);
void    WiFiWAKE_SetInterruptMode(uint16 position, uint16 mode);
uint8   WiFiWAKE_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the WiFiWAKE_SetDriveMode() function.
     *  @{
     */
        #define WiFiWAKE_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define WiFiWAKE_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define WiFiWAKE_DM_RES_UP          PIN_DM_RES_UP
        #define WiFiWAKE_DM_RES_DWN         PIN_DM_RES_DWN
        #define WiFiWAKE_DM_OD_LO           PIN_DM_OD_LO
        #define WiFiWAKE_DM_OD_HI           PIN_DM_OD_HI
        #define WiFiWAKE_DM_STRONG          PIN_DM_STRONG
        #define WiFiWAKE_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define WiFiWAKE_MASK               WiFiWAKE__MASK
#define WiFiWAKE_SHIFT              WiFiWAKE__SHIFT
#define WiFiWAKE_WIDTH              1u

/* Interrupt constants */
#if defined(WiFiWAKE__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in WiFiWAKE_SetInterruptMode() function.
     *  @{
     */
        #define WiFiWAKE_INTR_NONE      (uint16)(0x0000u)
        #define WiFiWAKE_INTR_RISING    (uint16)(0x0001u)
        #define WiFiWAKE_INTR_FALLING   (uint16)(0x0002u)
        #define WiFiWAKE_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define WiFiWAKE_INTR_MASK      (0x01u) 
#endif /* (WiFiWAKE__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define WiFiWAKE_PS                     (* (reg8 *) WiFiWAKE__PS)
/* Data Register */
#define WiFiWAKE_DR                     (* (reg8 *) WiFiWAKE__DR)
/* Port Number */
#define WiFiWAKE_PRT_NUM                (* (reg8 *) WiFiWAKE__PRT) 
/* Connect to Analog Globals */                                                  
#define WiFiWAKE_AG                     (* (reg8 *) WiFiWAKE__AG)                       
/* Analog MUX bux enable */
#define WiFiWAKE_AMUX                   (* (reg8 *) WiFiWAKE__AMUX) 
/* Bidirectional Enable */                                                        
#define WiFiWAKE_BIE                    (* (reg8 *) WiFiWAKE__BIE)
/* Bit-mask for Aliased Register Access */
#define WiFiWAKE_BIT_MASK               (* (reg8 *) WiFiWAKE__BIT_MASK)
/* Bypass Enable */
#define WiFiWAKE_BYP                    (* (reg8 *) WiFiWAKE__BYP)
/* Port wide control signals */                                                   
#define WiFiWAKE_CTL                    (* (reg8 *) WiFiWAKE__CTL)
/* Drive Modes */
#define WiFiWAKE_DM0                    (* (reg8 *) WiFiWAKE__DM0) 
#define WiFiWAKE_DM1                    (* (reg8 *) WiFiWAKE__DM1)
#define WiFiWAKE_DM2                    (* (reg8 *) WiFiWAKE__DM2) 
/* Input Buffer Disable Override */
#define WiFiWAKE_INP_DIS                (* (reg8 *) WiFiWAKE__INP_DIS)
/* LCD Common or Segment Drive */
#define WiFiWAKE_LCD_COM_SEG            (* (reg8 *) WiFiWAKE__LCD_COM_SEG)
/* Enable Segment LCD */
#define WiFiWAKE_LCD_EN                 (* (reg8 *) WiFiWAKE__LCD_EN)
/* Slew Rate Control */
#define WiFiWAKE_SLW                    (* (reg8 *) WiFiWAKE__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define WiFiWAKE_PRTDSI__CAPS_SEL       (* (reg8 *) WiFiWAKE__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define WiFiWAKE_PRTDSI__DBL_SYNC_IN    (* (reg8 *) WiFiWAKE__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define WiFiWAKE_PRTDSI__OE_SEL0        (* (reg8 *) WiFiWAKE__PRTDSI__OE_SEL0) 
#define WiFiWAKE_PRTDSI__OE_SEL1        (* (reg8 *) WiFiWAKE__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define WiFiWAKE_PRTDSI__OUT_SEL0       (* (reg8 *) WiFiWAKE__PRTDSI__OUT_SEL0) 
#define WiFiWAKE_PRTDSI__OUT_SEL1       (* (reg8 *) WiFiWAKE__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define WiFiWAKE_PRTDSI__SYNC_OUT       (* (reg8 *) WiFiWAKE__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(WiFiWAKE__SIO_CFG)
    #define WiFiWAKE_SIO_HYST_EN        (* (reg8 *) WiFiWAKE__SIO_HYST_EN)
    #define WiFiWAKE_SIO_REG_HIFREQ     (* (reg8 *) WiFiWAKE__SIO_REG_HIFREQ)
    #define WiFiWAKE_SIO_CFG            (* (reg8 *) WiFiWAKE__SIO_CFG)
    #define WiFiWAKE_SIO_DIFF           (* (reg8 *) WiFiWAKE__SIO_DIFF)
#endif /* (WiFiWAKE__SIO_CFG) */

/* Interrupt Registers */
#if defined(WiFiWAKE__INTSTAT)
    #define WiFiWAKE_INTSTAT            (* (reg8 *) WiFiWAKE__INTSTAT)
    #define WiFiWAKE_SNAP               (* (reg8 *) WiFiWAKE__SNAP)
    
	#define WiFiWAKE_0_INTTYPE_REG 		(* (reg8 *) WiFiWAKE__0__INTTYPE)
#endif /* (WiFiWAKE__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_WiFiWAKE_H */


/* [] END OF FILE */
