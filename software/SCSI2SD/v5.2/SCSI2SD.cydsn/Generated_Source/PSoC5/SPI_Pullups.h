/*******************************************************************************
* File Name: SPI_Pullups.h  
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

#if !defined(CY_PINS_SPI_Pullups_H) /* Pins SPI_Pullups_H */
#define CY_PINS_SPI_Pullups_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "SPI_Pullups_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 SPI_Pullups__PORT == 15 && ((SPI_Pullups__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    SPI_Pullups_Write(uint8 value);
void    SPI_Pullups_SetDriveMode(uint8 mode);
uint8   SPI_Pullups_ReadDataReg(void);
uint8   SPI_Pullups_Read(void);
void    SPI_Pullups_SetInterruptMode(uint16 position, uint16 mode);
uint8   SPI_Pullups_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the SPI_Pullups_SetDriveMode() function.
     *  @{
     */
        #define SPI_Pullups_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define SPI_Pullups_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define SPI_Pullups_DM_RES_UP          PIN_DM_RES_UP
        #define SPI_Pullups_DM_RES_DWN         PIN_DM_RES_DWN
        #define SPI_Pullups_DM_OD_LO           PIN_DM_OD_LO
        #define SPI_Pullups_DM_OD_HI           PIN_DM_OD_HI
        #define SPI_Pullups_DM_STRONG          PIN_DM_STRONG
        #define SPI_Pullups_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define SPI_Pullups_MASK               SPI_Pullups__MASK
#define SPI_Pullups_SHIFT              SPI_Pullups__SHIFT
#define SPI_Pullups_WIDTH              4u

/* Interrupt constants */
#if defined(SPI_Pullups__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in SPI_Pullups_SetInterruptMode() function.
     *  @{
     */
        #define SPI_Pullups_INTR_NONE      (uint16)(0x0000u)
        #define SPI_Pullups_INTR_RISING    (uint16)(0x0001u)
        #define SPI_Pullups_INTR_FALLING   (uint16)(0x0002u)
        #define SPI_Pullups_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define SPI_Pullups_INTR_MASK      (0x01u) 
#endif /* (SPI_Pullups__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define SPI_Pullups_PS                     (* (reg8 *) SPI_Pullups__PS)
/* Data Register */
#define SPI_Pullups_DR                     (* (reg8 *) SPI_Pullups__DR)
/* Port Number */
#define SPI_Pullups_PRT_NUM                (* (reg8 *) SPI_Pullups__PRT) 
/* Connect to Analog Globals */                                                  
#define SPI_Pullups_AG                     (* (reg8 *) SPI_Pullups__AG)                       
/* Analog MUX bux enable */
#define SPI_Pullups_AMUX                   (* (reg8 *) SPI_Pullups__AMUX) 
/* Bidirectional Enable */                                                        
#define SPI_Pullups_BIE                    (* (reg8 *) SPI_Pullups__BIE)
/* Bit-mask for Aliased Register Access */
#define SPI_Pullups_BIT_MASK               (* (reg8 *) SPI_Pullups__BIT_MASK)
/* Bypass Enable */
#define SPI_Pullups_BYP                    (* (reg8 *) SPI_Pullups__BYP)
/* Port wide control signals */                                                   
#define SPI_Pullups_CTL                    (* (reg8 *) SPI_Pullups__CTL)
/* Drive Modes */
#define SPI_Pullups_DM0                    (* (reg8 *) SPI_Pullups__DM0) 
#define SPI_Pullups_DM1                    (* (reg8 *) SPI_Pullups__DM1)
#define SPI_Pullups_DM2                    (* (reg8 *) SPI_Pullups__DM2) 
/* Input Buffer Disable Override */
#define SPI_Pullups_INP_DIS                (* (reg8 *) SPI_Pullups__INP_DIS)
/* LCD Common or Segment Drive */
#define SPI_Pullups_LCD_COM_SEG            (* (reg8 *) SPI_Pullups__LCD_COM_SEG)
/* Enable Segment LCD */
#define SPI_Pullups_LCD_EN                 (* (reg8 *) SPI_Pullups__LCD_EN)
/* Slew Rate Control */
#define SPI_Pullups_SLW                    (* (reg8 *) SPI_Pullups__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define SPI_Pullups_PRTDSI__CAPS_SEL       (* (reg8 *) SPI_Pullups__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define SPI_Pullups_PRTDSI__DBL_SYNC_IN    (* (reg8 *) SPI_Pullups__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define SPI_Pullups_PRTDSI__OE_SEL0        (* (reg8 *) SPI_Pullups__PRTDSI__OE_SEL0) 
#define SPI_Pullups_PRTDSI__OE_SEL1        (* (reg8 *) SPI_Pullups__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define SPI_Pullups_PRTDSI__OUT_SEL0       (* (reg8 *) SPI_Pullups__PRTDSI__OUT_SEL0) 
#define SPI_Pullups_PRTDSI__OUT_SEL1       (* (reg8 *) SPI_Pullups__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define SPI_Pullups_PRTDSI__SYNC_OUT       (* (reg8 *) SPI_Pullups__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(SPI_Pullups__SIO_CFG)
    #define SPI_Pullups_SIO_HYST_EN        (* (reg8 *) SPI_Pullups__SIO_HYST_EN)
    #define SPI_Pullups_SIO_REG_HIFREQ     (* (reg8 *) SPI_Pullups__SIO_REG_HIFREQ)
    #define SPI_Pullups_SIO_CFG            (* (reg8 *) SPI_Pullups__SIO_CFG)
    #define SPI_Pullups_SIO_DIFF           (* (reg8 *) SPI_Pullups__SIO_DIFF)
#endif /* (SPI_Pullups__SIO_CFG) */

/* Interrupt Registers */
#if defined(SPI_Pullups__INTSTAT)
    #define SPI_Pullups_INTSTAT            (* (reg8 *) SPI_Pullups__INTSTAT)
    #define SPI_Pullups_SNAP               (* (reg8 *) SPI_Pullups__SNAP)
    
	#define SPI_Pullups_0_INTTYPE_REG 		(* (reg8 *) SPI_Pullups__0__INTTYPE)
	#define SPI_Pullups_1_INTTYPE_REG 		(* (reg8 *) SPI_Pullups__1__INTTYPE)
	#define SPI_Pullups_2_INTTYPE_REG 		(* (reg8 *) SPI_Pullups__2__INTTYPE)
	#define SPI_Pullups_3_INTTYPE_REG 		(* (reg8 *) SPI_Pullups__3__INTTYPE)
#endif /* (SPI_Pullups__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_SPI_Pullups_H */


/* [] END OF FILE */
