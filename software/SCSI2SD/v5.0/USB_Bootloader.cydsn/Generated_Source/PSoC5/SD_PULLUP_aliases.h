/*******************************************************************************
* File Name: SD_PULLUP.h  
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

#if !defined(CY_PINS_SD_PULLUP_ALIASES_H) /* Pins SD_PULLUP_ALIASES_H */
#define CY_PINS_SD_PULLUP_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define SD_PULLUP_0			(SD_PULLUP__0__PC)
#define SD_PULLUP_0_INTR	((uint16)((uint16)0x0001u << SD_PULLUP__0__SHIFT))

#define SD_PULLUP_1			(SD_PULLUP__1__PC)
#define SD_PULLUP_1_INTR	((uint16)((uint16)0x0001u << SD_PULLUP__1__SHIFT))

#define SD_PULLUP_2			(SD_PULLUP__2__PC)
#define SD_PULLUP_2_INTR	((uint16)((uint16)0x0001u << SD_PULLUP__2__SHIFT))

#define SD_PULLUP_3			(SD_PULLUP__3__PC)
#define SD_PULLUP_3_INTR	((uint16)((uint16)0x0001u << SD_PULLUP__3__SHIFT))

#define SD_PULLUP_4			(SD_PULLUP__4__PC)
#define SD_PULLUP_4_INTR	((uint16)((uint16)0x0001u << SD_PULLUP__4__SHIFT))

#define SD_PULLUP_INTR_ALL	 ((uint16)(SD_PULLUP_0_INTR| SD_PULLUP_1_INTR| SD_PULLUP_2_INTR| SD_PULLUP_3_INTR| SD_PULLUP_4_INTR))

#endif /* End Pins SD_PULLUP_ALIASES_H */


/* [] END OF FILE */
