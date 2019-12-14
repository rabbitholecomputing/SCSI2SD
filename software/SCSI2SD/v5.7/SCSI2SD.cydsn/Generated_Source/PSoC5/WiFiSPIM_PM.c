/*******************************************************************************
* File Name: WiFiSPIM_PM.c
* Version 2.50
*
* Description:
*  This file contains the setup, control and status commands to support
*  component operations in low power mode.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "WiFiSPIM_PVT.h"

static WiFiSPIM_BACKUP_STRUCT WiFiSPIM_backup =
{
    WiFiSPIM_DISABLED,
    WiFiSPIM_BITCTR_INIT,
};


/*******************************************************************************
* Function Name: WiFiSPIM_SaveConfig
********************************************************************************
*
* Summary:
*  Empty function. Included for consistency with other components.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void WiFiSPIM_SaveConfig(void) 
{

}


/*******************************************************************************
* Function Name: WiFiSPIM_RestoreConfig
********************************************************************************
*
* Summary:
*  Empty function. Included for consistency with other components.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void WiFiSPIM_RestoreConfig(void) 
{

}


/*******************************************************************************
* Function Name: WiFiSPIM_Sleep
********************************************************************************
*
* Summary:
*  Prepare SPIM Component goes to sleep.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  WiFiSPIM_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void WiFiSPIM_Sleep(void) 
{
    /* Save components enable state */
    WiFiSPIM_backup.enableState = ((uint8) WiFiSPIM_IS_ENABLED);

    WiFiSPIM_Stop();
}


/*******************************************************************************
* Function Name: WiFiSPIM_Wakeup
********************************************************************************
*
* Summary:
*  Prepare SPIM Component to wake up.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  WiFiSPIM_backup - used when non-retention registers are restored.
*  WiFiSPIM_txBufferWrite - modified every function call - resets to
*  zero.
*  WiFiSPIM_txBufferRead - modified every function call - resets to
*  zero.
*  WiFiSPIM_rxBufferWrite - modified every function call - resets to
*  zero.
*  WiFiSPIM_rxBufferRead - modified every function call - resets to
*  zero.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void WiFiSPIM_Wakeup(void) 
{
    #if(WiFiSPIM_RX_SOFTWARE_BUF_ENABLED)
        WiFiSPIM_rxBufferFull  = 0u;
        WiFiSPIM_rxBufferRead  = 0u;
        WiFiSPIM_rxBufferWrite = 0u;
    #endif /* (WiFiSPIM_RX_SOFTWARE_BUF_ENABLED) */

    #if(WiFiSPIM_TX_SOFTWARE_BUF_ENABLED)
        WiFiSPIM_txBufferFull  = 0u;
        WiFiSPIM_txBufferRead  = 0u;
        WiFiSPIM_txBufferWrite = 0u;
    #endif /* (WiFiSPIM_TX_SOFTWARE_BUF_ENABLED) */

    /* Clear any data from the RX and TX FIFO */
    WiFiSPIM_ClearFIFO();

    /* Restore components block enable state */
    if(0u != WiFiSPIM_backup.enableState)
    {
        WiFiSPIM_Enable();
    }
}


/* [] END OF FILE */
