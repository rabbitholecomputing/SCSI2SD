/*******************************************************************************
* File Name: WiFiSS.h  
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

#if !defined(CY_PINS_WiFiSS_H) /* Pins WiFiSS_H */
#define CY_PINS_WiFiSS_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "WiFiSS_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 WiFiSS__PORT == 15 && ((WiFiSS__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    WiFiSS_Write(uint8 value);
void    WiFiSS_SetDriveMode(uint8 mode);
uint8   WiFiSS_ReadDataReg(void);
uint8   WiFiSS_Read(void);
void    WiFiSS_SetInterruptMode(uint16 position, uint16 mode);
uint8   WiFiSS_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the WiFiSS_SetDriveMode() function.
     *  @{
     */
        #define WiFiSS_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define WiFiSS_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define WiFiSS_DM_RES_UP          PIN_DM_RES_UP
        #define WiFiSS_DM_RES_DWN         PIN_DM_RES_DWN
        #define WiFiSS_DM_OD_LO           PIN_DM_OD_LO
        #define WiFiSS_DM_OD_HI           PIN_DM_OD_HI
        #define WiFiSS_DM_STRONG          PIN_DM_STRONG
        #define WiFiSS_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define WiFiSS_MASK               WiFiSS__MASK
#define WiFiSS_SHIFT              WiFiSS__SHIFT
#define WiFiSS_WIDTH              1u

/* Interrupt constants */
#if defined(WiFiSS__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in WiFiSS_SetInterruptMode() function.
     *  @{
     */
        #define WiFiSS_INTR_NONE      (uint16)(0x0000u)
        #define WiFiSS_INTR_RISING    (uint16)(0x0001u)
        #define WiFiSS_INTR_FALLING   (uint16)(0x0002u)
        #define WiFiSS_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define WiFiSS_INTR_MASK      (0x01u) 
#endif /* (WiFiSS__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define WiFiSS_PS                     (* (reg8 *) WiFiSS__PS)
/* Data Register */
#define WiFiSS_DR                     (* (reg8 *) WiFiSS__DR)
/* Port Number */
#define WiFiSS_PRT_NUM                (* (reg8 *) WiFiSS__PRT) 
/* Connect to Analog Globals */                                                  
#define WiFiSS_AG                     (* (reg8 *) WiFiSS__AG)                       
/* Analog MUX bux enable */
#define WiFiSS_AMUX                   (* (reg8 *) WiFiSS__AMUX) 
/* Bidirectional Enable */                                                        
#define WiFiSS_BIE                    (* (reg8 *) WiFiSS__BIE)
/* Bit-mask for Aliased Register Access */
#define WiFiSS_BIT_MASK               (* (reg8 *) WiFiSS__BIT_MASK)
/* Bypass Enable */
#define WiFiSS_BYP                    (* (reg8 *) WiFiSS__BYP)
/* Port wide control signals */                                                   
#define WiFiSS_CTL                    (* (reg8 *) WiFiSS__CTL)
/* Drive Modes */
#define WiFiSS_DM0                    (* (reg8 *) WiFiSS__DM0) 
#define WiFiSS_DM1                    (* (reg8 *) WiFiSS__DM1)
#define WiFiSS_DM2                    (* (reg8 *) WiFiSS__DM2) 
/* Input Buffer Disable Override */
#define WiFiSS_INP_DIS                (* (reg8 *) WiFiSS__INP_DIS)
/* LCD Common or Segment Drive */
#define WiFiSS_LCD_COM_SEG            (* (reg8 *) WiFiSS__LCD_COM_SEG)
/* Enable Segment LCD */
#define WiFiSS_LCD_EN                 (* (reg8 *) WiFiSS__LCD_EN)
/* Slew Rate Control */
#define WiFiSS_SLW                    (* (reg8 *) WiFiSS__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define WiFiSS_PRTDSI__CAPS_SEL       (* (reg8 *) WiFiSS__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define WiFiSS_PRTDSI__DBL_SYNC_IN    (* (reg8 *) WiFiSS__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define WiFiSS_PRTDSI__OE_SEL0        (* (reg8 *) WiFiSS__PRTDSI__OE_SEL0) 
#define WiFiSS_PRTDSI__OE_SEL1        (* (reg8 *) WiFiSS__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define WiFiSS_PRTDSI__OUT_SEL0       (* (reg8 *) WiFiSS__PRTDSI__OUT_SEL0) 
#define WiFiSS_PRTDSI__OUT_SEL1       (* (reg8 *) WiFiSS__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define WiFiSS_PRTDSI__SYNC_OUT       (* (reg8 *) WiFiSS__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(WiFiSS__SIO_CFG)
    #define WiFiSS_SIO_HYST_EN        (* (reg8 *) WiFiSS__SIO_HYST_EN)
    #define WiFiSS_SIO_REG_HIFREQ     (* (reg8 *) WiFiSS__SIO_REG_HIFREQ)
    #define WiFiSS_SIO_CFG            (* (reg8 *) WiFiSS__SIO_CFG)
    #define WiFiSS_SIO_DIFF           (* (reg8 *) WiFiSS__SIO_DIFF)
#endif /* (WiFiSS__SIO_CFG) */

/* Interrupt Registers */
#if defined(WiFiSS__INTSTAT)
    #define WiFiSS_INTSTAT            (* (reg8 *) WiFiSS__INTSTAT)
    #define WiFiSS_SNAP               (* (reg8 *) WiFiSS__SNAP)
    
	#define WiFiSS_0_INTTYPE_REG 		(* (reg8 *) WiFiSS__0__INTTYPE)
#endif /* (WiFiSS__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_WiFiSS_H */


/* [] END OF FILE */
