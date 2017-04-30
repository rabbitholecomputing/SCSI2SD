/*******************************************************************************
* File Name: SCSI_In.h  
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

#if !defined(CY_PINS_SCSI_In_ALIASES_H) /* Pins SCSI_In_ALIASES_H */
#define CY_PINS_SCSI_In_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define SCSI_In_0			(SCSI_In__0__PC)
#define SCSI_In_0_INTR	((uint16)((uint16)0x0001u << SCSI_In__0__SHIFT))

#define SCSI_In_1			(SCSI_In__1__PC)
#define SCSI_In_1_INTR	((uint16)((uint16)0x0001u << SCSI_In__1__SHIFT))

#define SCSI_In_2			(SCSI_In__2__PC)
#define SCSI_In_2_INTR	((uint16)((uint16)0x0001u << SCSI_In__2__SHIFT))

#define SCSI_In_3			(SCSI_In__3__PC)
#define SCSI_In_3_INTR	((uint16)((uint16)0x0001u << SCSI_In__3__SHIFT))

#define SCSI_In_4			(SCSI_In__4__PC)
#define SCSI_In_4_INTR	((uint16)((uint16)0x0001u << SCSI_In__4__SHIFT))

#define SCSI_In_INTR_ALL	 ((uint16)(SCSI_In_0_INTR| SCSI_In_1_INTR| SCSI_In_2_INTR| SCSI_In_3_INTR| SCSI_In_4_INTR))
#define SCSI_In_DBP			(SCSI_In__DBP__PC)
#define SCSI_In_DBP_INTR	((uint16)((uint16)0x0001u << SCSI_In__0__SHIFT))

#define SCSI_In_MSG			(SCSI_In__MSG__PC)
#define SCSI_In_MSG_INTR	((uint16)((uint16)0x0001u << SCSI_In__1__SHIFT))

#define SCSI_In_CD			(SCSI_In__CD__PC)
#define SCSI_In_CD_INTR	((uint16)((uint16)0x0001u << SCSI_In__2__SHIFT))

#define SCSI_In_REQ			(SCSI_In__REQ__PC)
#define SCSI_In_REQ_INTR	((uint16)((uint16)0x0001u << SCSI_In__3__SHIFT))

#define SCSI_In_IO			(SCSI_In__IO__PC)
#define SCSI_In_IO_INTR	((uint16)((uint16)0x0001u << SCSI_In__4__SHIFT))

#endif /* End Pins SCSI_In_ALIASES_H */


/* [] END OF FILE */
