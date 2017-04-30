/*******************************************************************************
* File Name: SCSI_In_DBx.h  
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

#if !defined(CY_PINS_SCSI_In_DBx_ALIASES_H) /* Pins SCSI_In_DBx_ALIASES_H */
#define CY_PINS_SCSI_In_DBx_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define SCSI_In_DBx_0			(SCSI_In_DBx__0__PC)
#define SCSI_In_DBx_0_INTR	((uint16)((uint16)0x0001u << SCSI_In_DBx__0__SHIFT))

#define SCSI_In_DBx_1			(SCSI_In_DBx__1__PC)
#define SCSI_In_DBx_1_INTR	((uint16)((uint16)0x0001u << SCSI_In_DBx__1__SHIFT))

#define SCSI_In_DBx_2			(SCSI_In_DBx__2__PC)
#define SCSI_In_DBx_2_INTR	((uint16)((uint16)0x0001u << SCSI_In_DBx__2__SHIFT))

#define SCSI_In_DBx_3			(SCSI_In_DBx__3__PC)
#define SCSI_In_DBx_3_INTR	((uint16)((uint16)0x0001u << SCSI_In_DBx__3__SHIFT))

#define SCSI_In_DBx_4			(SCSI_In_DBx__4__PC)
#define SCSI_In_DBx_4_INTR	((uint16)((uint16)0x0001u << SCSI_In_DBx__4__SHIFT))

#define SCSI_In_DBx_5			(SCSI_In_DBx__5__PC)
#define SCSI_In_DBx_5_INTR	((uint16)((uint16)0x0001u << SCSI_In_DBx__5__SHIFT))

#define SCSI_In_DBx_6			(SCSI_In_DBx__6__PC)
#define SCSI_In_DBx_6_INTR	((uint16)((uint16)0x0001u << SCSI_In_DBx__6__SHIFT))

#define SCSI_In_DBx_7			(SCSI_In_DBx__7__PC)
#define SCSI_In_DBx_7_INTR	((uint16)((uint16)0x0001u << SCSI_In_DBx__7__SHIFT))

#define SCSI_In_DBx_INTR_ALL	 ((uint16)(SCSI_In_DBx_0_INTR| SCSI_In_DBx_1_INTR| SCSI_In_DBx_2_INTR| SCSI_In_DBx_3_INTR| SCSI_In_DBx_4_INTR| SCSI_In_DBx_5_INTR| SCSI_In_DBx_6_INTR| SCSI_In_DBx_7_INTR))
#define SCSI_In_DBx_DB0			(SCSI_In_DBx__DB0__PC)
#define SCSI_In_DBx_DB0_INTR	((uint16)((uint16)0x0001u << SCSI_In_DBx__0__SHIFT))

#define SCSI_In_DBx_DB1			(SCSI_In_DBx__DB1__PC)
#define SCSI_In_DBx_DB1_INTR	((uint16)((uint16)0x0001u << SCSI_In_DBx__1__SHIFT))

#define SCSI_In_DBx_DB2			(SCSI_In_DBx__DB2__PC)
#define SCSI_In_DBx_DB2_INTR	((uint16)((uint16)0x0001u << SCSI_In_DBx__2__SHIFT))

#define SCSI_In_DBx_DB3			(SCSI_In_DBx__DB3__PC)
#define SCSI_In_DBx_DB3_INTR	((uint16)((uint16)0x0001u << SCSI_In_DBx__3__SHIFT))

#define SCSI_In_DBx_DB4			(SCSI_In_DBx__DB4__PC)
#define SCSI_In_DBx_DB4_INTR	((uint16)((uint16)0x0001u << SCSI_In_DBx__4__SHIFT))

#define SCSI_In_DBx_DB5			(SCSI_In_DBx__DB5__PC)
#define SCSI_In_DBx_DB5_INTR	((uint16)((uint16)0x0001u << SCSI_In_DBx__5__SHIFT))

#define SCSI_In_DBx_DB6			(SCSI_In_DBx__DB6__PC)
#define SCSI_In_DBx_DB6_INTR	((uint16)((uint16)0x0001u << SCSI_In_DBx__6__SHIFT))

#define SCSI_In_DBx_DB7			(SCSI_In_DBx__DB7__PC)
#define SCSI_In_DBx_DB7_INTR	((uint16)((uint16)0x0001u << SCSI_In_DBx__7__SHIFT))

#endif /* End Pins SCSI_In_DBx_ALIASES_H */


/* [] END OF FILE */
