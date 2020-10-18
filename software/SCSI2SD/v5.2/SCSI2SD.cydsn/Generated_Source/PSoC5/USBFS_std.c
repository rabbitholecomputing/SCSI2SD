/***************************************************************************//**
* \file USBFS_std.c
* \version 3.10
*
* \brief
*  This file contains the USB Standard request handler.
*
********************************************************************************
* \copyright
* Copyright 2008-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "USBFS_pvt.h"

/***************************************
*   Static data allocation
***************************************/

#if defined(USBFS_ENABLE_FWSN_STRING)
    static volatile uint8* USBFS_fwSerialNumberStringDescriptor;
    static volatile uint8  USBFS_snStringConfirm = USBFS_FALSE;
#endif  /* (USBFS_ENABLE_FWSN_STRING) */

#if defined(USBFS_ENABLE_FWSN_STRING)
    /***************************************************************************
    * Function Name: USBFS_SerialNumString
    ************************************************************************//**
    *
    *  This function is available only when the User Call Back option in the 
    *  Serial Number String descriptor properties is selected. Application 
    *  firmware can provide the source of the USB device serial number string 
    *  descriptor during run time. The default string is used if the application 
    *  firmware does not use this function or sets the wrong string descriptor.
    *
    *  \param snString:  Pointer to the user-defined string descriptor. The 
    *  string descriptor should meet the Universal Serial Bus Specification 
    *  revision 2.0 chapter 9.6.7
    *  Offset|Size|Value|Description
    *  ------|----|------|---------------------------------
    *  0     |1   |N     |Size of this descriptor in bytes
    *  1     |1   |0x03  |STRING Descriptor Type
    *  2     |N-2 |Number|UNICODE encoded string
    *  
    * *For example:* uint8 snString[16]={0x0E,0x03,'F',0,'W',0,'S',0,'N',0,'0',0,'1',0};
    *
    * \reentrant
    *  No.
    *
    ***************************************************************************/
    void  USBFS_SerialNumString(uint8 snString[]) 
    {
        USBFS_snStringConfirm = USBFS_FALSE;
        
        if (snString != NULL)
        {
            /* Check descriptor validation */
            if ((snString[0u] > 1u) && (snString[1u] == USBFS_DESCR_STRING))
            {
                USBFS_fwSerialNumberStringDescriptor = snString;
                USBFS_snStringConfirm = USBFS_TRUE;
            }
        }
    }
#endif  /* USBFS_ENABLE_FWSN_STRING */


/*******************************************************************************
* Function Name: USBFS_HandleStandardRqst
****************************************************************************//**
*
*  This Routine dispatches standard requests
*
*
* \return
*  TRUE if request handled.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USBFS_HandleStandardRqst(void) 
{
    uint8 requestHandled = USBFS_FALSE;
    uint8 interfaceNumber;
    uint8 configurationN;
    uint8 bmRequestType = USBFS_bmRequestTypeReg;

#if defined(USBFS_ENABLE_STRINGS)
    volatile uint8 *pStr = 0u;
    #if defined(USBFS_ENABLE_DESCRIPTOR_STRINGS)
        uint8 nStr;
        uint8 descrLength;
    #endif /* (USBFS_ENABLE_DESCRIPTOR_STRINGS) */
#endif /* (USBFS_ENABLE_STRINGS) */
    
    static volatile uint8 USBFS_tBuffer[USBFS_STATUS_LENGTH_MAX];
    const T_USBFS_LUT CYCODE *pTmp;

    USBFS_currentTD.count = 0u;

    if (USBFS_RQST_DIR_D2H == (bmRequestType & USBFS_RQST_DIR_MASK))
    {
        /* Control Read */
        switch (USBFS_bRequestReg)
        {
            case USBFS_GET_DESCRIPTOR:
                if (USBFS_DESCR_DEVICE ==USBFS_wValueHiReg)
                {
                    pTmp = USBFS_GetDeviceTablePtr();
                    USBFS_currentTD.pData = (volatile uint8 *)pTmp->p_list;
                    USBFS_currentTD.count = USBFS_DEVICE_DESCR_LENGTH;
                    
                    requestHandled  = USBFS_InitControlRead();
                }
                else if (USBFS_DESCR_CONFIG == USBFS_wValueHiReg)
                {
                    pTmp = USBFS_GetConfigTablePtr((uint8) USBFS_wValueLoReg);
                    
                    /* Verify that requested descriptor exists */
                    if (pTmp != NULL)
                    {
                        USBFS_currentTD.pData = (volatile uint8 *)pTmp->p_list;
                        USBFS_currentTD.count = (uint16)((uint16)(USBFS_currentTD.pData)[USBFS_CONFIG_DESCR_TOTAL_LENGTH_HI] << 8u) | \
                                                                            (USBFS_currentTD.pData)[USBFS_CONFIG_DESCR_TOTAL_LENGTH_LOW];
                        requestHandled  = USBFS_InitControlRead();
                    }
                }
                
            #if(USBFS_BOS_ENABLE)
                else if (USBFS_DESCR_BOS == USBFS_wValueHiReg)
                {
                    pTmp = USBFS_GetBOSPtr();
                    
                    /* Verify that requested descriptor exists */
                    if (pTmp != NULL)
                    {
                        USBFS_currentTD.pData = (volatile uint8 *)pTmp;
                        USBFS_currentTD.count = ((uint16)((uint16)(USBFS_currentTD.pData)[USBFS_BOS_DESCR_TOTAL_LENGTH_HI] << 8u)) | \
                                                                             (USBFS_currentTD.pData)[USBFS_BOS_DESCR_TOTAL_LENGTH_LOW];
                        requestHandled  = USBFS_InitControlRead();
                    }
                }
            #endif /*(USBFS_BOS_ENABLE)*/
            
            #if defined(USBFS_ENABLE_STRINGS)
                else if (USBFS_DESCR_STRING == USBFS_wValueHiReg)
                {
                /* Descriptor Strings */
                #if defined(USBFS_ENABLE_DESCRIPTOR_STRINGS)
                    nStr = 0u;
                    pStr = (volatile uint8 *) &USBFS_STRING_DESCRIPTORS[0u];
                    
                    while ((USBFS_wValueLoReg > nStr) && (*pStr != 0u))
                    {
                        /* Read descriptor length from 1st byte */
                        descrLength = *pStr;
                        /* Move to next string descriptor */
                        pStr = &pStr[descrLength];
                        nStr++;
                    }
                #endif /* (USBFS_ENABLE_DESCRIPTOR_STRINGS) */
                
                /* Microsoft OS String */
                #if defined(USBFS_ENABLE_MSOS_STRING)
                    if (USBFS_STRING_MSOS == USBFS_wValueLoReg)
                    {
                        pStr = (volatile uint8 *)& USBFS_MSOS_DESCRIPTOR[0u];
                    }
                #endif /* (USBFS_ENABLE_MSOS_STRING) */
                
                /* SN string */
                #if defined(USBFS_ENABLE_SN_STRING)
                    if ((USBFS_wValueLoReg != 0u) && 
                        (USBFS_wValueLoReg == USBFS_DEVICE0_DESCR[USBFS_DEVICE_DESCR_SN_SHIFT]))
                    {
                    #if defined(USBFS_ENABLE_IDSN_STRING)
                        /* Read DIE ID and generate string descriptor in RAM */
                        USBFS_ReadDieID(USBFS_idSerialNumberStringDescriptor);
                        pStr = USBFS_idSerialNumberStringDescriptor;
                    #elif defined(USBFS_ENABLE_FWSN_STRING)
                        
                        if(USBFS_snStringConfirm != USBFS_FALSE)
                        {
                            pStr = USBFS_fwSerialNumberStringDescriptor;
                        }
                        else
                        {
                            pStr = (volatile uint8 *)&USBFS_SN_STRING_DESCRIPTOR[0u];
                        }
                    #else
                        pStr = (volatile uint8 *)&USBFS_SN_STRING_DESCRIPTOR[0u];
                    #endif  /* (USBFS_ENABLE_IDSN_STRING) */
                    }
                #endif /* (USBFS_ENABLE_SN_STRING) */
                
                    if (*pStr != 0u)
                    {
                        USBFS_currentTD.count = *pStr;
                        USBFS_currentTD.pData = pStr;
                        requestHandled  = USBFS_InitControlRead();
                    }
                }
            #endif /*  USBFS_ENABLE_STRINGS */
                else
                {
                    requestHandled = USBFS_DispatchClassRqst();
                }
                break;
                
            case USBFS_GET_STATUS:
                switch (bmRequestType & USBFS_RQST_RCPT_MASK)
                {
                    case USBFS_RQST_RCPT_EP:
                        USBFS_currentTD.count = USBFS_EP_STATUS_LENGTH;
                        USBFS_tBuffer[0u]     = USBFS_EP[USBFS_wIndexLoReg & USBFS_DIR_UNUSED].hwEpState;
                        USBFS_tBuffer[1u]     = 0u;
                        USBFS_currentTD.pData = &USBFS_tBuffer[0u];
                        
                        requestHandled  = USBFS_InitControlRead();
                        break;
                    case USBFS_RQST_RCPT_DEV:
                        USBFS_currentTD.count = USBFS_DEVICE_STATUS_LENGTH;
                        USBFS_tBuffer[0u]     = USBFS_deviceStatus;
                        USBFS_tBuffer[1u]     = 0u;
                        USBFS_currentTD.pData = &USBFS_tBuffer[0u];
                        
                        requestHandled  = USBFS_InitControlRead();
                        break;
                    default:    /* requestHandled is initialized as FALSE by default */
                        break;
                }
                break;
                
            case USBFS_GET_CONFIGURATION:
                USBFS_currentTD.count = 1u;
                USBFS_currentTD.pData = (volatile uint8 *) &USBFS_configuration;
                requestHandled  = USBFS_InitControlRead();
                break;
                
            case USBFS_GET_INTERFACE:
                USBFS_currentTD.count = 1u;
                USBFS_currentTD.pData = (volatile uint8 *) &USBFS_interfaceSetting[USBFS_wIndexLoReg];
                requestHandled  = USBFS_InitControlRead();
                break;
                
            default: /* requestHandled is initialized as FALSE by default */
                break;
        }
    }
    else
    {
        /* Control Write */
        switch (USBFS_bRequestReg)
        {
            case USBFS_SET_ADDRESS:
                /* Store address to be set in USBFS_NoDataControlStatusStage(). */
                USBFS_deviceAddress = (uint8) USBFS_wValueLoReg;
                requestHandled = USBFS_InitNoDataControlTransfer();
                break;
                
            case USBFS_SET_CONFIGURATION:
                configurationN = USBFS_wValueLoReg;
                
                /* Verify that configuration descriptor exists */
                if(configurationN > 0u)
                {
                    pTmp = USBFS_GetConfigTablePtr((uint8) configurationN - 1u);
                }
                
                /* Responds with a Request Error when configuration number is invalid */
                if (((configurationN > 0u) && (pTmp != NULL)) || (configurationN == 0u))
                {
                    /* Set new configuration if it has been changed */
                    if(configurationN != USBFS_configuration)
                    {
                        USBFS_configuration = (uint8) configurationN;
                        USBFS_configurationChanged = USBFS_TRUE;
                        USBFS_Config(USBFS_TRUE);
                    }
                    requestHandled = USBFS_InitNoDataControlTransfer();
                }
                break;
                
            case USBFS_SET_INTERFACE:
                if (0u != USBFS_ValidateAlternateSetting())
                {
                    /* Get interface number from the request. */
                    interfaceNumber = USBFS_wIndexLoReg;
                    USBFS_interfaceNumber = (uint8) USBFS_wIndexLoReg;
                     
                    /* Check if alternate settings is changed for interface. */
                    if (USBFS_interfaceSettingLast[interfaceNumber] != USBFS_interfaceSetting[interfaceNumber])
                    {
                        USBFS_configurationChanged = USBFS_TRUE;
                    
                        /* Change alternate setting for the endpoints. */
                    #if (USBFS_EP_MANAGEMENT_MANUAL && USBFS_EP_ALLOC_DYNAMIC)
                        USBFS_Config(USBFS_FALSE);
                    #else
                        USBFS_ConfigAltChanged();
                    #endif /* (USBFS_EP_MANAGEMENT_MANUAL && USBFS_EP_ALLOC_DYNAMIC) */
                    }
                    
                    requestHandled = USBFS_InitNoDataControlTransfer();
                }
                break;
                
            case USBFS_CLEAR_FEATURE:
                switch (bmRequestType & USBFS_RQST_RCPT_MASK)
                {
                    case USBFS_RQST_RCPT_EP:
                        if (USBFS_wValueLoReg == USBFS_ENDPOINT_HALT)
                        {
                            requestHandled = USBFS_ClearEndpointHalt();
                        }
                        break;
                    case USBFS_RQST_RCPT_DEV:
                        /* Clear device REMOTE_WAKEUP */
                        if (USBFS_wValueLoReg == USBFS_DEVICE_REMOTE_WAKEUP)
                        {
                            USBFS_deviceStatus &= (uint8)~USBFS_DEVICE_STATUS_REMOTE_WAKEUP;
                            requestHandled = USBFS_InitNoDataControlTransfer();
                        }
                        break;
                    case USBFS_RQST_RCPT_IFC:
                        /* Validate interfaceNumber */
                        if (USBFS_wIndexLoReg < USBFS_MAX_INTERFACES_NUMBER)
                        {
                            USBFS_interfaceStatus[USBFS_wIndexLoReg] &= (uint8) ~USBFS_wValueLoReg;
                            requestHandled = USBFS_InitNoDataControlTransfer();
                        }
                        break;
                    default:    /* requestHandled is initialized as FALSE by default */
                        break;
                }
                break;
                
            case USBFS_SET_FEATURE:
                switch (bmRequestType & USBFS_RQST_RCPT_MASK)
                {
                    case USBFS_RQST_RCPT_EP:
                        if (USBFS_wValueLoReg == USBFS_ENDPOINT_HALT)
                        {
                            requestHandled = USBFS_SetEndpointHalt();
                        }
                        break;
                        
                    case USBFS_RQST_RCPT_DEV:
                        /* Set device REMOTE_WAKEUP */
                        if (USBFS_wValueLoReg == USBFS_DEVICE_REMOTE_WAKEUP)
                        {
                            USBFS_deviceStatus |= USBFS_DEVICE_STATUS_REMOTE_WAKEUP;
                            requestHandled = USBFS_InitNoDataControlTransfer();
                        }
                        break;
                        
                    case USBFS_RQST_RCPT_IFC:
                        /* Validate interfaceNumber */
                        if (USBFS_wIndexLoReg < USBFS_MAX_INTERFACES_NUMBER)
                        {
                            USBFS_interfaceStatus[USBFS_wIndexLoReg] &= (uint8) ~USBFS_wValueLoReg;
                            requestHandled = USBFS_InitNoDataControlTransfer();
                        }
                        break;
                    
                    default:    /* requestHandled is initialized as FALSE by default */
                        break;
                }
                break;
                
            default:    /* requestHandled is initialized as FALSE by default */
                break;
        }
    }
    
    return (requestHandled);
}


#if defined(USBFS_ENABLE_IDSN_STRING)
    /***************************************************************************
    * Function Name: USBFS_ReadDieID
    ************************************************************************//**
    *
    *  This routine read Die ID and generate Serial Number string descriptor.
    *
    *  \param descr:  pointer on string descriptor. This string size has to be equal or
    *          greater than USBFS_IDSN_DESCR_LENGTH.
    *
    *
    * \reentrant
    *  No.
    *
    ***************************************************************************/
    void USBFS_ReadDieID(uint8 descr[]) 
    {
        const char8 CYCODE hex[] = "0123456789ABCDEF";
        uint8 i;
        uint8 j = 0u;
        uint8 uniqueId[8u];

        if (NULL != descr)
        {
            /* Initialize descriptor header. */
            descr[0u] = USBFS_IDSN_DESCR_LENGTH;
            descr[1u] = USBFS_DESCR_STRING;
            
            /* Unique ID size is 8 bytes. */
            CyGetUniqueId((uint32 *) uniqueId);

            /* Fill descriptor with unique device ID. */
            for (i = 2u; i < USBFS_IDSN_DESCR_LENGTH; i += 4u)
            {
                descr[i]      = (uint8) hex[(uniqueId[j] >> 4u)];
                descr[i + 1u] = 0u;
                descr[i + 2u] = (uint8) hex[(uniqueId[j] & 0x0Fu)];
                descr[i + 3u] = 0u;
                ++j;
            }
        }
    }
#endif /* (USBFS_ENABLE_IDSN_STRING) */


/*******************************************************************************
* Function Name: USBFS_ConfigReg
****************************************************************************//**
*
*  This routine configures hardware registers from the variables.
*  It is called from USBFS_Config() function and from RestoreConfig
*  after Wakeup.
*
*******************************************************************************/
void USBFS_ConfigReg(void) 
{
    uint8 ep;

#if (USBFS_EP_MANAGEMENT_DMA_AUTO)
    uint8 epType = 0u;
#endif /* (USBFS_EP_MANAGEMENT_DMA_AUTO) */

    /* Go thought all endpoints and set hardware configuration */
    for (ep = USBFS_EP1; ep < USBFS_MAX_EP; ++ep)
    {
        USBFS_ARB_EP_BASE.arbEp[ep].epCfg = USBFS_ARB_EPX_CFG_DEFAULT;
        
    #if (USBFS_EP_MANAGEMENT_DMA)
        /* Enable arbiter endpoint interrupt sources */
        USBFS_ARB_EP_BASE.arbEp[ep].epIntEn = USBFS_ARB_EPX_INT_MASK;
    #endif /* (USBFS_EP_MANAGEMENT_DMA) */
    
        if (USBFS_EP[ep].epMode != USBFS_MODE_DISABLE)
        {
            if (0u != (USBFS_EP[ep].addr & USBFS_DIR_IN))
            {
                USBFS_SIE_EP_BASE.sieEp[ep].epCr0 = USBFS_MODE_NAK_IN;
                
            #if (USBFS_EP_MANAGEMENT_DMA_AUTO && CY_PSOC4)
                /* Clear DMA_TERMIN for IN endpoint. */
                USBFS_ARB_EP_BASE.arbEp[ep].epIntEn &= (uint32) ~USBFS_ARB_EPX_INT_DMA_TERMIN;
            #endif /* (USBFS_EP_MANAGEMENT_DMA_AUTO && CY_PSOC4) */
            }
            else
            {
                USBFS_SIE_EP_BASE.sieEp[ep].epCr0 = USBFS_MODE_NAK_OUT;

            #if (USBFS_EP_MANAGEMENT_DMA_AUTO)
                /* (CY_PSOC4): DMA_TERMIN for OUT endpoint is set above. */
                
                /* Prepare endpoint type mask. */
                epType |= (uint8) (0x01u << (ep - USBFS_EP1));
            #endif /* (USBFS_EP_MANAGEMENT_DMA_AUTO) */
            }
        }
        else
        {
            USBFS_SIE_EP_BASE.sieEp[ep].epCr0 = USBFS_MODE_STALL_DATA_EP;
        }
        
    #if (!USBFS_EP_MANAGEMENT_DMA_AUTO)
        #if (CY_PSOC4)
            USBFS_ARB_EP16_BASE.arbEp[ep].rwRa16  = (uint32) USBFS_EP[ep].buffOffset;
            USBFS_ARB_EP16_BASE.arbEp[ep].rwWa16  = (uint32) USBFS_EP[ep].buffOffset;
        #else
            USBFS_ARB_EP_BASE.arbEp[ep].rwRa    = LO8(USBFS_EP[ep].buffOffset);
            USBFS_ARB_EP_BASE.arbEp[ep].rwRaMsb = HI8(USBFS_EP[ep].buffOffset);
            USBFS_ARB_EP_BASE.arbEp[ep].rwWa    = LO8(USBFS_EP[ep].buffOffset);
            USBFS_ARB_EP_BASE.arbEp[ep].rwWaMsb = HI8(USBFS_EP[ep].buffOffset);
        #endif /* (CY_PSOC4) */
    #endif /* (!USBFS_EP_MANAGEMENT_DMA_AUTO) */
    }

#if (USBFS_EP_MANAGEMENT_DMA_AUTO)
     /* BUF_SIZE depend on DMA_THRESS value:0x55-32 bytes  0x44-16 bytes 0x33-8 bytes 0x22-4 bytes 0x11-2 bytes */
    USBFS_BUF_SIZE_REG = USBFS_DMA_BUF_SIZE;

    /* Configure DMA burst threshold */
#if (CY_PSOC4)
    USBFS_DMA_THRES16_REG   = USBFS_DMA_BYTES_PER_BURST;
#else
    USBFS_DMA_THRES_REG     = USBFS_DMA_BYTES_PER_BURST;
    USBFS_DMA_THRES_MSB_REG = 0u;
#endif /* (CY_PSOC4) */
    USBFS_EP_ACTIVE_REG = USBFS_DEFAULT_ARB_INT_EN;
    USBFS_EP_TYPE_REG   = epType;
    
    /* Cfg_cmp bit set to 1 once configuration is complete. */
    /* Lock arbiter configtuation */
    USBFS_ARB_CFG_REG |= (uint8)  USBFS_ARB_CFG_CFG_CMP;
    /* Cfg_cmp bit set to 0 during configuration of PFSUSB Registers. */
    USBFS_ARB_CFG_REG &= (uint8) ~USBFS_ARB_CFG_CFG_CMP;

#endif /* (USBFS_EP_MANAGEMENT_DMA_AUTO) */

    /* Enable interrupt SIE interurpt source from EP0-EP1 */
    USBFS_SIE_EP_INT_EN_REG = (uint8) USBFS_DEFAULT_SIE_EP_INT_EN;
}


/*******************************************************************************
* Function Name: USBFS_EpStateInit
****************************************************************************//**
*
*  This routine initialize state of Data end points based of its type: 
*   IN  - USBFS_IN_BUFFER_EMPTY (USBFS_EVENT_PENDING)
*   OUT - USBFS_OUT_BUFFER_EMPTY (USBFS_NO_EVENT_PENDING)
*
*******************************************************************************/
void USBFS_EpStateInit(void) 
{
    uint8 i;

    for (i = USBFS_EP1; i < USBFS_MAX_EP; i++)
    { 
        if (0u != (USBFS_EP[i].addr & USBFS_DIR_IN))
        {
            /* IN Endpoint */
            USBFS_EP[i].apiEpState = USBFS_EVENT_PENDING;
        }
        else
        {
            /* OUT Endpoint */
            USBFS_EP[i].apiEpState = USBFS_NO_EVENT_PENDING;
        }
    }
                    
}


/*******************************************************************************
* Function Name: USBFS_Config
****************************************************************************//**
*
*  This routine configures endpoints for the entire configuration by scanning
*  the configuration descriptor.
*
*  \param clearAltSetting: It configures the bAlternateSetting 0 for each interface.
*
* USBFS_interfaceClass - Initialized class array for each interface.
*   It is used for handling Class specific requests depend on interface class.
*   Different classes in multiple Alternate settings does not supported.
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBFS_Config(uint8 clearAltSetting) 
{
    uint8 ep;
    uint8 curEp;
    uint8 i;
    uint8 epType;
    const uint8 *pDescr;
    
    #if (!USBFS_EP_MANAGEMENT_DMA_AUTO)
        uint16 buffCount = 0u;
    #endif /* (!USBFS_EP_MANAGEMENT_DMA_AUTO) */

    const T_USBFS_LUT CYCODE *pTmp;
    const T_USBFS_EP_SETTINGS_BLOCK CYCODE *pEP;

    /* Clear endpoints settings */
    for (ep = 0u; ep < USBFS_MAX_EP; ++ep)
    {
        USBFS_EP[ep].attrib     = 0u;
        USBFS_EP[ep].hwEpState  = 0u;
        USBFS_EP[ep].epToggle   = 0u;
        USBFS_EP[ep].bufferSize = 0u;
        USBFS_EP[ep].interface  = 0u;
        USBFS_EP[ep].apiEpState = USBFS_NO_EVENT_PENDING;
        USBFS_EP[ep].epMode     = USBFS_MODE_DISABLE;   
    }

    /* Clear Alternate settings for all interfaces. */
    if (0u != clearAltSetting)
    {
        for (i = 0u; i < USBFS_MAX_INTERFACES_NUMBER; ++i)
        {
            USBFS_interfaceSetting[i]     = 0u;
            USBFS_interfaceSettingLast[i] = 0u;
        }
    }

    /* Init Endpoints and Device Status if configured */
    if (USBFS_configuration > 0u)
    {
        #if defined(USBFS_ENABLE_CDC_CLASS)
            uint8 cdcComNums = 0u;
        #endif  /* (USBFS_ENABLE_CDC_CLASS) */  

        pTmp = USBFS_GetConfigTablePtr(USBFS_configuration - 1u);
        
        /* Set Power status for current configuration */
        pDescr = (const uint8 *)pTmp->p_list;
        if ((pDescr[USBFS_CONFIG_DESCR_ATTRIB] & USBFS_CONFIG_DESCR_ATTRIB_SELF_POWERED) != 0u)
        {
            USBFS_deviceStatus |= (uint8)  USBFS_DEVICE_STATUS_SELF_POWERED;
        }
        else
        {
            USBFS_deviceStatus &= (uint8) ~USBFS_DEVICE_STATUS_SELF_POWERED;
        }
        
        /* Move to next element */
        pTmp = &pTmp[1u];
        ep = pTmp->c;  /* For this table, c is the number of endpoints configurations  */

        #if (USBFS_EP_MANAGEMENT_MANUAL && USBFS_EP_ALLOC_DYNAMIC)
            /* Configure for dynamic EP memory allocation */
            /* p_list points the endpoint setting table. */
            pEP = (T_USBFS_EP_SETTINGS_BLOCK *) pTmp->p_list;
            
            for (i = 0u; i < ep; i++)
            {     
                /* Compare current Alternate setting with EP Alt */
                if (USBFS_interfaceSetting[pEP->interface] == pEP->altSetting)
                {                                                          
                    curEp  = pEP->addr & USBFS_DIR_UNUSED;
                    epType = pEP->attributes & USBFS_EP_TYPE_MASK;
                    
                    USBFS_EP[curEp].addr       = pEP->addr;
                    USBFS_EP[curEp].attrib     = pEP->attributes;
                    USBFS_EP[curEp].bufferSize = pEP->bufferSize;

                    if (0u != (pEP->addr & USBFS_DIR_IN))
                    {
                        /* IN Endpoint */
                        USBFS_EP[curEp].epMode     = USBFS_GET_ACTIVE_IN_EP_CR0_MODE(epType);
                        USBFS_EP[curEp].apiEpState = USBFS_EVENT_PENDING;
                    
                    #if (defined(USBFS_ENABLE_MIDI_STREAMING) && (USBFS_MIDI_IN_BUFF_SIZE > 0))
                        if ((pEP->bMisc == USBFS_CLASS_AUDIO) && (epType == USBFS_EP_TYPE_BULK))
                        {
                            USBFS_midi_in_ep = curEp;
                        }
                    #endif  /* (USBFS_ENABLE_MIDI_STREAMING) */
                    }
                    else
                    {
                        /* OUT Endpoint */
                        USBFS_EP[curEp].epMode     = USBFS_GET_ACTIVE_OUT_EP_CR0_MODE(epType);
                        USBFS_EP[curEp].apiEpState = USBFS_NO_EVENT_PENDING;
                        
                    #if (defined(USBFS_ENABLE_MIDI_STREAMING) && (USBFS_MIDI_OUT_BUFF_SIZE > 0))
                        if ((pEP->bMisc == USBFS_CLASS_AUDIO) && (epType == USBFS_EP_TYPE_BULK))
                        {
                            USBFS_midi_out_ep = curEp;
                        }
                    #endif  /* (USBFS_ENABLE_MIDI_STREAMING) */
                    }

                #if(defined (USBFS_ENABLE_CDC_CLASS))
                    if((pEP->bMisc == USBFS_CLASS_CDC_DATA) ||(pEP->bMisc == USBFS_CLASS_CDC))
                    {
                        cdcComNums = USBFS_Cdc_EpInit(pEP, curEp, cdcComNums);
                    }
                #endif  /* (USBFS_ENABLE_CDC_CLASS) */
                }
                
                pEP = &pEP[1u];
            }
            
        #else
            for (i = USBFS_EP1; i < USBFS_MAX_EP; ++i)
            {
                /* p_list points the endpoint setting table. */
                pEP = (const T_USBFS_EP_SETTINGS_BLOCK CYCODE *) pTmp->p_list;
                /* Find max length for each EP and select it (length could be different in different Alt settings) */
                /* but other settings should be correct with regards to Interface alt Setting */
                
                for (curEp = 0u; curEp < ep; ++curEp)
                {
                    if (i == (pEP->addr & USBFS_DIR_UNUSED))
                    {
                        /* Compare endpoint buffers size with current size to find greater. */
                        if (USBFS_EP[i].bufferSize < pEP->bufferSize)
                        {
                            USBFS_EP[i].bufferSize = pEP->bufferSize;
                        }
                        
                        /* Compare current Alternate setting with EP Alt */
                        if (USBFS_interfaceSetting[pEP->interface] == pEP->altSetting)
                        {                            
                            USBFS_EP[i].addr = pEP->addr;
                            USBFS_EP[i].attrib = pEP->attributes;
                            
                            epType = pEP->attributes & USBFS_EP_TYPE_MASK;
                            
                            if (0u != (pEP->addr & USBFS_DIR_IN))
                            {
                                /* IN Endpoint */
                                USBFS_EP[i].epMode     = USBFS_GET_ACTIVE_IN_EP_CR0_MODE(epType);
                                USBFS_EP[i].apiEpState = USBFS_EVENT_PENDING;
                                
                            #if (defined(USBFS_ENABLE_MIDI_STREAMING) && (USBFS_MIDI_IN_BUFF_SIZE > 0))
                                if ((pEP->bMisc == USBFS_CLASS_AUDIO) && (epType == USBFS_EP_TYPE_BULK))
                                {
                                    USBFS_midi_in_ep = i;
                                }
                            #endif  /* (USBFS_ENABLE_MIDI_STREAMING) */
                            }
                            else
                            {
                                /* OUT Endpoint */
                                USBFS_EP[i].epMode     = USBFS_GET_ACTIVE_OUT_EP_CR0_MODE(epType);
                                USBFS_EP[i].apiEpState = USBFS_NO_EVENT_PENDING;
                                
                            #if (defined(USBFS_ENABLE_MIDI_STREAMING) && (USBFS_MIDI_OUT_BUFF_SIZE > 0))
                                if ((pEP->bMisc == USBFS_CLASS_AUDIO) && (epType == USBFS_EP_TYPE_BULK))
                                {
                                    USBFS_midi_out_ep = i;
                                }
                            #endif  /* (USBFS_ENABLE_MIDI_STREAMING) */
                            }

                        #if (defined(USBFS_ENABLE_CDC_CLASS))
                            if((pEP->bMisc == USBFS_CLASS_CDC_DATA) ||(pEP->bMisc == USBFS_CLASS_CDC))
                            {
                                cdcComNums = USBFS_Cdc_EpInit(pEP, i, cdcComNums);
                            }
                        #endif  /* (USBFS_ENABLE_CDC_CLASS) */

                            #if (USBFS_EP_MANAGEMENT_DMA_AUTO)
                                break;  /* Use first EP setting in Auto memory management */
                            #endif /* (USBFS_EP_MANAGEMENT_DMA_AUTO) */
                        }
                    }
                    
                    pEP = &pEP[1u];
                }
            }
        #endif /*  (USBFS_EP_MANAGEMENT_MANUAL && USBFS_EP_ALLOC_DYNAMIC) */

        /* Init class array for each interface and interface number for each EP.
        *  It is used for handling Class specific requests directed to either an
        *  interface or the endpoint.
        */
        /* p_list points the endpoint setting table. */
        pEP = (const T_USBFS_EP_SETTINGS_BLOCK CYCODE *) pTmp->p_list;
        for (i = 0u; i < ep; i++)
        {
            /* Configure interface number for each EP */
            USBFS_EP[pEP->addr & USBFS_DIR_UNUSED].interface = pEP->interface;
            pEP = &pEP[1u];
        }
        
        /* Init pointer on interface class table */
        USBFS_interfaceClass = USBFS_GetInterfaceClassTablePtr();
        
    /* Set the endpoint buffer addresses */
    #if (!USBFS_EP_MANAGEMENT_DMA_AUTO)
        buffCount = 0u;
        for (ep = USBFS_EP1; ep < USBFS_MAX_EP; ++ep)
        {
            USBFS_EP[ep].buffOffset = buffCount;        
            buffCount += USBFS_EP[ep].bufferSize;
            
        #if (USBFS_GEN_16BITS_EP_ACCESS)
            /* Align EP buffers to be event size to access 16-bits DR register. */
            buffCount += (0u != (buffCount & 0x01u)) ? 1u : 0u;
        #endif /* (USBFS_GEN_16BITS_EP_ACCESS) */            
        }
    #endif /* (!USBFS_EP_MANAGEMENT_DMA_AUTO) */

        /* Configure hardware registers */
        USBFS_ConfigReg();
    }
}


/*******************************************************************************
* Function Name: USBFS_ConfigAltChanged
****************************************************************************//**
*
*  This routine update configuration for the required endpoints only.
*  It is called after SET_INTERFACE request when Static memory allocation used.
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBFS_ConfigAltChanged(void) 
{
    uint8 ep;
    uint8 curEp;
    uint8 epType;
    uint8 i;
    uint8 interfaceNum;

    const T_USBFS_LUT CYCODE *pTmp;
    const T_USBFS_EP_SETTINGS_BLOCK CYCODE *pEP;

    /* Init Endpoints and Device Status if configured */
    if (USBFS_configuration > 0u)
    {
        /* Get number of endpoints configurations (ep). */
        pTmp = USBFS_GetConfigTablePtr(USBFS_configuration - 1u);
        pTmp = &pTmp[1u];
        ep = pTmp->c;

        /* Get pointer to endpoints setting table (pEP). */
        pEP = (const T_USBFS_EP_SETTINGS_BLOCK CYCODE *) pTmp->p_list;
        
        /* Look through all possible endpoint configurations. Find endpoints 
        * which belong to current interface and alternate settings for 
        * re-configuration.
        */
        interfaceNum = USBFS_interfaceNumber;
        for (i = 0u; i < ep; i++)
        {
            /* Find endpoints which belong to current interface and alternate settings. */
            if ((interfaceNum == pEP->interface) && 
                (USBFS_interfaceSetting[interfaceNum] == pEP->altSetting))
            {
                curEp  = ((uint8) pEP->addr & USBFS_DIR_UNUSED);
                epType = ((uint8) pEP->attributes & USBFS_EP_TYPE_MASK);
                
                /* Change the SIE mode for the selected EP to NAK ALL */
                USBFS_EP[curEp].epToggle   = 0u;
                USBFS_EP[curEp].addr       = pEP->addr;
                USBFS_EP[curEp].attrib     = pEP->attributes;
                USBFS_EP[curEp].bufferSize = pEP->bufferSize;

                if (0u != (pEP->addr & USBFS_DIR_IN))
                {
                    /* IN Endpoint */
                    USBFS_EP[curEp].epMode     = USBFS_GET_ACTIVE_IN_EP_CR0_MODE(epType);
                    USBFS_EP[curEp].apiEpState = USBFS_EVENT_PENDING;
                }
                else
                {
                    /* OUT Endpoint */
                    USBFS_EP[curEp].epMode     = USBFS_GET_ACTIVE_OUT_EP_CR0_MODE(epType);
                    USBFS_EP[curEp].apiEpState = USBFS_NO_EVENT_PENDING;
                }
                
                /* Make SIE to NAK any endpoint requests */
                USBFS_SIE_EP_BASE.sieEp[curEp].epCr0 = USBFS_MODE_NAK_IN_OUT;

            #if (USBFS_EP_MANAGEMENT_DMA_AUTO)
                /* Clear IN data ready. */
                USBFS_ARB_EP_BASE.arbEp[curEp].epCfg &= (uint8) ~USBFS_ARB_EPX_CFG_IN_DATA_RDY;

                /* Select endpoint number of reconfiguration */
                USBFS_DYN_RECONFIG_REG = (uint8) ((curEp - 1u) << USBFS_DYN_RECONFIG_EP_SHIFT);
                
                /* Request for dynamic re-configuration of endpoint. */
                USBFS_DYN_RECONFIG_REG |= USBFS_DYN_RECONFIG_ENABLE;
                
                /* Wait until block is ready for re-configuration */
                while (0u == (USBFS_DYN_RECONFIG_REG & USBFS_DYN_RECONFIG_RDY_STS))
                {
                }
                
                /* Once DYN_RECONFIG_RDY_STS bit is set, FW can change the EP configuration. */
                /* Change EP Type with new direction */
                if (0u != (pEP->addr & USBFS_DIR_IN))
                {
                    /* Set endpoint type: 0 - IN and 1 - OUT. */
                    USBFS_EP_TYPE_REG &= (uint8) ~(uint8)((uint8) 0x01u << (curEp - 1u));
                    
                #if (CY_PSOC4)
                    /* Clear DMA_TERMIN for IN endpoint */
                    USBFS_ARB_EP_BASE.arbEp[curEp].epIntEn &= (uint32) ~USBFS_ARB_EPX_INT_DMA_TERMIN;
                #endif /* (CY_PSOC4) */
                }
                else
                {
                    /* Set endpoint type: 0 - IN and 1- OUT. */
                    USBFS_EP_TYPE_REG |= (uint8) ((uint8) 0x01u << (curEp - 1u));
                    
                #if (CY_PSOC4)
                    /* Set DMA_TERMIN for OUT endpoint */
                    USBFS_ARB_EP_BASE.arbEp[curEp].epIntEn |= (uint32) USBFS_ARB_EPX_INT_DMA_TERMIN;
                #endif /* (CY_PSOC4) */
                }
                
                /* Complete dynamic re-configuration: all endpoint related status and signals 
                * are set into the default state.
                */
                USBFS_DYN_RECONFIG_REG &= (uint8) ~USBFS_DYN_RECONFIG_ENABLE;

            #else
                USBFS_SIE_EP_BASE.sieEp[curEp].epCnt0 = HI8(USBFS_EP[curEp].bufferSize);
                USBFS_SIE_EP_BASE.sieEp[curEp].epCnt1 = LO8(USBFS_EP[curEp].bufferSize);
                
                #if (CY_PSOC4)
                    USBFS_ARB_EP16_BASE.arbEp[curEp].rwRa16  = (uint32) USBFS_EP[curEp].buffOffset;
                    USBFS_ARB_EP16_BASE.arbEp[curEp].rwWa16  = (uint32) USBFS_EP[curEp].buffOffset;
                #else
                    USBFS_ARB_EP_BASE.arbEp[curEp].rwRa    = LO8(USBFS_EP[curEp].buffOffset);
                    USBFS_ARB_EP_BASE.arbEp[curEp].rwRaMsb = HI8(USBFS_EP[curEp].buffOffset);
                    USBFS_ARB_EP_BASE.arbEp[curEp].rwWa    = LO8(USBFS_EP[curEp].buffOffset);
                    USBFS_ARB_EP_BASE.arbEp[curEp].rwWaMsb = HI8(USBFS_EP[curEp].buffOffset);
                #endif /* (CY_PSOC4) */                
            #endif /* (USBFS_EP_MANAGEMENT_DMA_AUTO) */
            }
            
            pEP = &pEP[1u]; /* Get next EP element */
        }
        
        /* The main loop has to re-enable DMA and OUT endpoint */
    }
}


/*******************************************************************************
* Function Name: USBFS_GetConfigTablePtr
****************************************************************************//**
*
*  This routine returns a pointer a configuration table entry
*
*  \param confIndex:  Configuration Index
*
* \return
*  Device Descriptor pointer or NULL when descriptor does not exist.
*
*******************************************************************************/
const T_USBFS_LUT CYCODE *USBFS_GetConfigTablePtr(uint8 confIndex)
                                                        
{
    /* Device Table */
    const T_USBFS_LUT CYCODE *pTmp;

    pTmp = (const T_USBFS_LUT CYCODE *) USBFS_TABLE[USBFS_device].p_list;

    /* The first entry points to the Device Descriptor,
    *  the second entry point to the BOS Descriptor
    *  the rest configuration entries.
    *  Set pointer to the first Configuration Descriptor
    */
    pTmp = &pTmp[2u];
    /* For this table, c is the number of configuration descriptors  */
    if(confIndex >= pTmp->c)   /* Verify that required configuration descriptor exists */
    {
        pTmp = (const T_USBFS_LUT CYCODE *) NULL;
    }
    else
    {
        pTmp = (const T_USBFS_LUT CYCODE *) pTmp[confIndex].p_list;
    }

    return (pTmp);
}


#if (USBFS_BOS_ENABLE)
    /*******************************************************************************
    * Function Name: USBFS_GetBOSPtr
    ****************************************************************************//**
    *
    *  This routine returns a pointer a BOS table entry
    *
    *  
    *
    * \return
    *  BOS Descriptor pointer or NULL when descriptor does not exist.
    *
    *******************************************************************************/
    const T_USBFS_LUT CYCODE *USBFS_GetBOSPtr(void)
                                                            
    {
        /* Device Table */
        const T_USBFS_LUT CYCODE *pTmp;

        pTmp = (const T_USBFS_LUT CYCODE *) USBFS_TABLE[USBFS_device].p_list;

        /* The first entry points to the Device Descriptor,
        *  the second entry points to the BOS Descriptor
        */
        pTmp = &pTmp[1u];
        pTmp = (const T_USBFS_LUT CYCODE *) pTmp->p_list;
        return (pTmp);
    }
#endif /* (USBFS_BOS_ENABLE) */


/*******************************************************************************
* Function Name: USBFS_GetDeviceTablePtr
****************************************************************************//**
*
*  This routine returns a pointer to the Device table
*
* \return
*  Device Table pointer
*
*******************************************************************************/
const T_USBFS_LUT CYCODE *USBFS_GetDeviceTablePtr(void)
                                                            
{
    /* Device Table */
    return( (const T_USBFS_LUT CYCODE *) USBFS_TABLE[USBFS_device].p_list );
}


/*******************************************************************************
* Function Name: USB_GetInterfaceClassTablePtr
****************************************************************************//**
*
*  This routine returns Interface Class table pointer, which contains
*  the relation between interface number and interface class.
*
* \return
*  Interface Class table pointer.
*
*******************************************************************************/
const uint8 CYCODE *USBFS_GetInterfaceClassTablePtr(void)
                                                        
{
    const T_USBFS_LUT CYCODE *pTmp;
    const uint8 CYCODE *pInterfaceClass;
    uint8 currentInterfacesNum;

    pTmp = USBFS_GetConfigTablePtr(USBFS_configuration - 1u);
    if (pTmp != NULL)
    {
        currentInterfacesNum  = ((const uint8 *) pTmp->p_list)[USBFS_CONFIG_DESCR_NUM_INTERFACES];
        /* Third entry in the LUT starts the Interface Table pointers */
        /* The INTERFACE_CLASS table is located after all interfaces */
        pTmp = &pTmp[currentInterfacesNum + 2u];
        pInterfaceClass = (const uint8 CYCODE *) pTmp->p_list;
    }
    else
    {
        pInterfaceClass = (const uint8 CYCODE *) NULL;
    }

    return (pInterfaceClass);
}


/*******************************************************************************
* Function Name: USBFS_TerminateEP
****************************************************************************//**
*
*  This function terminates the specified USBFS endpoint.
*  This function should be used before endpoint reconfiguration.
*
*  \param ep Contains the data endpoint number.
*
*  \reentrant
*  No.
*
* \sideeffect
* 
* The device responds with a NAK for any transactions on the selected endpoint.
*   
*******************************************************************************/
void USBFS_TerminateEP(uint8 epNumber) 
{
    /* Get endpoint number */
    epNumber &= USBFS_DIR_UNUSED;

    if ((epNumber > USBFS_EP0) && (epNumber < USBFS_MAX_EP))
    {
        /* Set the endpoint Halt */
        USBFS_EP[epNumber].hwEpState |= USBFS_ENDPOINT_STATUS_HALT;

        /* Clear the data toggle */
        USBFS_EP[epNumber].epToggle = 0u;
        USBFS_EP[epNumber].apiEpState = USBFS_NO_EVENT_ALLOWED;

        if ((USBFS_EP[epNumber].addr & USBFS_DIR_IN) != 0u)
        {   
            /* IN Endpoint */
            USBFS_SIE_EP_BASE.sieEp[epNumber].epCr0 = USBFS_MODE_NAK_IN;
        }
        else
        {
            /* OUT Endpoint */
            USBFS_SIE_EP_BASE.sieEp[epNumber].epCr0 = USBFS_MODE_NAK_OUT;
        }
    }
}


/*******************************************************************************
* Function Name: USBFS_SetEndpointHalt
****************************************************************************//**
*
*  This routine handles set endpoint halt.
*
* \return
*  requestHandled.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USBFS_SetEndpointHalt(void) 
{
    uint8 requestHandled = USBFS_FALSE;
    uint8 ep;
    
    /* Set endpoint halt */
    ep = USBFS_wIndexLoReg & USBFS_DIR_UNUSED;

    if ((ep > USBFS_EP0) && (ep < USBFS_MAX_EP))
    {
        /* Set the endpoint Halt */
        USBFS_EP[ep].hwEpState |= (USBFS_ENDPOINT_STATUS_HALT);

        /* Clear the data toggle */
        USBFS_EP[ep].epToggle = 0u;
        USBFS_EP[ep].apiEpState |= USBFS_NO_EVENT_ALLOWED;

        if ((USBFS_EP[ep].addr & USBFS_DIR_IN) != 0u)
        {
            /* IN Endpoint */
            USBFS_SIE_EP_BASE.sieEp[ep].epCr0 = (USBFS_MODE_STALL_DATA_EP | 
                                                            USBFS_MODE_ACK_IN);
        }
        else
        {
            /* OUT Endpoint */
            USBFS_SIE_EP_BASE.sieEp[ep].epCr0 = (USBFS_MODE_STALL_DATA_EP | 
                                                            USBFS_MODE_ACK_OUT);
        }
        requestHandled = USBFS_InitNoDataControlTransfer();
    }

    return (requestHandled);
}


/*******************************************************************************
* Function Name: USBFS_ClearEndpointHalt
****************************************************************************//**
*
*  This routine handles clear endpoint halt.
*
* \return
*  requestHandled.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USBFS_ClearEndpointHalt(void) 
{
    uint8 requestHandled = USBFS_FALSE;
    uint8 ep;

    /* Clear endpoint halt */
    ep = USBFS_wIndexLoReg & USBFS_DIR_UNUSED;

    if ((ep > USBFS_EP0) && (ep < USBFS_MAX_EP))
    {
        /* Clear the endpoint Halt */
        USBFS_EP[ep].hwEpState &= (uint8) ~USBFS_ENDPOINT_STATUS_HALT;

        /* Clear the data toggle */
        USBFS_EP[ep].epToggle = 0u;
        
        /* Clear toggle bit for already armed packet */
        USBFS_SIE_EP_BASE.sieEp[ep].epCnt0 = (uint8) ~(uint8)USBFS_EPX_CNT_DATA_TOGGLE;
        
        /* Return API State as it was defined before */
        USBFS_EP[ep].apiEpState &= (uint8) ~USBFS_NO_EVENT_ALLOWED;

        if ((USBFS_EP[ep].addr & USBFS_DIR_IN) != 0u)
        {
            /* IN Endpoint */
            if(USBFS_EP[ep].apiEpState == USBFS_IN_BUFFER_EMPTY)
            {       
                /* Wait for next packet from application */
                USBFS_SIE_EP_BASE.sieEp[ep].epCr0 = USBFS_MODE_NAK_IN;
            }
            else    /* Continue armed transfer */
            {
                USBFS_SIE_EP_BASE.sieEp[ep].epCr0 = USBFS_MODE_ACK_IN;
            }
        }
        else
        {
            /* OUT Endpoint */
            if (USBFS_EP[ep].apiEpState == USBFS_OUT_BUFFER_FULL)
            {       
                /* Allow application to read full buffer */
                USBFS_SIE_EP_BASE.sieEp[ep].epCr0 = USBFS_MODE_NAK_OUT;
            }
            else    /* Mark endpoint as empty, so it will be reloaded */
            {
                USBFS_SIE_EP_BASE.sieEp[ep].epCr0 = USBFS_MODE_ACK_OUT;
            }
        }
        
        requestHandled = USBFS_InitNoDataControlTransfer();
    }

    return(requestHandled);
}


/*******************************************************************************
* Function Name: USBFS_ValidateAlternateSetting
****************************************************************************//**
*
*  Validates (and records) a SET INTERFACE request.
*
* \return
*  requestHandled.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USBFS_ValidateAlternateSetting(void) 
{
    uint8 requestHandled = USBFS_FALSE;
    
    uint8 interfaceNum;
    uint8 curInterfacesNum;
    const T_USBFS_LUT CYCODE *pTmp;
    
    /* Get interface number from the request. */
    interfaceNum = (uint8) USBFS_wIndexLoReg;
    
    /* Get number of interfaces for current configuration. */
    pTmp = USBFS_GetConfigTablePtr(USBFS_configuration - 1u);
    curInterfacesNum  = ((const uint8 *) pTmp->p_list)[USBFS_CONFIG_DESCR_NUM_INTERFACES];

    /* Validate that interface number is within range. */
    if ((interfaceNum <= curInterfacesNum) || (interfaceNum <= USBFS_MAX_INTERFACES_NUMBER))
    {
        /* Save current and new alternate settings (come with request) to make 
        * desicion about following endpoint re-configuration.
        */
        USBFS_interfaceSettingLast[interfaceNum] = USBFS_interfaceSetting[interfaceNum];
        USBFS_interfaceSetting[interfaceNum]     = (uint8) USBFS_wValueLoReg;
        
        requestHandled = USBFS_TRUE;
    }

    return (requestHandled);
}


/* [] END OF FILE */
