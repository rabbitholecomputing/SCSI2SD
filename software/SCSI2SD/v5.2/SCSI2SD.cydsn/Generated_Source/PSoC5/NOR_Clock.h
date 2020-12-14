/*******************************************************************************
* File Name: NOR_Clock.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_NOR_Clock_H)
#define CY_CLOCK_NOR_Clock_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
* Conditional Compilation Parameters
***************************************/

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component cy_clock_v2_20 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*        Function Prototypes
***************************************/

void NOR_Clock_Start(void) ;
void NOR_Clock_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void NOR_Clock_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void NOR_Clock_StandbyPower(uint8 state) ;
void NOR_Clock_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 NOR_Clock_GetDividerRegister(void) ;
void NOR_Clock_SetModeRegister(uint8 modeBitMask) ;
void NOR_Clock_ClearModeRegister(uint8 modeBitMask) ;
uint8 NOR_Clock_GetModeRegister(void) ;
void NOR_Clock_SetSourceRegister(uint8 clkSource) ;
uint8 NOR_Clock_GetSourceRegister(void) ;
#if defined(NOR_Clock__CFG3)
void NOR_Clock_SetPhaseRegister(uint8 clkPhase) ;
uint8 NOR_Clock_GetPhaseRegister(void) ;
#endif /* defined(NOR_Clock__CFG3) */

#define NOR_Clock_Enable()                       NOR_Clock_Start()
#define NOR_Clock_Disable()                      NOR_Clock_Stop()
#define NOR_Clock_SetDivider(clkDivider)         NOR_Clock_SetDividerRegister(clkDivider, 1u)
#define NOR_Clock_SetDividerValue(clkDivider)    NOR_Clock_SetDividerRegister((clkDivider) - 1u, 1u)
#define NOR_Clock_SetMode(clkMode)               NOR_Clock_SetModeRegister(clkMode)
#define NOR_Clock_SetSource(clkSource)           NOR_Clock_SetSourceRegister(clkSource)
#if defined(NOR_Clock__CFG3)
#define NOR_Clock_SetPhase(clkPhase)             NOR_Clock_SetPhaseRegister(clkPhase)
#define NOR_Clock_SetPhaseValue(clkPhase)        NOR_Clock_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(NOR_Clock__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define NOR_Clock_CLKEN              (* (reg8 *) NOR_Clock__PM_ACT_CFG)
#define NOR_Clock_CLKEN_PTR          ((reg8 *) NOR_Clock__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define NOR_Clock_CLKSTBY            (* (reg8 *) NOR_Clock__PM_STBY_CFG)
#define NOR_Clock_CLKSTBY_PTR        ((reg8 *) NOR_Clock__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define NOR_Clock_DIV_LSB            (* (reg8 *) NOR_Clock__CFG0)
#define NOR_Clock_DIV_LSB_PTR        ((reg8 *) NOR_Clock__CFG0)
#define NOR_Clock_DIV_PTR            ((reg16 *) NOR_Clock__CFG0)

/* Clock MSB divider configuration register. */
#define NOR_Clock_DIV_MSB            (* (reg8 *) NOR_Clock__CFG1)
#define NOR_Clock_DIV_MSB_PTR        ((reg8 *) NOR_Clock__CFG1)

/* Mode and source configuration register */
#define NOR_Clock_MOD_SRC            (* (reg8 *) NOR_Clock__CFG2)
#define NOR_Clock_MOD_SRC_PTR        ((reg8 *) NOR_Clock__CFG2)

#if defined(NOR_Clock__CFG3)
/* Analog clock phase configuration register */
#define NOR_Clock_PHASE              (* (reg8 *) NOR_Clock__CFG3)
#define NOR_Clock_PHASE_PTR          ((reg8 *) NOR_Clock__CFG3)
#endif /* defined(NOR_Clock__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define NOR_Clock_CLKEN_MASK         NOR_Clock__PM_ACT_MSK
#define NOR_Clock_CLKSTBY_MASK       NOR_Clock__PM_STBY_MSK

/* CFG2 field masks */
#define NOR_Clock_SRC_SEL_MSK        NOR_Clock__CFG2_SRC_SEL_MASK
#define NOR_Clock_MODE_MASK          (~(NOR_Clock_SRC_SEL_MSK))

#if defined(NOR_Clock__CFG3)
/* CFG3 phase mask */
#define NOR_Clock_PHASE_MASK         NOR_Clock__CFG3_PHASE_DLY_MASK
#endif /* defined(NOR_Clock__CFG3) */

#endif /* CY_CLOCK_NOR_Clock_H */


/* [] END OF FILE */
