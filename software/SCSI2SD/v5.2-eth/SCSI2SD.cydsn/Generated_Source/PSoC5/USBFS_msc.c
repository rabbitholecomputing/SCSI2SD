/***************************************************************************//**
* \file USBFS_cdc.c
* \version 3.10
*
* \brief
*  This file contains the USB MSC Class request handler and global API for MSC 
*  class.
*
* Related Document:
*  Universal Serial Bus Class Definitions for Communication Devices Version 1.1
*
********************************************************************************
* \copyright
* Copyright 2012-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "USBFS_msc.h"
#include "USBFS_pvt.h"


#if (USBFS_HANDLE_MSC_REQUESTS)

/***************************************
*          Internal variables
***************************************/

static uint8 USBFS_lunCount = USBFS_MSC_LUN_NUMBER;


/*******************************************************************************
* Function Name: USBFS_DispatchMSCClassRqst
****************************************************************************//**
*   
*  \internal 
*  This routine dispatches MSC class requests.
*
* \return
*  Status of request processing: handled or not handled.
*
* \globalvars
*  USBFS_lunCount - stores number of LUN (logical units).
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USBFS_DispatchMSCClassRqst(void) 
{
    uint8 requestHandled = USBFS_FALSE;
    
    /* Get request data. */
    uint16 value  = USBFS_GET_UINT16(USBFS_wValueHiReg,  USBFS_wValueLoReg);
    uint16 dataLength = USBFS_GET_UINT16(USBFS_wLengthHiReg, USBFS_wLengthLoReg);
       
    /* Check request direction: D2H or H2D. */
    if (0u != (USBFS_bmRequestTypeReg & USBFS_RQST_DIR_D2H))
    {
        /* Handle direction from device to host. */
        
        if (USBFS_MSC_GET_MAX_LUN == USBFS_bRequestReg)
        {
            /* Check request fields. */
            if ((value  == USBFS_MSC_GET_MAX_LUN_WVALUE) &&
                (dataLength == USBFS_MSC_GET_MAX_LUN_WLENGTH))
            {
                /* Reply to Get Max LUN request: setup control read. */
                USBFS_currentTD.pData = &USBFS_lunCount;
                USBFS_currentTD.count =  USBFS_MSC_GET_MAX_LUN_WLENGTH;
                
                requestHandled  = USBFS_InitControlRead();
            }
        }
    }
    else
    {
        /* Handle direction from host to device. */
        
        if (USBFS_MSC_RESET == USBFS_bRequestReg)
        {
            /* Check request fields. */
            if ((value  == USBFS_MSC_RESET_WVALUE) &&
                (dataLength == USBFS_MSC_RESET_WLENGTH))
            {
                /* Handle to Bulk-Only Reset request: no data control transfer. */
                USBFS_currentTD.count = USBFS_MSC_RESET_WLENGTH;
                
            #ifdef USBFS_DISPATCH_MSC_CLASS_MSC_RESET_RQST_CALLBACK
                USBFS_DispatchMSCClass_MSC_RESET_RQST_Callback();
            #endif /* (USBFS_DISPATCH_MSC_CLASS_MSC_RESET_RQST_CALLBACK) */
                
                requestHandled = USBFS_InitNoDataControlTransfer();
            }
        }
    }
    
    return (requestHandled);
}


/*******************************************************************************
* Function Name: USBFS_MSC_SetLunCount
****************************************************************************//**
*
*  This function sets the number of logical units supported in the application. 
*  The default number of logical units is set in the component customizer.
*
*  \param lunCount: Count of the logical units. Valid range is between 1 and 16.
*
*
* \globalvars
*  USBFS_lunCount - stores number of LUN (logical units).
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBFS_MSC_SetLunCount(uint8 lunCount) 
{
    USBFS_lunCount = (lunCount - 1u);
}


/*******************************************************************************
* Function Name: USBFS_MSC_GetLunCount
****************************************************************************//**
*
*  This function returns the number of logical units.
*
* \return
*   Number of the logical units.
*
* \globalvars
*  USBFS_lunCount - stores number of LUN (logical units).
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USBFS_MSC_GetLunCount(void) 
{
    return (USBFS_lunCount + 1u);
}   

#endif /* (USBFS_HANDLE_MSC_REQUESTS) */


/* [] END OF FILE */
