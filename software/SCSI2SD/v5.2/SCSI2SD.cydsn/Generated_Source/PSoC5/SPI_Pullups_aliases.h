/*******************************************************************************
* File Name: SPI_Pullups.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_SPI_Pullups_ALIASES_H) /* Pins SPI_Pullups_ALIASES_H */
#define CY_PINS_SPI_Pullups_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define SPI_Pullups_0			(SPI_Pullups__0__PC)
#define SPI_Pullups_0_INTR	((uint16)((uint16)0x0001u << SPI_Pullups__0__SHIFT))

#define SPI_Pullups_1			(SPI_Pullups__1__PC)
#define SPI_Pullups_1_INTR	((uint16)((uint16)0x0001u << SPI_Pullups__1__SHIFT))

#define SPI_Pullups_2			(SPI_Pullups__2__PC)
#define SPI_Pullups_2_INTR	((uint16)((uint16)0x0001u << SPI_Pullups__2__SHIFT))

#define SPI_Pullups_3			(SPI_Pullups__3__PC)
#define SPI_Pullups_3_INTR	((uint16)((uint16)0x0001u << SPI_Pullups__3__SHIFT))

#define SPI_Pullups_INTR_ALL	 ((uint16)(SPI_Pullups_0_INTR| SPI_Pullups_1_INTR| SPI_Pullups_2_INTR| SPI_Pullups_3_INTR))

#endif /* End Pins SPI_Pullups_ALIASES_H */


/* [] END OF FILE */
