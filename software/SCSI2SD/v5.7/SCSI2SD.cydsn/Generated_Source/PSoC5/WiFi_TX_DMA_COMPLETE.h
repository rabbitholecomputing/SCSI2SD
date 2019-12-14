/*******************************************************************************
* File Name: WiFi_TX_DMA_COMPLETE.h
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
#if !defined(CY_ISR_WiFi_TX_DMA_COMPLETE_H)
#define CY_ISR_WiFi_TX_DMA_COMPLETE_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void WiFi_TX_DMA_COMPLETE_Start(void);
void WiFi_TX_DMA_COMPLETE_StartEx(cyisraddress address);
void WiFi_TX_DMA_COMPLETE_Stop(void);

CY_ISR_PROTO(WiFi_TX_DMA_COMPLETE_Interrupt);

void WiFi_TX_DMA_COMPLETE_SetVector(cyisraddress address);
cyisraddress WiFi_TX_DMA_COMPLETE_GetVector(void);

void WiFi_TX_DMA_COMPLETE_SetPriority(uint8 priority);
uint8 WiFi_TX_DMA_COMPLETE_GetPriority(void);

void WiFi_TX_DMA_COMPLETE_Enable(void);
uint8 WiFi_TX_DMA_COMPLETE_GetState(void);
void WiFi_TX_DMA_COMPLETE_Disable(void);

void WiFi_TX_DMA_COMPLETE_SetPending(void);
void WiFi_TX_DMA_COMPLETE_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the WiFi_TX_DMA_COMPLETE ISR. */
#define WiFi_TX_DMA_COMPLETE_INTC_VECTOR            ((reg32 *) WiFi_TX_DMA_COMPLETE__INTC_VECT)

/* Address of the WiFi_TX_DMA_COMPLETE ISR priority. */
#define WiFi_TX_DMA_COMPLETE_INTC_PRIOR             ((reg8 *) WiFi_TX_DMA_COMPLETE__INTC_PRIOR_REG)

/* Priority of the WiFi_TX_DMA_COMPLETE interrupt. */
#define WiFi_TX_DMA_COMPLETE_INTC_PRIOR_NUMBER      WiFi_TX_DMA_COMPLETE__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable WiFi_TX_DMA_COMPLETE interrupt. */
#define WiFi_TX_DMA_COMPLETE_INTC_SET_EN            ((reg32 *) WiFi_TX_DMA_COMPLETE__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the WiFi_TX_DMA_COMPLETE interrupt. */
#define WiFi_TX_DMA_COMPLETE_INTC_CLR_EN            ((reg32 *) WiFi_TX_DMA_COMPLETE__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the WiFi_TX_DMA_COMPLETE interrupt state to pending. */
#define WiFi_TX_DMA_COMPLETE_INTC_SET_PD            ((reg32 *) WiFi_TX_DMA_COMPLETE__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the WiFi_TX_DMA_COMPLETE interrupt. */
#define WiFi_TX_DMA_COMPLETE_INTC_CLR_PD            ((reg32 *) WiFi_TX_DMA_COMPLETE__INTC_CLR_PD_REG)


#endif /* CY_ISR_WiFi_TX_DMA_COMPLETE_H */


/* [] END OF FILE */
