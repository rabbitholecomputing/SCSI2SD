/*******************************************************************************
* File Name: NOR_SO.h  
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

#if !defined(CY_PINS_NOR_SO_H) /* Pins NOR_SO_H */
#define CY_PINS_NOR_SO_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "NOR_SO_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 NOR_SO__PORT == 15 && ((NOR_SO__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    NOR_SO_Write(uint8 value);
void    NOR_SO_SetDriveMode(uint8 mode);
uint8   NOR_SO_ReadDataReg(void);
uint8   NOR_SO_Read(void);
void    NOR_SO_SetInterruptMode(uint16 position, uint16 mode);
uint8   NOR_SO_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the NOR_SO_SetDriveMode() function.
     *  @{
     */
        #define NOR_SO_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define NOR_SO_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define NOR_SO_DM_RES_UP          PIN_DM_RES_UP
        #define NOR_SO_DM_RES_DWN         PIN_DM_RES_DWN
        #define NOR_SO_DM_OD_LO           PIN_DM_OD_LO
        #define NOR_SO_DM_OD_HI           PIN_DM_OD_HI
        #define NOR_SO_DM_STRONG          PIN_DM_STRONG
        #define NOR_SO_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define NOR_SO_MASK               NOR_SO__MASK
#define NOR_SO_SHIFT              NOR_SO__SHIFT
#define NOR_SO_WIDTH              1u

/* Interrupt constants */
#if defined(NOR_SO__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in NOR_SO_SetInterruptMode() function.
     *  @{
     */
        #define NOR_SO_INTR_NONE      (uint16)(0x0000u)
        #define NOR_SO_INTR_RISING    (uint16)(0x0001u)
        #define NOR_SO_INTR_FALLING   (uint16)(0x0002u)
        #define NOR_SO_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define NOR_SO_INTR_MASK      (0x01u) 
#endif /* (NOR_SO__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define NOR_SO_PS                     (* (reg8 *) NOR_SO__PS)
/* Data Register */
#define NOR_SO_DR                     (* (reg8 *) NOR_SO__DR)
/* Port Number */
#define NOR_SO_PRT_NUM                (* (reg8 *) NOR_SO__PRT) 
/* Connect to Analog Globals */                                                  
#define NOR_SO_AG                     (* (reg8 *) NOR_SO__AG)                       
/* Analog MUX bux enable */
#define NOR_SO_AMUX                   (* (reg8 *) NOR_SO__AMUX) 
/* Bidirectional Enable */                                                        
#define NOR_SO_BIE                    (* (reg8 *) NOR_SO__BIE)
/* Bit-mask for Aliased Register Access */
#define NOR_SO_BIT_MASK               (* (reg8 *) NOR_SO__BIT_MASK)
/* Bypass Enable */
#define NOR_SO_BYP                    (* (reg8 *) NOR_SO__BYP)
/* Port wide control signals */                                                   
#define NOR_SO_CTL                    (* (reg8 *) NOR_SO__CTL)
/* Drive Modes */
#define NOR_SO_DM0                    (* (reg8 *) NOR_SO__DM0) 
#define NOR_SO_DM1                    (* (reg8 *) NOR_SO__DM1)
#define NOR_SO_DM2                    (* (reg8 *) NOR_SO__DM2) 
/* Input Buffer Disable Override */
#define NOR_SO_INP_DIS                (* (reg8 *) NOR_SO__INP_DIS)
/* LCD Common or Segment Drive */
#define NOR_SO_LCD_COM_SEG            (* (reg8 *) NOR_SO__LCD_COM_SEG)
/* Enable Segment LCD */
#define NOR_SO_LCD_EN                 (* (reg8 *) NOR_SO__LCD_EN)
/* Slew Rate Control */
#define NOR_SO_SLW                    (* (reg8 *) NOR_SO__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define NOR_SO_PRTDSI__CAPS_SEL       (* (reg8 *) NOR_SO__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define NOR_SO_PRTDSI__DBL_SYNC_IN    (* (reg8 *) NOR_SO__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define NOR_SO_PRTDSI__OE_SEL0        (* (reg8 *) NOR_SO__PRTDSI__OE_SEL0) 
#define NOR_SO_PRTDSI__OE_SEL1        (* (reg8 *) NOR_SO__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define NOR_SO_PRTDSI__OUT_SEL0       (* (reg8 *) NOR_SO__PRTDSI__OUT_SEL0) 
#define NOR_SO_PRTDSI__OUT_SEL1       (* (reg8 *) NOR_SO__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define NOR_SO_PRTDSI__SYNC_OUT       (* (reg8 *) NOR_SO__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(NOR_SO__SIO_CFG)
    #define NOR_SO_SIO_HYST_EN        (* (reg8 *) NOR_SO__SIO_HYST_EN)
    #define NOR_SO_SIO_REG_HIFREQ     (* (reg8 *) NOR_SO__SIO_REG_HIFREQ)
    #define NOR_SO_SIO_CFG            (* (reg8 *) NOR_SO__SIO_CFG)
    #define NOR_SO_SIO_DIFF           (* (reg8 *) NOR_SO__SIO_DIFF)
#endif /* (NOR_SO__SIO_CFG) */

/* Interrupt Registers */
#if defined(NOR_SO__INTSTAT)
    #define NOR_SO_INTSTAT            (* (reg8 *) NOR_SO__INTSTAT)
    #define NOR_SO_SNAP               (* (reg8 *) NOR_SO__SNAP)
    
	#define NOR_SO_0_INTTYPE_REG 		(* (reg8 *) NOR_SO__0__INTTYPE)
#endif /* (NOR_SO__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_NOR_SO_H */


/* [] END OF FILE */
