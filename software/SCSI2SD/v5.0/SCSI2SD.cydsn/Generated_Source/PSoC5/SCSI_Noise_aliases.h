/*******************************************************************************
* File Name: SCSI_Noise.h  
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

#if !defined(CY_PINS_SCSI_Noise_ALIASES_H) /* Pins SCSI_Noise_ALIASES_H */
#define CY_PINS_SCSI_Noise_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define SCSI_Noise_0			(SCSI_Noise__0__PC)
#define SCSI_Noise_0_INTR	((uint16)((uint16)0x0001u << SCSI_Noise__0__SHIFT))

#define SCSI_Noise_1			(SCSI_Noise__1__PC)
#define SCSI_Noise_1_INTR	((uint16)((uint16)0x0001u << SCSI_Noise__1__SHIFT))

#define SCSI_Noise_2			(SCSI_Noise__2__PC)
#define SCSI_Noise_2_INTR	((uint16)((uint16)0x0001u << SCSI_Noise__2__SHIFT))

#define SCSI_Noise_3			(SCSI_Noise__3__PC)
#define SCSI_Noise_3_INTR	((uint16)((uint16)0x0001u << SCSI_Noise__3__SHIFT))

#define SCSI_Noise_4			(SCSI_Noise__4__PC)
#define SCSI_Noise_4_INTR	((uint16)((uint16)0x0001u << SCSI_Noise__4__SHIFT))

#define SCSI_Noise_INTR_ALL	 ((uint16)(SCSI_Noise_0_INTR| SCSI_Noise_1_INTR| SCSI_Noise_2_INTR| SCSI_Noise_3_INTR| SCSI_Noise_4_INTR))
#define SCSI_Noise_ATN			(SCSI_Noise__ATN__PC)
#define SCSI_Noise_ATN_INTR	((uint16)((uint16)0x0001u << SCSI_Noise__0__SHIFT))

#define SCSI_Noise_BSY			(SCSI_Noise__BSY__PC)
#define SCSI_Noise_BSY_INTR	((uint16)((uint16)0x0001u << SCSI_Noise__1__SHIFT))

#define SCSI_Noise_SEL			(SCSI_Noise__SEL__PC)
#define SCSI_Noise_SEL_INTR	((uint16)((uint16)0x0001u << SCSI_Noise__2__SHIFT))

#define SCSI_Noise_RST			(SCSI_Noise__RST__PC)
#define SCSI_Noise_RST_INTR	((uint16)((uint16)0x0001u << SCSI_Noise__3__SHIFT))

#define SCSI_Noise_ACK			(SCSI_Noise__ACK__PC)
#define SCSI_Noise_ACK_INTR	((uint16)((uint16)0x0001u << SCSI_Noise__4__SHIFT))

#endif /* End Pins SCSI_Noise_ALIASES_H */


/* [] END OF FILE */
