/*******************************************************************************
* File Name: SPI_ETH_PM.c
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

#include "SPI_ETH_PVT.h"

static SPI_ETH_BACKUP_STRUCT SPI_ETH_backup =
{
    SPI_ETH_DISABLED,
    SPI_ETH_BITCTR_INIT,
};


/*******************************************************************************
* Function Name: SPI_ETH_SaveConfig
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
void SPI_ETH_SaveConfig(void) 
{

}


/*******************************************************************************
* Function Name: SPI_ETH_RestoreConfig
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
void SPI_ETH_RestoreConfig(void) 
{

}


/*******************************************************************************
* Function Name: SPI_ETH_Sleep
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
*  SPI_ETH_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SPI_ETH_Sleep(void) 
{
    /* Save components enable state */
    SPI_ETH_backup.enableState = ((uint8) SPI_ETH_IS_ENABLED);

    SPI_ETH_Stop();
}


/*******************************************************************************
* Function Name: SPI_ETH_Wakeup
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
*  SPI_ETH_backup - used when non-retention registers are restored.
*  SPI_ETH_txBufferWrite - modified every function call - resets to
*  zero.
*  SPI_ETH_txBufferRead - modified every function call - resets to
*  zero.
*  SPI_ETH_rxBufferWrite - modified every function call - resets to
*  zero.
*  SPI_ETH_rxBufferRead - modified every function call - resets to
*  zero.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SPI_ETH_Wakeup(void) 
{
    #if(SPI_ETH_RX_SOFTWARE_BUF_ENABLED)
        SPI_ETH_rxBufferFull  = 0u;
        SPI_ETH_rxBufferRead  = 0u;
        SPI_ETH_rxBufferWrite = 0u;
    #endif /* (SPI_ETH_RX_SOFTWARE_BUF_ENABLED) */

    #if(SPI_ETH_TX_SOFTWARE_BUF_ENABLED)
        SPI_ETH_txBufferFull  = 0u;
        SPI_ETH_txBufferRead  = 0u;
        SPI_ETH_txBufferWrite = 0u;
    #endif /* (SPI_ETH_TX_SOFTWARE_BUF_ENABLED) */

    /* Clear any data from the RX and TX FIFO */
    SPI_ETH_ClearFIFO();

    /* Restore components block enable state */
    if(0u != SPI_ETH_backup.enableState)
    {
        SPI_ETH_Enable();
    }
}


/* [] END OF FILE */
