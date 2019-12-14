/*******************************************************************************
* File Name: WiFiSCLK.h  
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

#if !defined(CY_PINS_WiFiSCLK_H) /* Pins WiFiSCLK_H */
#define CY_PINS_WiFiSCLK_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "WiFiSCLK_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 WiFiSCLK__PORT == 15 && ((WiFiSCLK__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    WiFiSCLK_Write(uint8 value);
void    WiFiSCLK_SetDriveMode(uint8 mode);
uint8   WiFiSCLK_ReadDataReg(void);
uint8   WiFiSCLK_Read(void);
void    WiFiSCLK_SetInterruptMode(uint16 position, uint16 mode);
uint8   WiFiSCLK_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the WiFiSCLK_SetDriveMode() function.
     *  @{
     */
        #define WiFiSCLK_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define WiFiSCLK_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define WiFiSCLK_DM_RES_UP          PIN_DM_RES_UP
        #define WiFiSCLK_DM_RES_DWN         PIN_DM_RES_DWN
        #define WiFiSCLK_DM_OD_LO           PIN_DM_OD_LO
        #define WiFiSCLK_DM_OD_HI           PIN_DM_OD_HI
        #define WiFiSCLK_DM_STRONG          PIN_DM_STRONG
        #define WiFiSCLK_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define WiFiSCLK_MASK               WiFiSCLK__MASK
#define WiFiSCLK_SHIFT              WiFiSCLK__SHIFT
#define WiFiSCLK_WIDTH              1u

/* Interrupt constants */
#if defined(WiFiSCLK__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in WiFiSCLK_SetInterruptMode() function.
     *  @{
     */
        #define WiFiSCLK_INTR_NONE      (uint16)(0x0000u)
        #define WiFiSCLK_INTR_RISING    (uint16)(0x0001u)
        #define WiFiSCLK_INTR_FALLING   (uint16)(0x0002u)
        #define WiFiSCLK_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define WiFiSCLK_INTR_MASK      (0x01u) 
#endif /* (WiFiSCLK__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define WiFiSCLK_PS                     (* (reg8 *) WiFiSCLK__PS)
/* Data Register */
#define WiFiSCLK_DR                     (* (reg8 *) WiFiSCLK__DR)
/* Port Number */
#define WiFiSCLK_PRT_NUM                (* (reg8 *) WiFiSCLK__PRT) 
/* Connect to Analog Globals */                                                  
#define WiFiSCLK_AG                     (* (reg8 *) WiFiSCLK__AG)                       
/* Analog MUX bux enable */
#define WiFiSCLK_AMUX                   (* (reg8 *) WiFiSCLK__AMUX) 
/* Bidirectional Enable */                                                        
#define WiFiSCLK_BIE                    (* (reg8 *) WiFiSCLK__BIE)
/* Bit-mask for Aliased Register Access */
#define WiFiSCLK_BIT_MASK               (* (reg8 *) WiFiSCLK__BIT_MASK)
/* Bypass Enable */
#define WiFiSCLK_BYP                    (* (reg8 *) WiFiSCLK__BYP)
/* Port wide control signals */                                                   
#define WiFiSCLK_CTL                    (* (reg8 *) WiFiSCLK__CTL)
/* Drive Modes */
#define WiFiSCLK_DM0                    (* (reg8 *) WiFiSCLK__DM0) 
#define WiFiSCLK_DM1                    (* (reg8 *) WiFiSCLK__DM1)
#define WiFiSCLK_DM2                    (* (reg8 *) WiFiSCLK__DM2) 
/* Input Buffer Disable Override */
#define WiFiSCLK_INP_DIS                (* (reg8 *) WiFiSCLK__INP_DIS)
/* LCD Common or Segment Drive */
#define WiFiSCLK_LCD_COM_SEG            (* (reg8 *) WiFiSCLK__LCD_COM_SEG)
/* Enable Segment LCD */
#define WiFiSCLK_LCD_EN                 (* (reg8 *) WiFiSCLK__LCD_EN)
/* Slew Rate Control */
#define WiFiSCLK_SLW                    (* (reg8 *) WiFiSCLK__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define WiFiSCLK_PRTDSI__CAPS_SEL       (* (reg8 *) WiFiSCLK__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define WiFiSCLK_PRTDSI__DBL_SYNC_IN    (* (reg8 *) WiFiSCLK__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define WiFiSCLK_PRTDSI__OE_SEL0        (* (reg8 *) WiFiSCLK__PRTDSI__OE_SEL0) 
#define WiFiSCLK_PRTDSI__OE_SEL1        (* (reg8 *) WiFiSCLK__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define WiFiSCLK_PRTDSI__OUT_SEL0       (* (reg8 *) WiFiSCLK__PRTDSI__OUT_SEL0) 
#define WiFiSCLK_PRTDSI__OUT_SEL1       (* (reg8 *) WiFiSCLK__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define WiFiSCLK_PRTDSI__SYNC_OUT       (* (reg8 *) WiFiSCLK__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(WiFiSCLK__SIO_CFG)
    #define WiFiSCLK_SIO_HYST_EN        (* (reg8 *) WiFiSCLK__SIO_HYST_EN)
    #define WiFiSCLK_SIO_REG_HIFREQ     (* (reg8 *) WiFiSCLK__SIO_REG_HIFREQ)
    #define WiFiSCLK_SIO_CFG            (* (reg8 *) WiFiSCLK__SIO_CFG)
    #define WiFiSCLK_SIO_DIFF           (* (reg8 *) WiFiSCLK__SIO_DIFF)
#endif /* (WiFiSCLK__SIO_CFG) */

/* Interrupt Registers */
#if defined(WiFiSCLK__INTSTAT)
    #define WiFiSCLK_INTSTAT            (* (reg8 *) WiFiSCLK__INTSTAT)
    #define WiFiSCLK_SNAP               (* (reg8 *) WiFiSCLK__SNAP)
    
	#define WiFiSCLK_0_INTTYPE_REG 		(* (reg8 *) WiFiSCLK__0__INTTYPE)
#endif /* (WiFiSCLK__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_WiFiSCLK_H */


/* [] END OF FILE */
