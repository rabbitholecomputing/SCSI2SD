/*******************************************************************************
* File Name: WiFiMOSI.h  
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

#if !defined(CY_PINS_WiFiMOSI_H) /* Pins WiFiMOSI_H */
#define CY_PINS_WiFiMOSI_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "WiFiMOSI_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 WiFiMOSI__PORT == 15 && ((WiFiMOSI__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    WiFiMOSI_Write(uint8 value);
void    WiFiMOSI_SetDriveMode(uint8 mode);
uint8   WiFiMOSI_ReadDataReg(void);
uint8   WiFiMOSI_Read(void);
void    WiFiMOSI_SetInterruptMode(uint16 position, uint16 mode);
uint8   WiFiMOSI_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the WiFiMOSI_SetDriveMode() function.
     *  @{
     */
        #define WiFiMOSI_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define WiFiMOSI_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define WiFiMOSI_DM_RES_UP          PIN_DM_RES_UP
        #define WiFiMOSI_DM_RES_DWN         PIN_DM_RES_DWN
        #define WiFiMOSI_DM_OD_LO           PIN_DM_OD_LO
        #define WiFiMOSI_DM_OD_HI           PIN_DM_OD_HI
        #define WiFiMOSI_DM_STRONG          PIN_DM_STRONG
        #define WiFiMOSI_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define WiFiMOSI_MASK               WiFiMOSI__MASK
#define WiFiMOSI_SHIFT              WiFiMOSI__SHIFT
#define WiFiMOSI_WIDTH              1u

/* Interrupt constants */
#if defined(WiFiMOSI__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in WiFiMOSI_SetInterruptMode() function.
     *  @{
     */
        #define WiFiMOSI_INTR_NONE      (uint16)(0x0000u)
        #define WiFiMOSI_INTR_RISING    (uint16)(0x0001u)
        #define WiFiMOSI_INTR_FALLING   (uint16)(0x0002u)
        #define WiFiMOSI_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define WiFiMOSI_INTR_MASK      (0x01u) 
#endif /* (WiFiMOSI__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define WiFiMOSI_PS                     (* (reg8 *) WiFiMOSI__PS)
/* Data Register */
#define WiFiMOSI_DR                     (* (reg8 *) WiFiMOSI__DR)
/* Port Number */
#define WiFiMOSI_PRT_NUM                (* (reg8 *) WiFiMOSI__PRT) 
/* Connect to Analog Globals */                                                  
#define WiFiMOSI_AG                     (* (reg8 *) WiFiMOSI__AG)                       
/* Analog MUX bux enable */
#define WiFiMOSI_AMUX                   (* (reg8 *) WiFiMOSI__AMUX) 
/* Bidirectional Enable */                                                        
#define WiFiMOSI_BIE                    (* (reg8 *) WiFiMOSI__BIE)
/* Bit-mask for Aliased Register Access */
#define WiFiMOSI_BIT_MASK               (* (reg8 *) WiFiMOSI__BIT_MASK)
/* Bypass Enable */
#define WiFiMOSI_BYP                    (* (reg8 *) WiFiMOSI__BYP)
/* Port wide control signals */                                                   
#define WiFiMOSI_CTL                    (* (reg8 *) WiFiMOSI__CTL)
/* Drive Modes */
#define WiFiMOSI_DM0                    (* (reg8 *) WiFiMOSI__DM0) 
#define WiFiMOSI_DM1                    (* (reg8 *) WiFiMOSI__DM1)
#define WiFiMOSI_DM2                    (* (reg8 *) WiFiMOSI__DM2) 
/* Input Buffer Disable Override */
#define WiFiMOSI_INP_DIS                (* (reg8 *) WiFiMOSI__INP_DIS)
/* LCD Common or Segment Drive */
#define WiFiMOSI_LCD_COM_SEG            (* (reg8 *) WiFiMOSI__LCD_COM_SEG)
/* Enable Segment LCD */
#define WiFiMOSI_LCD_EN                 (* (reg8 *) WiFiMOSI__LCD_EN)
/* Slew Rate Control */
#define WiFiMOSI_SLW                    (* (reg8 *) WiFiMOSI__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define WiFiMOSI_PRTDSI__CAPS_SEL       (* (reg8 *) WiFiMOSI__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define WiFiMOSI_PRTDSI__DBL_SYNC_IN    (* (reg8 *) WiFiMOSI__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define WiFiMOSI_PRTDSI__OE_SEL0        (* (reg8 *) WiFiMOSI__PRTDSI__OE_SEL0) 
#define WiFiMOSI_PRTDSI__OE_SEL1        (* (reg8 *) WiFiMOSI__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define WiFiMOSI_PRTDSI__OUT_SEL0       (* (reg8 *) WiFiMOSI__PRTDSI__OUT_SEL0) 
#define WiFiMOSI_PRTDSI__OUT_SEL1       (* (reg8 *) WiFiMOSI__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define WiFiMOSI_PRTDSI__SYNC_OUT       (* (reg8 *) WiFiMOSI__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(WiFiMOSI__SIO_CFG)
    #define WiFiMOSI_SIO_HYST_EN        (* (reg8 *) WiFiMOSI__SIO_HYST_EN)
    #define WiFiMOSI_SIO_REG_HIFREQ     (* (reg8 *) WiFiMOSI__SIO_REG_HIFREQ)
    #define WiFiMOSI_SIO_CFG            (* (reg8 *) WiFiMOSI__SIO_CFG)
    #define WiFiMOSI_SIO_DIFF           (* (reg8 *) WiFiMOSI__SIO_DIFF)
#endif /* (WiFiMOSI__SIO_CFG) */

/* Interrupt Registers */
#if defined(WiFiMOSI__INTSTAT)
    #define WiFiMOSI_INTSTAT            (* (reg8 *) WiFiMOSI__INTSTAT)
    #define WiFiMOSI_SNAP               (* (reg8 *) WiFiMOSI__SNAP)
    
	#define WiFiMOSI_0_INTTYPE_REG 		(* (reg8 *) WiFiMOSI__0__INTTYPE)
#endif /* (WiFiMOSI__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_WiFiMOSI_H */


/* [] END OF FILE */
