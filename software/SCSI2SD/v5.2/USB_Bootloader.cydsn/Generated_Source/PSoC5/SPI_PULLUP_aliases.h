/*******************************************************************************
* File Name: SPI_PULLUP.h  
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

#if !defined(CY_PINS_SPI_PULLUP_ALIASES_H) /* Pins SPI_PULLUP_ALIASES_H */
#define CY_PINS_SPI_PULLUP_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define SPI_PULLUP_0			(SPI_PULLUP__0__PC)
#define SPI_PULLUP_0_INTR	((uint16)((uint16)0x0001u << SPI_PULLUP__0__SHIFT))

#define SPI_PULLUP_1			(SPI_PULLUP__1__PC)
#define SPI_PULLUP_1_INTR	((uint16)((uint16)0x0001u << SPI_PULLUP__1__SHIFT))

#define SPI_PULLUP_2			(SPI_PULLUP__2__PC)
#define SPI_PULLUP_2_INTR	((uint16)((uint16)0x0001u << SPI_PULLUP__2__SHIFT))

#define SPI_PULLUP_3			(SPI_PULLUP__3__PC)
#define SPI_PULLUP_3_INTR	((uint16)((uint16)0x0001u << SPI_PULLUP__3__SHIFT))

#define SPI_PULLUP_INTR_ALL	 ((uint16)(SPI_PULLUP_0_INTR| SPI_PULLUP_1_INTR| SPI_PULLUP_2_INTR| SPI_PULLUP_3_INTR))

#endif /* End Pins SPI_PULLUP_ALIASES_H */


/* [] END OF FILE */
