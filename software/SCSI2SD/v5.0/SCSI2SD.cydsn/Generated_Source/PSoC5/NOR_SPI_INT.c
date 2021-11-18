/*******************************************************************************
* File Name: NOR_SPI_INT.c
* Version 2.50
*
* Description:
*  This file provides all Interrupt Service Routine (ISR) for the SPI Master
*  component.
*
* Note:
*  None.
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "NOR_SPI_PVT.h"


/* User code required at start of ISR */
/* `#START NOR_SPI_ISR_START_DEF` */

/* `#END` */


/*******************************************************************************
* Function Name: NOR_SPI_TX_ISR
********************************************************************************
*
* Summary:
*  Interrupt Service Routine for TX portion of the SPI Master.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  NOR_SPI_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer.
*  NOR_SPI_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer, modified when exist data to
*  sending and FIFO Not Full.
*  NOR_SPI_txBuffer[NOR_SPI_TX_BUFFER_SIZE] - used to store
*  data to sending.
*  All described above Global variables are used when Software Buffer is used.
*
*******************************************************************************/
CY_ISR(NOR_SPI_TX_ISR)
{
    #if(NOR_SPI_TX_SOFTWARE_BUF_ENABLED)
        uint8 tmpStatus;
    #endif /* (NOR_SPI_TX_SOFTWARE_BUF_ENABLED) */

    #ifdef NOR_SPI_TX_ISR_ENTRY_CALLBACK
        NOR_SPI_TX_ISR_EntryCallback();
    #endif /* NOR_SPI_TX_ISR_ENTRY_CALLBACK */

    /* User code required at start of ISR */
    /* `#START NOR_SPI_TX_ISR_START` */

    /* `#END` */
    
    #if(NOR_SPI_TX_SOFTWARE_BUF_ENABLED)
        /* Check if TX data buffer is not empty and there is space in TX FIFO */
        while(NOR_SPI_txBufferRead != NOR_SPI_txBufferWrite)
        {
            tmpStatus = NOR_SPI_GET_STATUS_TX(NOR_SPI_swStatusTx);
            NOR_SPI_swStatusTx = tmpStatus;

            if(0u != (NOR_SPI_swStatusTx & NOR_SPI_STS_TX_FIFO_NOT_FULL))
            {
                if(0u == NOR_SPI_txBufferFull)
                {
                   NOR_SPI_txBufferRead++;

                    if(NOR_SPI_txBufferRead >= NOR_SPI_TX_BUFFER_SIZE)
                    {
                        NOR_SPI_txBufferRead = 0u;
                    }
                }
                else
                {
                    NOR_SPI_txBufferFull = 0u;
                }

                /* Put data element into the TX FIFO */
                CY_SET_REG8(NOR_SPI_TXDATA_PTR, 
                                             NOR_SPI_txBuffer[NOR_SPI_txBufferRead]);
            }
            else
            {
                break;
            }
        }

        if(NOR_SPI_txBufferRead == NOR_SPI_txBufferWrite)
        {
            /* TX Buffer is EMPTY: disable interrupt on TX NOT FULL */
            NOR_SPI_TX_STATUS_MASK_REG &= ((uint8) ~NOR_SPI_STS_TX_FIFO_NOT_FULL);
        }

    #endif /* (NOR_SPI_TX_SOFTWARE_BUF_ENABLED) */

    /* User code required at end of ISR (Optional) */
    /* `#START NOR_SPI_TX_ISR_END` */

    /* `#END` */
    
    #ifdef NOR_SPI_TX_ISR_EXIT_CALLBACK
        NOR_SPI_TX_ISR_ExitCallback();
    #endif /* NOR_SPI_TX_ISR_EXIT_CALLBACK */
}


/*******************************************************************************
* Function Name: NOR_SPI_RX_ISR
********************************************************************************
*
* Summary:
*  Interrupt Service Routine for RX portion of the SPI Master.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  NOR_SPI_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer modified when FIFO contains
*  new data.
*  NOR_SPI_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer, modified when overflow occurred.
*  NOR_SPI_rxBuffer[NOR_SPI_RX_BUFFER_SIZE] - used to store
*  received data, modified when FIFO contains new data.
*  All described above Global variables are used when Software Buffer is used.
*
*******************************************************************************/
CY_ISR(NOR_SPI_RX_ISR)
{
    #if(NOR_SPI_RX_SOFTWARE_BUF_ENABLED)
        uint8 tmpStatus;
        uint8 rxData;
    #endif /* (NOR_SPI_RX_SOFTWARE_BUF_ENABLED) */

    #ifdef NOR_SPI_RX_ISR_ENTRY_CALLBACK
        NOR_SPI_RX_ISR_EntryCallback();
    #endif /* NOR_SPI_RX_ISR_ENTRY_CALLBACK */

    /* User code required at start of ISR */
    /* `#START NOR_SPI_RX_ISR_START` */

    /* `#END` */
    
    #if(NOR_SPI_RX_SOFTWARE_BUF_ENABLED)

        tmpStatus = NOR_SPI_GET_STATUS_RX(NOR_SPI_swStatusRx);
        NOR_SPI_swStatusRx = tmpStatus;

        /* Check if RX data FIFO has some data to be moved into the RX Buffer */
        while(0u != (NOR_SPI_swStatusRx & NOR_SPI_STS_RX_FIFO_NOT_EMPTY))
        {
            rxData = CY_GET_REG8(NOR_SPI_RXDATA_PTR);

            /* Set next pointer. */
            NOR_SPI_rxBufferWrite++;
            if(NOR_SPI_rxBufferWrite >= NOR_SPI_RX_BUFFER_SIZE)
            {
                NOR_SPI_rxBufferWrite = 0u;
            }

            if(NOR_SPI_rxBufferWrite == NOR_SPI_rxBufferRead)
            {
                NOR_SPI_rxBufferRead++;
                if(NOR_SPI_rxBufferRead >= NOR_SPI_RX_BUFFER_SIZE)
                {
                    NOR_SPI_rxBufferRead = 0u;
                }

                NOR_SPI_rxBufferFull = 1u;
            }

            /* Move data from the FIFO to the Buffer */
            NOR_SPI_rxBuffer[NOR_SPI_rxBufferWrite] = rxData;

            tmpStatus = NOR_SPI_GET_STATUS_RX(NOR_SPI_swStatusRx);
            NOR_SPI_swStatusRx = tmpStatus;
        }

    #endif /* (NOR_SPI_RX_SOFTWARE_BUF_ENABLED) */

    /* User code required at end of ISR (Optional) */
    /* `#START NOR_SPI_RX_ISR_END` */

    /* `#END` */
    
    #ifdef NOR_SPI_RX_ISR_EXIT_CALLBACK
        NOR_SPI_RX_ISR_ExitCallback();
    #endif /* NOR_SPI_RX_ISR_EXIT_CALLBACK */
}

/* [] END OF FILE */
