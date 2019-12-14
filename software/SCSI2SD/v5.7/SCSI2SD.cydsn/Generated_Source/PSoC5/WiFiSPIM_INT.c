/*******************************************************************************
* File Name: WiFiSPIM_INT.c
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

#include "WiFiSPIM_PVT.h"


/* User code required at start of ISR */
/* `#START WiFiSPIM_ISR_START_DEF` */

/* `#END` */


/*******************************************************************************
* Function Name: WiFiSPIM_TX_ISR
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
*  WiFiSPIM_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer.
*  WiFiSPIM_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer, modified when exist data to
*  sending and FIFO Not Full.
*  WiFiSPIM_txBuffer[WiFiSPIM_TX_BUFFER_SIZE] - used to store
*  data to sending.
*  All described above Global variables are used when Software Buffer is used.
*
*******************************************************************************/
CY_ISR(WiFiSPIM_TX_ISR)
{
    #if(WiFiSPIM_TX_SOFTWARE_BUF_ENABLED)
        uint8 tmpStatus;
    #endif /* (WiFiSPIM_TX_SOFTWARE_BUF_ENABLED) */

    #ifdef WiFiSPIM_TX_ISR_ENTRY_CALLBACK
        WiFiSPIM_TX_ISR_EntryCallback();
    #endif /* WiFiSPIM_TX_ISR_ENTRY_CALLBACK */

    /* User code required at start of ISR */
    /* `#START WiFiSPIM_TX_ISR_START` */

    /* `#END` */
    
    #if(WiFiSPIM_TX_SOFTWARE_BUF_ENABLED)
        /* Check if TX data buffer is not empty and there is space in TX FIFO */
        while(WiFiSPIM_txBufferRead != WiFiSPIM_txBufferWrite)
        {
            tmpStatus = WiFiSPIM_GET_STATUS_TX(WiFiSPIM_swStatusTx);
            WiFiSPIM_swStatusTx = tmpStatus;

            if(0u != (WiFiSPIM_swStatusTx & WiFiSPIM_STS_TX_FIFO_NOT_FULL))
            {
                if(0u == WiFiSPIM_txBufferFull)
                {
                   WiFiSPIM_txBufferRead++;

                    if(WiFiSPIM_txBufferRead >= WiFiSPIM_TX_BUFFER_SIZE)
                    {
                        WiFiSPIM_txBufferRead = 0u;
                    }
                }
                else
                {
                    WiFiSPIM_txBufferFull = 0u;
                }

                /* Put data element into the TX FIFO */
                CY_SET_REG8(WiFiSPIM_TXDATA_PTR, 
                                             WiFiSPIM_txBuffer[WiFiSPIM_txBufferRead]);
            }
            else
            {
                break;
            }
        }

        if(WiFiSPIM_txBufferRead == WiFiSPIM_txBufferWrite)
        {
            /* TX Buffer is EMPTY: disable interrupt on TX NOT FULL */
            WiFiSPIM_TX_STATUS_MASK_REG &= ((uint8) ~WiFiSPIM_STS_TX_FIFO_NOT_FULL);
        }

    #endif /* (WiFiSPIM_TX_SOFTWARE_BUF_ENABLED) */

    /* User code required at end of ISR (Optional) */
    /* `#START WiFiSPIM_TX_ISR_END` */

    /* `#END` */
    
    #ifdef WiFiSPIM_TX_ISR_EXIT_CALLBACK
        WiFiSPIM_TX_ISR_ExitCallback();
    #endif /* WiFiSPIM_TX_ISR_EXIT_CALLBACK */
}


/*******************************************************************************
* Function Name: WiFiSPIM_RX_ISR
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
*  WiFiSPIM_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer modified when FIFO contains
*  new data.
*  WiFiSPIM_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer, modified when overflow occurred.
*  WiFiSPIM_rxBuffer[WiFiSPIM_RX_BUFFER_SIZE] - used to store
*  received data, modified when FIFO contains new data.
*  All described above Global variables are used when Software Buffer is used.
*
*******************************************************************************/
CY_ISR(WiFiSPIM_RX_ISR)
{
    #if(WiFiSPIM_RX_SOFTWARE_BUF_ENABLED)
        uint8 tmpStatus;
        uint8 rxData;
    #endif /* (WiFiSPIM_RX_SOFTWARE_BUF_ENABLED) */

    #ifdef WiFiSPIM_RX_ISR_ENTRY_CALLBACK
        WiFiSPIM_RX_ISR_EntryCallback();
    #endif /* WiFiSPIM_RX_ISR_ENTRY_CALLBACK */

    /* User code required at start of ISR */
    /* `#START WiFiSPIM_RX_ISR_START` */

    /* `#END` */
    
    #if(WiFiSPIM_RX_SOFTWARE_BUF_ENABLED)

        tmpStatus = WiFiSPIM_GET_STATUS_RX(WiFiSPIM_swStatusRx);
        WiFiSPIM_swStatusRx = tmpStatus;

        /* Check if RX data FIFO has some data to be moved into the RX Buffer */
        while(0u != (WiFiSPIM_swStatusRx & WiFiSPIM_STS_RX_FIFO_NOT_EMPTY))
        {
            rxData = CY_GET_REG8(WiFiSPIM_RXDATA_PTR);

            /* Set next pointer. */
            WiFiSPIM_rxBufferWrite++;
            if(WiFiSPIM_rxBufferWrite >= WiFiSPIM_RX_BUFFER_SIZE)
            {
                WiFiSPIM_rxBufferWrite = 0u;
            }

            if(WiFiSPIM_rxBufferWrite == WiFiSPIM_rxBufferRead)
            {
                WiFiSPIM_rxBufferRead++;
                if(WiFiSPIM_rxBufferRead >= WiFiSPIM_RX_BUFFER_SIZE)
                {
                    WiFiSPIM_rxBufferRead = 0u;
                }

                WiFiSPIM_rxBufferFull = 1u;
            }

            /* Move data from the FIFO to the Buffer */
            WiFiSPIM_rxBuffer[WiFiSPIM_rxBufferWrite] = rxData;

            tmpStatus = WiFiSPIM_GET_STATUS_RX(WiFiSPIM_swStatusRx);
            WiFiSPIM_swStatusRx = tmpStatus;
        }

    #endif /* (WiFiSPIM_RX_SOFTWARE_BUF_ENABLED) */

    /* User code required at end of ISR (Optional) */
    /* `#START WiFiSPIM_RX_ISR_END` */

    /* `#END` */
    
    #ifdef WiFiSPIM_RX_ISR_EXIT_CALLBACK
        WiFiSPIM_RX_ISR_ExitCallback();
    #endif /* WiFiSPIM_RX_ISR_EXIT_CALLBACK */
}

/* [] END OF FILE */
