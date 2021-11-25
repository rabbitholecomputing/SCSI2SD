/*******************************************************************************
* File Name: NOR_SPI_PM.c
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

#include "NOR_SPI_PVT.h"

static NOR_SPI_BACKUP_STRUCT NOR_SPI_backup =
{
    NOR_SPI_DISABLED,
    NOR_SPI_BITCTR_INIT,
};


/*******************************************************************************
* Function Name: NOR_SPI_SaveConfig
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
void NOR_SPI_SaveConfig(void) 
{

}


/*******************************************************************************
* Function Name: NOR_SPI_RestoreConfig
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
void NOR_SPI_RestoreConfig(void) 
{

}


/*******************************************************************************
* Function Name: NOR_SPI_Sleep
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
*  NOR_SPI_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void NOR_SPI_Sleep(void) 
{
    /* Save components enable state */
    NOR_SPI_backup.enableState = ((uint8) NOR_SPI_IS_ENABLED);

    NOR_SPI_Stop();
}


/*******************************************************************************
* Function Name: NOR_SPI_Wakeup
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
*  NOR_SPI_backup - used when non-retention registers are restored.
*  NOR_SPI_txBufferWrite - modified every function call - resets to
*  zero.
*  NOR_SPI_txBufferRead - modified every function call - resets to
*  zero.
*  NOR_SPI_rxBufferWrite - modified every function call - resets to
*  zero.
*  NOR_SPI_rxBufferRead - modified every function call - resets to
*  zero.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void NOR_SPI_Wakeup(void) 
{
    #if(NOR_SPI_RX_SOFTWARE_BUF_ENABLED)
        NOR_SPI_rxBufferFull  = 0u;
        NOR_SPI_rxBufferRead  = 0u;
        NOR_SPI_rxBufferWrite = 0u;
    #endif /* (NOR_SPI_RX_SOFTWARE_BUF_ENABLED) */

    #if(NOR_SPI_TX_SOFTWARE_BUF_ENABLED)
        NOR_SPI_txBufferFull  = 0u;
        NOR_SPI_txBufferRead  = 0u;
        NOR_SPI_txBufferWrite = 0u;
    #endif /* (NOR_SPI_TX_SOFTWARE_BUF_ENABLED) */

    /* Clear any data from the RX and TX FIFO */
    NOR_SPI_ClearFIFO();

    /* Restore components block enable state */
    if(0u != NOR_SPI_backup.enableState)
    {
        NOR_SPI_Enable();
    }
}


/* [] END OF FILE */
