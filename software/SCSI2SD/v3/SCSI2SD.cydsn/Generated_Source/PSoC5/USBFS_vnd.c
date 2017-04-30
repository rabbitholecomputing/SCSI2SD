/***************************************************************************//**
* \file USBFS_vnd.c
* \version 3.10
*
* \brief
*  This file contains the  USB vendor request handler.
*
********************************************************************************
* \copyright
* Copyright 2008-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "USBFS_pvt.h"


#if(USBFS_EXTERN_VND == USBFS_FALSE)

/***************************************
* Vendor Specific Declarations
***************************************/

/* `#START VENDOR_SPECIFIC_DECLARATIONS` Place your declaration here */

/* `#END` */


/*******************************************************************************
* Function Name: USBFS_HandleVendorRqst
****************************************************************************//**
*
*  This routine provide users with a method to implement vendor specific
*  requests.
*
*  To implement vendor specific requests, add your code in this function to
*  decode and disposition the request.  If the request is handled, your code
*  must set the variable "requestHandled" to TRUE, indicating that the
*  request has been handled.
*
* \return
*  requestHandled.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USBFS_HandleVendorRqst(void) 
{
    uint8 requestHandled = USBFS_FALSE;

    /* Check request direction: D2H or H2D. */
    if (0u != (USBFS_bmRequestTypeReg & USBFS_RQST_DIR_D2H))
    {
        /* Handle direction from device to host. */
        
        switch (USBFS_bRequestReg)
        {
            case USBFS_GET_EXTENDED_CONFIG_DESCRIPTOR:
            #if defined(USBFS_ENABLE_MSOS_STRING)
                USBFS_currentTD.pData = (volatile uint8 *) &USBFS_MSOS_CONFIGURATION_DESCR[0u];
                USBFS_currentTD.count = USBFS_MSOS_CONFIGURATION_DESCR[0u];
                requestHandled  = USBFS_InitControlRead();
            #endif /* (USBFS_ENABLE_MSOS_STRING) */
                break;
            
            default:
                break;
        }
    }

    /* `#START VENDOR_SPECIFIC_CODE` Place your vendor specific request here */

    /* `#END` */

#ifdef USBFS_HANDLE_VENDOR_RQST_CALLBACK
    if (USBFS_FALSE == requestHandled)
    {
        requestHandled = USBFS_HandleVendorRqst_Callback();
    }
#endif /* (USBFS_HANDLE_VENDOR_RQST_CALLBACK) */

    return (requestHandled);
}


/*******************************************************************************
* Additional user functions supporting Vendor Specific Requests
********************************************************************************/

/* `#START VENDOR_SPECIFIC_FUNCTIONS` Place any additional functions here */

/* `#END` */


#endif /* USBFS_EXTERN_VND */


/* [] END OF FILE */
