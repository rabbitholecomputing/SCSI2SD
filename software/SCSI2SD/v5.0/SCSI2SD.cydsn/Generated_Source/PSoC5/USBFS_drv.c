/***************************************************************************//**
* \file USBFS_drv.c
* \version 3.10
*
* \brief
*  This file contains the Endpoint 0 Driver for the USBFS Component.  
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
* Global data allocation
***************************************/

volatile T_USBFS_EP_CTL_BLOCK USBFS_EP[USBFS_MAX_EP];

/** Contains the current configuration number, which is set by the host using a 
 * SET_CONFIGURATION request. This variable is initialized to zero in 
 * USBFS_InitComponent() API and can be read by the USBFS_GetConfiguration() 
 * API.*/
volatile uint8 USBFS_configuration;

/** Contains the current interface number.*/
volatile uint8 USBFS_interfaceNumber;

/** This variable is set to one after SET_CONFIGURATION and SET_INTERFACE 
 *requests. It can be read by the USBFS_IsConfigurationChanged() API */
volatile uint8 USBFS_configurationChanged;

/** Contains the current device address.*/
volatile uint8 USBFS_deviceAddress;

/** This is a two-bit variable that contains power status in the bit 0 
 * (DEVICE_STATUS_BUS_POWERED or DEVICE_STATUS_SELF_POWERED) and remote wakeup 
 * status (DEVICE_STATUS_REMOTE_WAKEUP) in the bit 1. This variable is 
 * initialized to zero in USBFS_InitComponent() API, configured by the 
 * USBFS_SetPowerStatus() API. The remote wakeup status cannot be set using the 
 * API SetPowerStatus(). */
volatile uint8 USBFS_deviceStatus;

volatile uint8 USBFS_interfaceSetting[USBFS_MAX_INTERFACES_NUMBER];
volatile uint8 USBFS_interfaceSetting_last[USBFS_MAX_INTERFACES_NUMBER];
volatile uint8 USBFS_interfaceStatus[USBFS_MAX_INTERFACES_NUMBER];

/** Contains the started device number. This variable is set by the 
 * USBFS_Start() or USBFS_InitComponent() APIs.*/
volatile uint8 USBFS_device;

/** Initialized class array for each interface. It is used for handling Class 
 * specific requests depend on interface class. Different classes in multiple 
 * alternate settings are not supported.*/
const uint8 CYCODE *USBFS_interfaceClass;


/***************************************
* Local data allocation
***************************************/

volatile uint8  USBFS_ep0Toggle;
volatile uint8  USBFS_lastPacketSize;

/** This variable is used by the communication functions to handle the current 
* transfer state.
* Initialized to TRANS_STATE_IDLE in the USBFS_InitComponent() API and after a 
* complete transfer in the status stage.
* Changed to the TRANS_STATE_CONTROL_READ or TRANS_STATE_CONTROL_WRITE in setup 
* transaction depending on the request type.
*/
volatile uint8  USBFS_transferState;
volatile T_USBFS_TD USBFS_currentTD;
volatile uint8  USBFS_ep0Mode;
volatile uint8  USBFS_ep0Count;
volatile uint16 USBFS_transferByteCount;


/*******************************************************************************
* Function Name: USBFS_ep_0_Interrupt
****************************************************************************//**
*
*  This Interrupt Service Routine handles Endpoint 0 (Control Pipe) traffic.
*  It dispatches setup requests and handles the data and status stages.
*
*
*******************************************************************************/
CY_ISR(USBFS_EP_0_ISR)
{
    uint8 tempReg;
    uint8 modifyReg;

#ifdef USBFS_EP_0_ISR_ENTRY_CALLBACK
    USBFS_EP_0_ISR_EntryCallback();
#endif /* (USBFS_EP_0_ISR_ENTRY_CALLBACK) */
    
    tempReg = USBFS_EP0_CR_REG;
    if ((tempReg & USBFS_MODE_ACKD) != 0u)
    {
        modifyReg = 1u;
        if ((tempReg & USBFS_MODE_SETUP_RCVD) != 0u)
        {
            if ((tempReg & USBFS_MODE_MASK) != USBFS_MODE_NAK_IN_OUT)
            {
                /* Mode not equal to NAK_IN_OUT: invalid setup */
                modifyReg = 0u;
            }
            else
            {
                USBFS_HandleSetup();
                
                if ((USBFS_ep0Mode & USBFS_MODE_SETUP_RCVD) != 0u)
                {
                    /* SETUP bit set: exit without mode modificaiton */
                    modifyReg = 0u;
                }
            }
        }
        else if ((tempReg & USBFS_MODE_IN_RCVD) != 0u)
        {
            USBFS_HandleIN();
        }
        else if ((tempReg & USBFS_MODE_OUT_RCVD) != 0u)
        {
            USBFS_HandleOUT();
        }
        else
        {
            modifyReg = 0u;
        }
        
        /* Modify the EP0_CR register */
        if (modifyReg != 0u)
        {
            
            tempReg = USBFS_EP0_CR_REG;
            
            /* Make sure that SETUP bit is cleared before modification */
            if ((tempReg & USBFS_MODE_SETUP_RCVD) == 0u)
            {
                /* Update count register */
                tempReg = (uint8) USBFS_ep0Toggle | USBFS_ep0Count;
                USBFS_EP0_CNT_REG = tempReg;
               
                /* Make sure that previous write operaiton was successful */
                if (tempReg == USBFS_EP0_CNT_REG)
                {
                    /* Repeat until next successful write operation */
                    do
                    {
                        /* Init temporary variable */
                        modifyReg = USBFS_ep0Mode;
                        
                        /* Unlock register */
                        tempReg = (uint8) (USBFS_EP0_CR_REG & USBFS_MODE_SETUP_RCVD);
                        
                        /* Check if SETUP bit is not set */
                        if (0u == tempReg)
                        {
                            /* Set the Mode Register  */
                            USBFS_EP0_CR_REG = USBFS_ep0Mode;
                            
                            /* Writing check */
                            modifyReg = USBFS_EP0_CR_REG & USBFS_MODE_MASK;
                        }
                    }
                    while (modifyReg != USBFS_ep0Mode);
                }
            }
        }
    }

    USBFS_ClearSieInterruptSource(USBFS_INTR_SIE_EP0_INTR);
	
#ifdef USBFS_EP_0_ISR_EXIT_CALLBACK
    USBFS_EP_0_ISR_ExitCallback();
#endif /* (USBFS_EP_0_ISR_EXIT_CALLBACK) */
}


/*******************************************************************************
* Function Name: USBFS_HandleSetup
****************************************************************************//**
*
*  This Routine dispatches requests for the four USB request types
*
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBFS_HandleSetup(void) 
{
    uint8 requestHandled;
    
    /* Clear register lock by SIE (read register) and clear setup bit 
    * (write any value in register).
    */
    requestHandled = (uint8) USBFS_EP0_CR_REG;
    USBFS_EP0_CR_REG = (uint8) requestHandled;
    requestHandled = (uint8) USBFS_EP0_CR_REG;

    if ((requestHandled & USBFS_MODE_SETUP_RCVD) != 0u)
    {
        /* SETUP bit is set: exit without mode modification. */
        USBFS_ep0Mode = requestHandled;
    }
    else
    {
        /* In case the previous transfer did not complete, close it out */
        USBFS_UpdateStatusBlock(USBFS_XFER_PREMATURE);

        /* Check request type. */
        switch (USBFS_bmRequestTypeReg & USBFS_RQST_TYPE_MASK)
        {
            case USBFS_RQST_TYPE_STD:
                requestHandled = USBFS_HandleStandardRqst();
                break;
                
            case USBFS_RQST_TYPE_CLS:
                requestHandled = USBFS_DispatchClassRqst();
                break;
                
            case USBFS_RQST_TYPE_VND:
                requestHandled = USBFS_HandleVendorRqst();
                break;
                
            default:
                requestHandled = USBFS_FALSE;
                break;
        }
        
        /* If request is not recognized. Stall endpoint 0 IN and OUT. */
        if (requestHandled == USBFS_FALSE)
        {
            USBFS_ep0Mode = USBFS_MODE_STALL_IN_OUT;
        }
    }
}


/*******************************************************************************
* Function Name: USBFS_HandleIN
****************************************************************************//**
*
*  This routine handles EP0 IN transfers.
*
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBFS_HandleIN(void) 
{
    switch (USBFS_transferState)
    {
        case USBFS_TRANS_STATE_IDLE:
            break;
        
        case USBFS_TRANS_STATE_CONTROL_READ:
            USBFS_ControlReadDataStage();
            break;
            
        case USBFS_TRANS_STATE_CONTROL_WRITE:
            USBFS_ControlWriteStatusStage();
            break;
            
        case USBFS_TRANS_STATE_NO_DATA_CONTROL:
            USBFS_NoDataControlStatusStage();
            break;
            
        default:    /* there are no more states */
            break;
    }
}


/*******************************************************************************
* Function Name: USBFS_HandleOUT
****************************************************************************//**
*
*  This routine handles EP0 OUT transfers.
*
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBFS_HandleOUT(void) 
{
    switch (USBFS_transferState)
    {
        case USBFS_TRANS_STATE_IDLE:
            break;
        
        case USBFS_TRANS_STATE_CONTROL_READ:
            USBFS_ControlReadStatusStage();
            break;
            
        case USBFS_TRANS_STATE_CONTROL_WRITE:
            USBFS_ControlWriteDataStage();
            break;
            
        case USBFS_TRANS_STATE_NO_DATA_CONTROL:
            /* Update the completion block */
            USBFS_UpdateStatusBlock(USBFS_XFER_ERROR);
            
            /* We expect no more data, so stall INs and OUTs */
            USBFS_ep0Mode = USBFS_MODE_STALL_IN_OUT;
            break;
            
        default:    
            /* There are no more states */
            break;
    }
}


/*******************************************************************************
* Function Name: USBFS_LoadEP0
****************************************************************************//**
*
*  This routine loads the EP0 data registers for OUT transfers. It uses the
*  currentTD (previously initialized by the _InitControlWrite function and
*  updated for each OUT transfer, and the bLastPacketSize) to determine how
*  many uint8s to transfer on the current OUT.
*
*  If the number of uint8s remaining is zero and the last transfer was full,
*  we need to send a zero length packet.  Otherwise we send the minimum
*  of the control endpoint size (8) or remaining number of uint8s for the
*  transaction.
*
*
* \globalvars
*  USBFS_transferByteCount - Update the transfer byte count from the
*     last transaction.
*  USBFS_ep0Count - counts the data loaded to the SIE memory in
*     current packet.
*  USBFS_lastPacketSize - remembers the USBFS_ep0Count value for the
*     next packet.
*  USBFS_transferByteCount - sum of the previous bytes transferred
*     on previous packets(sum of USBFS_lastPacketSize)
*  USBFS_ep0Toggle - inverted
*  USBFS_ep0Mode  - prepare for mode register content.
*  USBFS_transferState - set to TRANS_STATE_CONTROL_READ
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBFS_LoadEP0(void) 
{
    uint8 ep0Count = 0u;

    /* Update the transfer byte count from the last transaction */
    USBFS_transferByteCount += USBFS_lastPacketSize;

    /* Now load the next transaction */
    while ((USBFS_currentTD.count > 0u) && (ep0Count < 8u))
    {
        USBFS_EP0_DR_BASE.epData[ep0Count] = (uint8) *USBFS_currentTD.pData;
        USBFS_currentTD.pData = &USBFS_currentTD.pData[1u];
        ep0Count++;
        USBFS_currentTD.count--;
    }

    /* Support zero-length packet */
    if ((USBFS_lastPacketSize == 8u) || (ep0Count > 0u))
    {
        /* Update the data toggle */
        USBFS_ep0Toggle ^= USBFS_EP0_CNT_DATA_TOGGLE;
        /* Set the Mode Register  */
        USBFS_ep0Mode = USBFS_MODE_ACK_IN_STATUS_OUT;
        /* Update the state (or stay the same) */
        USBFS_transferState = USBFS_TRANS_STATE_CONTROL_READ;
    }
    else
    {
        /* Expect Status Stage Out */
        USBFS_ep0Mode = USBFS_MODE_STATUS_OUT_ONLY;
        /* Update the state (or stay the same) */
        USBFS_transferState = USBFS_TRANS_STATE_CONTROL_READ;
    }

    /* Save the packet size for next time */
    USBFS_ep0Count =       (uint8) ep0Count;
    USBFS_lastPacketSize = (uint8) ep0Count;
}


/*******************************************************************************
* Function Name: USBFS_InitControlRead
****************************************************************************//**
*
*  Initialize a control read transaction. It is used to send data to the host.
*  The following global variables should be initialized before this function
*  called. To send zero length packet use InitZeroLengthControlTransfer
*  function.
*
*
* \return
*  requestHandled state.
*
* \globalvars
*  USBFS_currentTD.count - counts of data to be sent.
*  USBFS_currentTD.pData - data pointer.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USBFS_InitControlRead(void) 
{
    uint16 xferCount;

    if (USBFS_currentTD.count == 0u)
    {
        (void) USBFS_InitZeroLengthControlTransfer();
    }
    else
    {
        /* Set up the state machine */
        USBFS_transferState = USBFS_TRANS_STATE_CONTROL_READ;
        
        /* Set the toggle, it gets updated in LoadEP */
        USBFS_ep0Toggle = 0u;
        
        /* Initialize the Status Block */
        USBFS_InitializeStatusBlock();
        
        xferCount = ((uint16)((uint16) USBFS_lengthHiReg << 8u) | ((uint16) USBFS_lengthLoReg));

        if (USBFS_currentTD.count > xferCount)
        {
            USBFS_currentTD.count = xferCount;
        }
        
        USBFS_LoadEP0();
    }

    return (USBFS_TRUE);
}


/*******************************************************************************
* Function Name: USBFS_InitZeroLengthControlTransfer
****************************************************************************//**
*
*  Initialize a zero length data IN transfer.
*
* \return
*  requestHandled state.
*
* \globalvars
*  USBFS_ep0Toggle - set to EP0_CNT_DATA_TOGGLE
*  USBFS_ep0Mode  - prepare for mode register content.
*  USBFS_transferState - set to TRANS_STATE_CONTROL_READ
*  USBFS_ep0Count - cleared, means the zero-length packet.
*  USBFS_lastPacketSize - cleared.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USBFS_InitZeroLengthControlTransfer(void)
                                                
{
    /* Update the state */
    USBFS_transferState = USBFS_TRANS_STATE_CONTROL_READ;
    
    /* Set the data toggle */
    USBFS_ep0Toggle = USBFS_EP0_CNT_DATA_TOGGLE;
    
    /* Set the Mode Register  */
    USBFS_ep0Mode = USBFS_MODE_ACK_IN_STATUS_OUT;
    
    /* Save the packet size for next time */
    USBFS_lastPacketSize = 0u;
    
    USBFS_ep0Count = 0u;

    return (USBFS_TRUE);
}


/*******************************************************************************
* Function Name: USBFS_ControlReadDataStage
****************************************************************************//**
*
*  Handle the Data Stage of a control read transfer.
*
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBFS_ControlReadDataStage(void) 

{
    USBFS_LoadEP0();
}


/*******************************************************************************
* Function Name: USBFS_ControlReadStatusStage
****************************************************************************//**
*
*  Handle the Status Stage of a control read transfer.
*
*
* \globalvars
*  USBFS_USBFS_transferByteCount - updated with last packet size.
*  USBFS_transferState - set to TRANS_STATE_IDLE.
*  USBFS_ep0Mode  - set to MODE_STALL_IN_OUT.
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBFS_ControlReadStatusStage(void) 
{
    /* Update the transfer byte count */
    USBFS_transferByteCount += USBFS_lastPacketSize;
    
    /* Go Idle */
    USBFS_transferState = USBFS_TRANS_STATE_IDLE;
    
    /* Update the completion block */
    USBFS_UpdateStatusBlock(USBFS_XFER_STATUS_ACK);
    
    /* We expect no more data, so stall INs and OUTs */
    USBFS_ep0Mode = USBFS_MODE_STALL_IN_OUT;
}


/*******************************************************************************
* Function Name: USBFS_InitControlWrite
****************************************************************************//**
*
*  Initialize a control write transaction
*
* \return
*  requestHandled state.
*
* \globalvars
*  USBFS_USBFS_transferState - set to TRANS_STATE_CONTROL_WRITE
*  USBFS_ep0Toggle - set to EP0_CNT_DATA_TOGGLE
*  USBFS_ep0Mode  - set to MODE_ACK_OUT_STATUS_IN
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USBFS_InitControlWrite(void) 
{
    uint16 xferCount;

    /* Set up the state machine */
    USBFS_transferState = USBFS_TRANS_STATE_CONTROL_WRITE;
    
    /* This might not be necessary */
    USBFS_ep0Toggle = USBFS_EP0_CNT_DATA_TOGGLE;
    
    /* Initialize the Status Block */
    USBFS_InitializeStatusBlock();

    xferCount = ((uint16)((uint16) USBFS_lengthHiReg << 8u) | ((uint16) USBFS_lengthLoReg));

    if (USBFS_currentTD.count > xferCount)
    {
        USBFS_currentTD.count = xferCount;
    }

    /* Expect Data or Status Stage */
    USBFS_ep0Mode = USBFS_MODE_ACK_OUT_STATUS_IN;

    return(USBFS_TRUE);
}


/*******************************************************************************
* Function Name: USBFS_ControlWriteDataStage
****************************************************************************//**
*
*  Handle the Data Stage of a control write transfer
*       1. Get the data (We assume the destination was validated previously)
*       2. Update the count and data toggle
*       3. Update the mode register for the next transaction
*
*
* \globalvars
*  USBFS_transferByteCount - Update the transfer byte count from the
*    last transaction.
*  USBFS_ep0Count - counts the data loaded from the SIE memory
*    in current packet.
*  USBFS_transferByteCount - sum of the previous bytes transferred
*    on previous packets(sum of USBFS_lastPacketSize)
*  USBFS_ep0Toggle - inverted
*  USBFS_ep0Mode  - set to MODE_ACK_OUT_STATUS_IN.
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBFS_ControlWriteDataStage(void) 
{
    uint8 ep0Count;
    uint8 regIndex = 0u;

    ep0Count = (USBFS_EP0_CNT_REG & USBFS_EPX_CNT0_MASK) - USBFS_EPX_CNTX_CRC_COUNT;

    USBFS_transferByteCount += (uint8)ep0Count;

    while ((USBFS_currentTD.count > 0u) && (ep0Count > 0u))
    {
        *USBFS_currentTD.pData = (uint8) USBFS_EP0_DR_BASE.epData[regIndex];
        USBFS_currentTD.pData = &USBFS_currentTD.pData[1u];
        regIndex++;
        ep0Count--;
        USBFS_currentTD.count--;
    }
    
    USBFS_ep0Count = (uint8)ep0Count;
    
    /* Update the data toggle */
    USBFS_ep0Toggle ^= USBFS_EP0_CNT_DATA_TOGGLE;
    
    /* Expect Data or Status Stage */
    USBFS_ep0Mode = USBFS_MODE_ACK_OUT_STATUS_IN;
}


/*******************************************************************************
* Function Name: USBFS_ControlWriteStatusStage
****************************************************************************//**
*
*  Handle the Status Stage of a control write transfer
*
* \globalvars
*  USBFS_transferState - set to TRANS_STATE_IDLE.
*  USBFS_USBFS_ep0Mode  - set to MODE_STALL_IN_OUT.
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBFS_ControlWriteStatusStage(void) 
{
    /* Go Idle */
    USBFS_transferState = USBFS_TRANS_STATE_IDLE;
    
    /* Update the completion block */    
    USBFS_UpdateStatusBlock(USBFS_XFER_STATUS_ACK);
    
    /* We expect no more data, so stall INs and OUTs */
    USBFS_ep0Mode = USBFS_MODE_STALL_IN_OUT;
}


/*******************************************************************************
* Function Name: USBFS_InitNoDataControlTransfer
****************************************************************************//**
*
*  Initialize a no data control transfer
*
* \return
*  requestHandled state.
*
* \globalvars
*  USBFS_transferState - set to TRANS_STATE_NO_DATA_CONTROL.
*  USBFS_ep0Mode  - set to MODE_STATUS_IN_ONLY.
*  USBFS_ep0Count - cleared.
*  USBFS_ep0Toggle - set to EP0_CNT_DATA_TOGGLE
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USBFS_InitNoDataControlTransfer(void) 
{
    USBFS_transferState = USBFS_TRANS_STATE_NO_DATA_CONTROL;
    USBFS_ep0Mode       = USBFS_MODE_STATUS_IN_ONLY;
    USBFS_ep0Toggle     = USBFS_EP0_CNT_DATA_TOGGLE;
    USBFS_ep0Count      = 0u;

    return (USBFS_TRUE);
}


/*******************************************************************************
* Function Name: USBFS_NoDataControlStatusStage
****************************************************************************//**
*  Handle the Status Stage of a no data control transfer.
*
*  SET_ADDRESS is special, since we need to receive the status stage with
*  the old address.
*
* \globalvars
*  USBFS_transferState - set to TRANS_STATE_IDLE.
*  USBFS_ep0Mode  - set to MODE_STALL_IN_OUT.
*  USBFS_ep0Toggle - set to EP0_CNT_DATA_TOGGLE
*  USBFS_deviceAddress - used to set new address and cleared
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBFS_NoDataControlStatusStage(void) 
{
    if (0u != USBFS_deviceAddress)
    {
        /* Update device address if we got new address. */
        USBFS_CR0_REG = (uint8) USBFS_deviceAddress | USBFS_CR0_ENABLE;
        USBFS_deviceAddress = 0u;
    }

    USBFS_transferState = USBFS_TRANS_STATE_IDLE;
    
    /* Update the completion block. */
    USBFS_UpdateStatusBlock(USBFS_XFER_STATUS_ACK);
    
    /* Stall IN and OUT, no more data is expected. */
    USBFS_ep0Mode = USBFS_MODE_STALL_IN_OUT;
}


/*******************************************************************************
* Function Name: USBFS_UpdateStatusBlock
****************************************************************************//**
*
*  Update the Completion Status Block for a Request.  The block is updated
*  with the completion code the USBFS_transferByteCount.  The
*  StatusBlock Pointer is set to NULL.
*
*  completionCode - status.
*
*
* \globalvars
*  USBFS_currentTD.pStatusBlock->status - updated by the
*    completionCode parameter.
*  USBFS_currentTD.pStatusBlock->length - updated.
*  USBFS_currentTD.pStatusBlock - cleared.
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBFS_UpdateStatusBlock(uint8 completionCode) 
{
    if (USBFS_currentTD.pStatusBlock != NULL)
    {
        USBFS_currentTD.pStatusBlock->status = completionCode;
        USBFS_currentTD.pStatusBlock->length = USBFS_transferByteCount;
        USBFS_currentTD.pStatusBlock = NULL;
    }
}


/*******************************************************************************
* Function Name: USBFS_InitializeStatusBlock
****************************************************************************//**
*
*  Initialize the Completion Status Block for a Request.  The completion
*  code is set to USB_XFER_IDLE.
*
*  Also, initializes USBFS_transferByteCount.  Save some space,
*  this is the only consumer.
*
* \globalvars
*  USBFS_currentTD.pStatusBlock->status - set to XFER_IDLE.
*  USBFS_currentTD.pStatusBlock->length - cleared.
*  USBFS_transferByteCount - cleared.
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBFS_InitializeStatusBlock(void) 
{
    USBFS_transferByteCount = 0u;
    
    if (USBFS_currentTD.pStatusBlock != NULL)
    {
        USBFS_currentTD.pStatusBlock->status = USBFS_XFER_IDLE;
        USBFS_currentTD.pStatusBlock->length = 0u;
    }
}


/* [] END OF FILE */
