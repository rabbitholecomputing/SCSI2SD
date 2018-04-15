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

#define SCSI_In_INTR_ALL	 ((uint16)(SCSI_In_0_INTR))
#define SCSI_In_DBP			(SCSI_In__DBP__PC)
#define SCSI_In_DBP_INTR	((uint16)((uint16)0x0001u << SCSI_In__0__SHIFT))

#endif /* End Pins SCSI_In_ALIASES_H */


/* [] END OF FILE */
