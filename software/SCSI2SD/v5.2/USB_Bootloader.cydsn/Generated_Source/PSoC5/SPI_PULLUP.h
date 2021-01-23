/*******************************************************************************
* File Name: SPI_PULLUP.h  
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

#if !defined(CY_PINS_SPI_PULLUP_H) /* Pins SPI_PULLUP_H */
#define CY_PINS_SPI_PULLUP_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "SPI_PULLUP_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 SPI_PULLUP__PORT == 15 && ((SPI_PULLUP__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    SPI_PULLUP_Write(uint8 value);
void    SPI_PULLUP_SetDriveMode(uint8 mode);
uint8   SPI_PULLUP_ReadDataReg(void);
uint8   SPI_PULLUP_Read(void);
void    SPI_PULLUP_SetInterruptMode(uint16 position, uint16 mode);
uint8   SPI_PULLUP_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the SPI_PULLUP_SetDriveMode() function.
     *  @{
     */
        #define SPI_PULLUP_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define SPI_PULLUP_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define SPI_PULLUP_DM_RES_UP          PIN_DM_RES_UP
        #define SPI_PULLUP_DM_RES_DWN         PIN_DM_RES_DWN
        #define SPI_PULLUP_DM_OD_LO           PIN_DM_OD_LO
        #define SPI_PULLUP_DM_OD_HI           PIN_DM_OD_HI
        #define SPI_PULLUP_DM_STRONG          PIN_DM_STRONG
        #define SPI_PULLUP_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define SPI_PULLUP_MASK               SPI_PULLUP__MASK
#define SPI_PULLUP_SHIFT              SPI_PULLUP__SHIFT
#define SPI_PULLUP_WIDTH              4u

/* Interrupt constants */
#if defined(SPI_PULLUP__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in SPI_PULLUP_SetInterruptMode() function.
     *  @{
     */
        #define SPI_PULLUP_INTR_NONE      (uint16)(0x0000u)
        #define SPI_PULLUP_INTR_RISING    (uint16)(0x0001u)
        #define SPI_PULLUP_INTR_FALLING   (uint16)(0x0002u)
        #define SPI_PULLUP_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define SPI_PULLUP_INTR_MASK      (0x01u) 
#endif /* (SPI_PULLUP__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define SPI_PULLUP_PS                     (* (reg8 *) SPI_PULLUP__PS)
/* Data Register */
#define SPI_PULLUP_DR                     (* (reg8 *) SPI_PULLUP__DR)
/* Port Number */
#define SPI_PULLUP_PRT_NUM                (* (reg8 *) SPI_PULLUP__PRT) 
/* Connect to Analog Globals */                                                  
#define SPI_PULLUP_AG                     (* (reg8 *) SPI_PULLUP__AG)                       
/* Analog MUX bux enable */
#define SPI_PULLUP_AMUX                   (* (reg8 *) SPI_PULLUP__AMUX) 
/* Bidirectional Enable */                                                        
#define SPI_PULLUP_BIE                    (* (reg8 *) SPI_PULLUP__BIE)
/* Bit-mask for Aliased Register Access */
#define SPI_PULLUP_BIT_MASK               (* (reg8 *) SPI_PULLUP__BIT_MASK)
/* Bypass Enable */
#define SPI_PULLUP_BYP                    (* (reg8 *) SPI_PULLUP__BYP)
/* Port wide control signals */                                                   
#define SPI_PULLUP_CTL                    (* (reg8 *) SPI_PULLUP__CTL)
/* Drive Modes */
#define SPI_PULLUP_DM0                    (* (reg8 *) SPI_PULLUP__DM0) 
#define SPI_PULLUP_DM1                    (* (reg8 *) SPI_PULLUP__DM1)
#define SPI_PULLUP_DM2                    (* (reg8 *) SPI_PULLUP__DM2) 
/* Input Buffer Disable Override */
#define SPI_PULLUP_INP_DIS                (* (reg8 *) SPI_PULLUP__INP_DIS)
/* LCD Common or Segment Drive */
#define SPI_PULLUP_LCD_COM_SEG            (* (reg8 *) SPI_PULLUP__LCD_COM_SEG)
/* Enable Segment LCD */
#define SPI_PULLUP_LCD_EN                 (* (reg8 *) SPI_PULLUP__LCD_EN)
/* Slew Rate Control */
#define SPI_PULLUP_SLW                    (* (reg8 *) SPI_PULLUP__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define SPI_PULLUP_PRTDSI__CAPS_SEL       (* (reg8 *) SPI_PULLUP__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define SPI_PULLUP_PRTDSI__DBL_SYNC_IN    (* (reg8 *) SPI_PULLUP__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define SPI_PULLUP_PRTDSI__OE_SEL0        (* (reg8 *) SPI_PULLUP__PRTDSI__OE_SEL0) 
#define SPI_PULLUP_PRTDSI__OE_SEL1        (* (reg8 *) SPI_PULLUP__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define SPI_PULLUP_PRTDSI__OUT_SEL0       (* (reg8 *) SPI_PULLUP__PRTDSI__OUT_SEL0) 
#define SPI_PULLUP_PRTDSI__OUT_SEL1       (* (reg8 *) SPI_PULLUP__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define SPI_PULLUP_PRTDSI__SYNC_OUT       (* (reg8 *) SPI_PULLUP__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(SPI_PULLUP__SIO_CFG)
    #define SPI_PULLUP_SIO_HYST_EN        (* (reg8 *) SPI_PULLUP__SIO_HYST_EN)
    #define SPI_PULLUP_SIO_REG_HIFREQ     (* (reg8 *) SPI_PULLUP__SIO_REG_HIFREQ)
    #define SPI_PULLUP_SIO_CFG            (* (reg8 *) SPI_PULLUP__SIO_CFG)
    #define SPI_PULLUP_SIO_DIFF           (* (reg8 *) SPI_PULLUP__SIO_DIFF)
#endif /* (SPI_PULLUP__SIO_CFG) */

/* Interrupt Registers */
#if defined(SPI_PULLUP__INTSTAT)
    #define SPI_PULLUP_INTSTAT            (* (reg8 *) SPI_PULLUP__INTSTAT)
    #define SPI_PULLUP_SNAP               (* (reg8 *) SPI_PULLUP__SNAP)
    
	#define SPI_PULLUP_0_INTTYPE_REG 		(* (reg8 *) SPI_PULLUP__0__INTTYPE)
	#define SPI_PULLUP_1_INTTYPE_REG 		(* (reg8 *) SPI_PULLUP__1__INTTYPE)
	#define SPI_PULLUP_2_INTTYPE_REG 		(* (reg8 *) SPI_PULLUP__2__INTTYPE)
	#define SPI_PULLUP_3_INTTYPE_REG 		(* (reg8 *) SPI_PULLUP__3__INTTYPE)
#endif /* (SPI_PULLUP__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_SPI_PULLUP_H */


/* [] END OF FILE */
