/*******************************************************************************
* File Name: ETH_TX_DMA_COMPLETE.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_ETH_TX_DMA_COMPLETE_H)
#define CY_ISR_ETH_TX_DMA_COMPLETE_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void ETH_TX_DMA_COMPLETE_Start(void);
void ETH_TX_DMA_COMPLETE_StartEx(cyisraddress address);
void ETH_TX_DMA_COMPLETE_Stop(void);

CY_ISR_PROTO(ETH_TX_DMA_COMPLETE_Interrupt);

void ETH_TX_DMA_COMPLETE_SetVector(cyisraddress address);
cyisraddress ETH_TX_DMA_COMPLETE_GetVector(void);

void ETH_TX_DMA_COMPLETE_SetPriority(uint8 priority);
uint8 ETH_TX_DMA_COMPLETE_GetPriority(void);

void ETH_TX_DMA_COMPLETE_Enable(void);
uint8 ETH_TX_DMA_COMPLETE_GetState(void);
void ETH_TX_DMA_COMPLETE_Disable(void);

void ETH_TX_DMA_COMPLETE_SetPending(void);
void ETH_TX_DMA_COMPLETE_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the ETH_TX_DMA_COMPLETE ISR. */
#define ETH_TX_DMA_COMPLETE_INTC_VECTOR            ((reg32 *) ETH_TX_DMA_COMPLETE__INTC_VECT)

/* Address of the ETH_TX_DMA_COMPLETE ISR priority. */
#define ETH_TX_DMA_COMPLETE_INTC_PRIOR             ((reg8 *) ETH_TX_DMA_COMPLETE__INTC_PRIOR_REG)

/* Priority of the ETH_TX_DMA_COMPLETE interrupt. */
#define ETH_TX_DMA_COMPLETE_INTC_PRIOR_NUMBER      ETH_TX_DMA_COMPLETE__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable ETH_TX_DMA_COMPLETE interrupt. */
#define ETH_TX_DMA_COMPLETE_INTC_SET_EN            ((reg32 *) ETH_TX_DMA_COMPLETE__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the ETH_TX_DMA_COMPLETE interrupt. */
#define ETH_TX_DMA_COMPLETE_INTC_CLR_EN            ((reg32 *) ETH_TX_DMA_COMPLETE__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the ETH_TX_DMA_COMPLETE interrupt state to pending. */
#define ETH_TX_DMA_COMPLETE_INTC_SET_PD            ((reg32 *) ETH_TX_DMA_COMPLETE__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the ETH_TX_DMA_COMPLETE interrupt. */
#define ETH_TX_DMA_COMPLETE_INTC_CLR_PD            ((reg32 *) ETH_TX_DMA_COMPLETE__INTC_CLR_PD_REG)


#endif /* CY_ISR_ETH_TX_DMA_COMPLETE_H */


/* [] END OF FILE */
