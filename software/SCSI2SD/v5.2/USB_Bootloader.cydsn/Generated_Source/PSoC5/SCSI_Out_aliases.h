/*******************************************************************************
* File Name: SCSI_Out.h  
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

#if !defined(CY_PINS_SCSI_Out_ALIASES_H) /* Pins SCSI_Out_ALIASES_H */
#define CY_PINS_SCSI_Out_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define SCSI_Out_0			(SCSI_Out__0__PC)
#define SCSI_Out_0_INTR	((uint16)((uint16)0x0001u << SCSI_Out__0__SHIFT))

#define SCSI_Out_1			(SCSI_Out__1__PC)
#define SCSI_Out_1_INTR	((uint16)((uint16)0x0001u << SCSI_Out__1__SHIFT))

#define SCSI_Out_2			(SCSI_Out__2__PC)
#define SCSI_Out_2_INTR	((uint16)((uint16)0x0001u << SCSI_Out__2__SHIFT))

#define SCSI_Out_3			(SCSI_Out__3__PC)
#define SCSI_Out_3_INTR	((uint16)((uint16)0x0001u << SCSI_Out__3__SHIFT))

#define SCSI_Out_4			(SCSI_Out__4__PC)
#define SCSI_Out_4_INTR	((uint16)((uint16)0x0001u << SCSI_Out__4__SHIFT))

#define SCSI_Out_5			(SCSI_Out__5__PC)
#define SCSI_Out_5_INTR	((uint16)((uint16)0x0001u << SCSI_Out__5__SHIFT))

#define SCSI_Out_6			(SCSI_Out__6__PC)
#define SCSI_Out_6_INTR	((uint16)((uint16)0x0001u << SCSI_Out__6__SHIFT))

#define SCSI_Out_7			(SCSI_Out__7__PC)
#define SCSI_Out_7_INTR	((uint16)((uint16)0x0001u << SCSI_Out__7__SHIFT))

#define SCSI_Out_INTR_ALL	 ((uint16)(SCSI_Out_0_INTR| SCSI_Out_1_INTR| SCSI_Out_2_INTR| SCSI_Out_3_INTR| SCSI_Out_4_INTR| SCSI_Out_5_INTR| SCSI_Out_6_INTR| SCSI_Out_7_INTR))
#define SCSI_Out_DBP_raw			(SCSI_Out__DBP_raw__PC)
#define SCSI_Out_DBP_raw_INTR	((uint16)((uint16)0x0001u << SCSI_Out__0__SHIFT))

#define SCSI_Out_BSY			(SCSI_Out__BSY__PC)
#define SCSI_Out_BSY_INTR	((uint16)((uint16)0x0001u << SCSI_Out__1__SHIFT))

#define SCSI_Out_RST			(SCSI_Out__RST__PC)
#define SCSI_Out_RST_INTR	((uint16)((uint16)0x0001u << SCSI_Out__2__SHIFT))

#define SCSI_Out_MSG			(SCSI_Out__MSG__PC)
#define SCSI_Out_MSG_INTR	((uint16)((uint16)0x0001u << SCSI_Out__3__SHIFT))

#define SCSI_Out_SEL			(SCSI_Out__SEL__PC)
#define SCSI_Out_SEL_INTR	((uint16)((uint16)0x0001u << SCSI_Out__4__SHIFT))

#define SCSI_Out_CD			(SCSI_Out__CD__PC)
#define SCSI_Out_CD_INTR	((uint16)((uint16)0x0001u << SCSI_Out__5__SHIFT))

#define SCSI_Out_REQ			(SCSI_Out__REQ__PC)
#define SCSI_Out_REQ_INTR	((uint16)((uint16)0x0001u << SCSI_Out__6__SHIFT))

#define SCSI_Out_IO_raw			(SCSI_Out__IO_raw__PC)
#define SCSI_Out_IO_raw_INTR	((uint16)((uint16)0x0001u << SCSI_Out__7__SHIFT))

#endif /* End Pins SCSI_Out_ALIASES_H */


/* [] END OF FILE */
