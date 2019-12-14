/*******************************************************************************
* File Name: WiFiSPIM.c
* Version 2.50
*
* Description:
*  This file provides all API functionality of the SPI Master component.
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

#if(WiFiSPIM_TX_SOFTWARE_BUF_ENABLED)
    volatile uint8 WiFiSPIM_txBuffer[WiFiSPIM_TX_BUFFER_SIZE];
    volatile uint8 WiFiSPIM_txBufferFull;
    volatile uint8 WiFiSPIM_txBufferRead;
    volatile uint8 WiFiSPIM_txBufferWrite;
#endif /* (WiFiSPIM_TX_SOFTWARE_BUF_ENABLED) */

#if(WiFiSPIM_RX_SOFTWARE_BUF_ENABLED)
    volatile uint8 WiFiSPIM_rxBuffer[WiFiSPIM_RX_BUFFER_SIZE];
    volatile uint8 WiFiSPIM_rxBufferFull;
    volatile uint8 WiFiSPIM_rxBufferRead;
    volatile uint8 WiFiSPIM_rxBufferWrite;
#endif /* (WiFiSPIM_RX_SOFTWARE_BUF_ENABLED) */

uint8 WiFiSPIM_initVar = 0u;

volatile uint8 WiFiSPIM_swStatusTx;
volatile uint8 WiFiSPIM_swStatusRx;


/*******************************************************************************
* Function Name: WiFiSPIM_Init
********************************************************************************
*
* Summary:
*  Inits/Restores default SPIM configuration provided with customizer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  When this function is called it initializes all of the necessary parameters
*  for execution. i.e. setting the initial interrupt mask, configuring the
*  interrupt service routine, configuring the bit-counter parameters and
*  clearing the FIFO and Status Register.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void WiFiSPIM_Init(void) 
{
    /* Initialize the Bit counter */
    WiFiSPIM_COUNTER_PERIOD_REG = WiFiSPIM_BITCTR_INIT;

    /* Init TX ISR  */
    #if(0u != WiFiSPIM_INTERNAL_TX_INT_ENABLED)
        CyIntDisable         (WiFiSPIM_TX_ISR_NUMBER);
        CyIntSetPriority     (WiFiSPIM_TX_ISR_NUMBER,  WiFiSPIM_TX_ISR_PRIORITY);
        (void) CyIntSetVector(WiFiSPIM_TX_ISR_NUMBER, &WiFiSPIM_TX_ISR);
    #endif /* (0u != WiFiSPIM_INTERNAL_TX_INT_ENABLED) */

    /* Init RX ISR  */
    #if(0u != WiFiSPIM_INTERNAL_RX_INT_ENABLED)
        CyIntDisable         (WiFiSPIM_RX_ISR_NUMBER);
        CyIntSetPriority     (WiFiSPIM_RX_ISR_NUMBER,  WiFiSPIM_RX_ISR_PRIORITY);
        (void) CyIntSetVector(WiFiSPIM_RX_ISR_NUMBER, &WiFiSPIM_RX_ISR);
    #endif /* (0u != WiFiSPIM_INTERNAL_RX_INT_ENABLED) */

    /* Clear any stray data from the RX and TX FIFO */
    WiFiSPIM_ClearFIFO();

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

    (void) WiFiSPIM_ReadTxStatus(); /* Clear Tx status and swStatusTx */
    (void) WiFiSPIM_ReadRxStatus(); /* Clear Rx status and swStatusRx */

    /* Configure TX and RX interrupt mask */
    WiFiSPIM_TX_STATUS_MASK_REG = WiFiSPIM_TX_INIT_INTERRUPTS_MASK;
    WiFiSPIM_RX_STATUS_MASK_REG = WiFiSPIM_RX_INIT_INTERRUPTS_MASK;
}


/*******************************************************************************
* Function Name: WiFiSPIM_Enable
********************************************************************************
*
* Summary:
*  Enable SPIM component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void WiFiSPIM_Enable(void) 
{
    uint8 enableInterrupts;

    enableInterrupts = CyEnterCriticalSection();
    WiFiSPIM_COUNTER_CONTROL_REG |= WiFiSPIM_CNTR_ENABLE;
    WiFiSPIM_TX_STATUS_ACTL_REG  |= WiFiSPIM_INT_ENABLE;
    WiFiSPIM_RX_STATUS_ACTL_REG  |= WiFiSPIM_INT_ENABLE;
    CyExitCriticalSection(enableInterrupts);

    #if(0u != WiFiSPIM_INTERNAL_CLOCK)
        WiFiSPIM_IntClock_Enable();
    #endif /* (0u != WiFiSPIM_INTERNAL_CLOCK) */

    WiFiSPIM_EnableTxInt();
    WiFiSPIM_EnableRxInt();
}


/*******************************************************************************
* Function Name: WiFiSPIM_Start
********************************************************************************
*
* Summary:
*  Initialize and Enable the SPI Master component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  WiFiSPIM_initVar - used to check initial configuration, modified on
*  first function call.
*
* Theory:
*  Enable the clock input to enable operation.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void WiFiSPIM_Start(void) 
{
    if(0u == WiFiSPIM_initVar)
    {
        WiFiSPIM_Init();
        WiFiSPIM_initVar = 1u;
    }

    WiFiSPIM_Enable();
}


/*******************************************************************************
* Function Name: WiFiSPIM_Stop
********************************************************************************
*
* Summary:
*  Disable the SPI Master component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Disable the clock input to enable operation.
*
*******************************************************************************/
void WiFiSPIM_Stop(void) 
{
    uint8 enableInterrupts;

    enableInterrupts = CyEnterCriticalSection();
    WiFiSPIM_TX_STATUS_ACTL_REG &= ((uint8) ~WiFiSPIM_INT_ENABLE);
    WiFiSPIM_RX_STATUS_ACTL_REG &= ((uint8) ~WiFiSPIM_INT_ENABLE);
    CyExitCriticalSection(enableInterrupts);

    #if(0u != WiFiSPIM_INTERNAL_CLOCK)
        WiFiSPIM_IntClock_Disable();
    #endif /* (0u != WiFiSPIM_INTERNAL_CLOCK) */

    WiFiSPIM_DisableTxInt();
    WiFiSPIM_DisableRxInt();
}


/*******************************************************************************
* Function Name: WiFiSPIM_EnableTxInt
********************************************************************************
*
* Summary:
*  Enable internal Tx interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Enable the internal Tx interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void WiFiSPIM_EnableTxInt(void) 
{
    #if(0u != WiFiSPIM_INTERNAL_TX_INT_ENABLED)
        CyIntEnable(WiFiSPIM_TX_ISR_NUMBER);
    #endif /* (0u != WiFiSPIM_INTERNAL_TX_INT_ENABLED) */
}


/*******************************************************************************
* Function Name: WiFiSPIM_EnableRxInt
********************************************************************************
*
* Summary:
*  Enable internal Rx interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Enable the internal Rx interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void WiFiSPIM_EnableRxInt(void) 
{
    #if(0u != WiFiSPIM_INTERNAL_RX_INT_ENABLED)
        CyIntEnable(WiFiSPIM_RX_ISR_NUMBER);
    #endif /* (0u != WiFiSPIM_INTERNAL_RX_INT_ENABLED) */
}


/*******************************************************************************
* Function Name: WiFiSPIM_DisableTxInt
********************************************************************************
*
* Summary:
*  Disable internal Tx interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Disable the internal Tx interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void WiFiSPIM_DisableTxInt(void) 
{
    #if(0u != WiFiSPIM_INTERNAL_TX_INT_ENABLED)
        CyIntDisable(WiFiSPIM_TX_ISR_NUMBER);
    #endif /* (0u != WiFiSPIM_INTERNAL_TX_INT_ENABLED) */
}


/*******************************************************************************
* Function Name: WiFiSPIM_DisableRxInt
********************************************************************************
*
* Summary:
*  Disable internal Rx interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Disable the internal Rx interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void WiFiSPIM_DisableRxInt(void) 
{
    #if(0u != WiFiSPIM_INTERNAL_RX_INT_ENABLED)
        CyIntDisable(WiFiSPIM_RX_ISR_NUMBER);
    #endif /* (0u != WiFiSPIM_INTERNAL_RX_INT_ENABLED) */
}


/*******************************************************************************
* Function Name: WiFiSPIM_SetTxInterruptMode
********************************************************************************
*
* Summary:
*  Configure which status bits trigger an interrupt event.
*
* Parameters:
*  intSrc: An or'd combination of the desired status bit masks (defined in the
*  header file).
*
* Return:
*  None.
*
* Theory:
*  Enables the output of specific status bits to the interrupt controller.
*
*******************************************************************************/
void WiFiSPIM_SetTxInterruptMode(uint8 intSrc) 
{
    WiFiSPIM_TX_STATUS_MASK_REG = intSrc;
}


/*******************************************************************************
* Function Name: WiFiSPIM_SetRxInterruptMode
********************************************************************************
*
* Summary:
*  Configure which status bits trigger an interrupt event.
*
* Parameters:
*  intSrc: An or'd combination of the desired status bit masks (defined in the
*  header file).
*
* Return:
*  None.
*
* Theory:
*  Enables the output of specific status bits to the interrupt controller.
*
*******************************************************************************/
void WiFiSPIM_SetRxInterruptMode(uint8 intSrc) 
{
    WiFiSPIM_RX_STATUS_MASK_REG  = intSrc;
}


/*******************************************************************************
* Function Name: WiFiSPIM_ReadTxStatus
********************************************************************************
*
* Summary:
*  Read the Tx status register for the component.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the Tx status register.
*
* Global variables:
*  WiFiSPIM_swStatusTx - used to store in software status register,
*  modified every function call - resets to zero.
*
* Theory:
*  Allows the user and the API to read the Tx status register for error
*  detection and flow control.
*
* Side Effects:
*  Clear Tx status register of the component.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 WiFiSPIM_ReadTxStatus(void) 
{
    uint8 tmpStatus;

    #if(WiFiSPIM_TX_SOFTWARE_BUF_ENABLED)
        /* Disable TX interrupt to protect global veriables */
        WiFiSPIM_DisableTxInt();

        tmpStatus = WiFiSPIM_GET_STATUS_TX(WiFiSPIM_swStatusTx);
        WiFiSPIM_swStatusTx = 0u;

        WiFiSPIM_EnableTxInt();

    #else

        tmpStatus = WiFiSPIM_TX_STATUS_REG;

    #endif /* (WiFiSPIM_TX_SOFTWARE_BUF_ENABLED) */

    return(tmpStatus);
}


/*******************************************************************************
* Function Name: WiFiSPIM_ReadRxStatus
********************************************************************************
*
* Summary:
*  Read the Rx status register for the component.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the Rx status register.
*
* Global variables:
*  WiFiSPIM_swStatusRx - used to store in software Rx status register,
*  modified every function call - resets to zero.
*
* Theory:
*  Allows the user and the API to read the Rx status register for error
*  detection and flow control.
*
* Side Effects:
*  Clear Rx status register of the component.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 WiFiSPIM_ReadRxStatus(void) 
{
    uint8 tmpStatus;

    #if(WiFiSPIM_RX_SOFTWARE_BUF_ENABLED)
        /* Disable RX interrupt to protect global veriables */
        WiFiSPIM_DisableRxInt();

        tmpStatus = WiFiSPIM_GET_STATUS_RX(WiFiSPIM_swStatusRx);
        WiFiSPIM_swStatusRx = 0u;

        WiFiSPIM_EnableRxInt();

    #else

        tmpStatus = WiFiSPIM_RX_STATUS_REG;

    #endif /* (WiFiSPIM_RX_SOFTWARE_BUF_ENABLED) */

    return(tmpStatus);
}


/*******************************************************************************
* Function Name: WiFiSPIM_WriteTxData
********************************************************************************
*
* Summary:
*  Write a byte of data to be sent across the SPI.
*
* Parameters:
*  txDataByte: The data value to send across the SPI.
*
* Return:
*  None.
*
* Global variables:
*  WiFiSPIM_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer, modified every function
*  call if TX Software Buffer is used.
*  WiFiSPIM_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer.
*  WiFiSPIM_txBuffer[WiFiSPIM_TX_BUFFER_SIZE] - used to store
*  data to sending, modified every function call if TX Software Buffer is used.
*
* Theory:
*  Allows the user to transmit any byte of data in a single transfer.
*
* Side Effects:
*  If this function is called again before the previous byte is finished then
*  the next byte will be appended to the transfer with no time between
*  the byte transfers. Clear Tx status register of the component.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void WiFiSPIM_WriteTxData(uint8 txData) 
{
    #if(WiFiSPIM_TX_SOFTWARE_BUF_ENABLED)

        uint8 tempStatus;
        uint8 tmpTxBufferRead;

        /* Block if TX buffer is FULL: don't overwrite */
        do
        {
            tmpTxBufferRead = WiFiSPIM_txBufferRead;
            if(0u == tmpTxBufferRead)
            {
                tmpTxBufferRead = (WiFiSPIM_TX_BUFFER_SIZE - 1u);
            }
            else
            {
                tmpTxBufferRead--;
            }

        }while(tmpTxBufferRead == WiFiSPIM_txBufferWrite);

        /* Disable TX interrupt to protect global veriables */
        WiFiSPIM_DisableTxInt();

        tempStatus = WiFiSPIM_GET_STATUS_TX(WiFiSPIM_swStatusTx);
        WiFiSPIM_swStatusTx = tempStatus;


        if((WiFiSPIM_txBufferRead == WiFiSPIM_txBufferWrite) &&
           (0u != (WiFiSPIM_swStatusTx & WiFiSPIM_STS_TX_FIFO_NOT_FULL)))
        {
            /* Put data element into the TX FIFO */
            CY_SET_REG8(WiFiSPIM_TXDATA_PTR, txData);
        }
        else
        {
            /* Add to the TX software buffer */
            WiFiSPIM_txBufferWrite++;
            if(WiFiSPIM_txBufferWrite >= WiFiSPIM_TX_BUFFER_SIZE)
            {
                WiFiSPIM_txBufferWrite = 0u;
            }

            if(WiFiSPIM_txBufferWrite == WiFiSPIM_txBufferRead)
            {
                WiFiSPIM_txBufferRead++;
                if(WiFiSPIM_txBufferRead >= WiFiSPIM_TX_BUFFER_SIZE)
                {
                    WiFiSPIM_txBufferRead = 0u;
                }
                WiFiSPIM_txBufferFull = 1u;
            }

            WiFiSPIM_txBuffer[WiFiSPIM_txBufferWrite] = txData;

            WiFiSPIM_TX_STATUS_MASK_REG |= WiFiSPIM_STS_TX_FIFO_NOT_FULL;
        }

        WiFiSPIM_EnableTxInt();

    #else
        /* Wait until TX FIFO has a place */
        while(0u == (WiFiSPIM_TX_STATUS_REG & WiFiSPIM_STS_TX_FIFO_NOT_FULL))
        {
        }

        /* Put data element into the TX FIFO */
        CY_SET_REG8(WiFiSPIM_TXDATA_PTR, txData);

    #endif /* (WiFiSPIM_TX_SOFTWARE_BUF_ENABLED) */
}


/*******************************************************************************
* Function Name: WiFiSPIM_ReadRxData
********************************************************************************
*
* Summary:
*  Read the next byte of data received across the SPI.
*
* Parameters:
*  None.
*
* Return:
*  The next byte of data read from the FIFO.
*
* Global variables:
*  WiFiSPIM_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer.
*  WiFiSPIM_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer, modified every function
*  call if RX Software Buffer is used.
*  WiFiSPIM_rxBuffer[WiFiSPIM_RX_BUFFER_SIZE] - used to store
*  received data.
*
* Theory:
*  Allows the user to read a byte of data received.
*
* Side Effects:
*  Will return invalid data if the FIFO is empty. The user should Call
*  GetRxBufferSize() and if it returns a non-zero value then it is safe to call
*  ReadByte() function.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 WiFiSPIM_ReadRxData(void) 
{
    uint8 rxData;

    #if(WiFiSPIM_RX_SOFTWARE_BUF_ENABLED)

        /* Disable RX interrupt to protect global veriables */
        WiFiSPIM_DisableRxInt();

        if(WiFiSPIM_rxBufferRead != WiFiSPIM_rxBufferWrite)
        {
            if(0u == WiFiSPIM_rxBufferFull)
            {
                WiFiSPIM_rxBufferRead++;
                if(WiFiSPIM_rxBufferRead >= WiFiSPIM_RX_BUFFER_SIZE)
                {
                    WiFiSPIM_rxBufferRead = 0u;
                }
            }
            else
            {
                WiFiSPIM_rxBufferFull = 0u;
            }
        }

        rxData = WiFiSPIM_rxBuffer[WiFiSPIM_rxBufferRead];

        WiFiSPIM_EnableRxInt();

    #else

        rxData = CY_GET_REG8(WiFiSPIM_RXDATA_PTR);

    #endif /* (WiFiSPIM_RX_SOFTWARE_BUF_ENABLED) */

    return(rxData);
}


/*******************************************************************************
* Function Name: WiFiSPIM_GetRxBufferSize
********************************************************************************
*
* Summary:
*  Returns the number of bytes/words of data currently held in the RX buffer.
*  If RX Software Buffer not used then function return 0 if FIFO empty or 1 if
*  FIFO not empty. In another case function return size of RX Software Buffer.
*
* Parameters:
*  None.
*
* Return:
*  Integer count of the number of bytes/words in the RX buffer.
*
* Global variables:
*  WiFiSPIM_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer.
*  WiFiSPIM_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer.
*
* Side Effects:
*  Clear status register of the component.
*
*******************************************************************************/
uint8 WiFiSPIM_GetRxBufferSize(void) 
{
    uint8 size;

    #if(WiFiSPIM_RX_SOFTWARE_BUF_ENABLED)

        /* Disable RX interrupt to protect global veriables */
        WiFiSPIM_DisableRxInt();

        if(WiFiSPIM_rxBufferRead == WiFiSPIM_rxBufferWrite)
        {
            size = 0u;
        }
        else if(WiFiSPIM_rxBufferRead < WiFiSPIM_rxBufferWrite)
        {
            size = (WiFiSPIM_rxBufferWrite - WiFiSPIM_rxBufferRead);
        }
        else
        {
            size = (WiFiSPIM_RX_BUFFER_SIZE - WiFiSPIM_rxBufferRead) + WiFiSPIM_rxBufferWrite;
        }

        WiFiSPIM_EnableRxInt();

    #else

        /* We can only know if there is data in the RX FIFO */
        size = (0u != (WiFiSPIM_RX_STATUS_REG & WiFiSPIM_STS_RX_FIFO_NOT_EMPTY)) ? 1u : 0u;

    #endif /* (WiFiSPIM_TX_SOFTWARE_BUF_ENABLED) */

    return(size);
}


/*******************************************************************************
* Function Name: WiFiSPIM_GetTxBufferSize
********************************************************************************
*
* Summary:
*  Returns the number of bytes/words of data currently held in the TX buffer.
*  If TX Software Buffer not used then function return 0 - if FIFO empty, 1 - if
*  FIFO not full, 4 - if FIFO full. In another case function return size of TX
*  Software Buffer.
*
* Parameters:
*  None.
*
* Return:
*  Integer count of the number of bytes/words in the TX buffer.
*
* Global variables:
*  WiFiSPIM_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer.
*  WiFiSPIM_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer.
*
* Side Effects:
*  Clear status register of the component.
*
*******************************************************************************/
uint8  WiFiSPIM_GetTxBufferSize(void) 
{
    uint8 size;

    #if(WiFiSPIM_TX_SOFTWARE_BUF_ENABLED)
        /* Disable TX interrupt to protect global veriables */
        WiFiSPIM_DisableTxInt();

        if(WiFiSPIM_txBufferRead == WiFiSPIM_txBufferWrite)
        {
            size = 0u;
        }
        else if(WiFiSPIM_txBufferRead < WiFiSPIM_txBufferWrite)
        {
            size = (WiFiSPIM_txBufferWrite - WiFiSPIM_txBufferRead);
        }
        else
        {
            size = (WiFiSPIM_TX_BUFFER_SIZE - WiFiSPIM_txBufferRead) + WiFiSPIM_txBufferWrite;
        }

        WiFiSPIM_EnableTxInt();

    #else

        size = WiFiSPIM_TX_STATUS_REG;

        if(0u != (size & WiFiSPIM_STS_TX_FIFO_EMPTY))
        {
            size = 0u;
        }
        else if(0u != (size & WiFiSPIM_STS_TX_FIFO_NOT_FULL))
        {
            size = 1u;
        }
        else
        {
            size = WiFiSPIM_FIFO_SIZE;
        }

    #endif /* (WiFiSPIM_TX_SOFTWARE_BUF_ENABLED) */

    return(size);
}


/*******************************************************************************
* Function Name: WiFiSPIM_ClearRxBuffer
********************************************************************************
*
* Summary:
*  Clear the RX RAM buffer by setting the read and write pointers both to zero.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  WiFiSPIM_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer, modified every function
*  call - resets to zero.
*  WiFiSPIM_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer, modified every function call -
*  resets to zero.
*
* Theory:
*  Setting the pointers to zero makes the system believe there is no data to
*  read and writing will resume at address 0 overwriting any data that may have
*  remained in the RAM.
*
* Side Effects:
*  Any received data not read from the RAM buffer will be lost when overwritten.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void WiFiSPIM_ClearRxBuffer(void) 
{
    /* Clear Hardware RX FIFO */
    while(0u !=(WiFiSPIM_RX_STATUS_REG & WiFiSPIM_STS_RX_FIFO_NOT_EMPTY))
    {
        (void) CY_GET_REG8(WiFiSPIM_RXDATA_PTR);
    }

    #if(WiFiSPIM_RX_SOFTWARE_BUF_ENABLED)
        /* Disable RX interrupt to protect global veriables */
        WiFiSPIM_DisableRxInt();

        WiFiSPIM_rxBufferFull  = 0u;
        WiFiSPIM_rxBufferRead  = 0u;
        WiFiSPIM_rxBufferWrite = 0u;

        WiFiSPIM_EnableRxInt();
    #endif /* (WiFiSPIM_RX_SOFTWARE_BUF_ENABLED) */
}


/*******************************************************************************
* Function Name: WiFiSPIM_ClearTxBuffer
********************************************************************************
*
* Summary:
*  Clear the TX RAM buffer by setting the read and write pointers both to zero.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  WiFiSPIM_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer, modified every function
*  call - resets to zero.
*  WiFiSPIM_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer, modified every function call -
*  resets to zero.
*
* Theory:
*  Setting the pointers to zero makes the system believe there is no data to
*  read and writing will resume at address 0 overwriting any data that may have
*  remained in the RAM.
*
* Side Effects:
*  Any data not yet transmitted from the RAM buffer will be lost when
*  overwritten.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void WiFiSPIM_ClearTxBuffer(void) 
{
    uint8 enableInterrupts;

    enableInterrupts = CyEnterCriticalSection();
    /* Clear TX FIFO */
    WiFiSPIM_AUX_CONTROL_DP0_REG |= ((uint8)  WiFiSPIM_TX_FIFO_CLR);
    WiFiSPIM_AUX_CONTROL_DP0_REG &= ((uint8) ~WiFiSPIM_TX_FIFO_CLR);

    #if(WiFiSPIM_USE_SECOND_DATAPATH)
        /* Clear TX FIFO for 2nd Datapath */
        WiFiSPIM_AUX_CONTROL_DP1_REG |= ((uint8)  WiFiSPIM_TX_FIFO_CLR);
        WiFiSPIM_AUX_CONTROL_DP1_REG &= ((uint8) ~WiFiSPIM_TX_FIFO_CLR);
    #endif /* (WiFiSPIM_USE_SECOND_DATAPATH) */
    CyExitCriticalSection(enableInterrupts);

    #if(WiFiSPIM_TX_SOFTWARE_BUF_ENABLED)
        /* Disable TX interrupt to protect global veriables */
        WiFiSPIM_DisableTxInt();

        WiFiSPIM_txBufferFull  = 0u;
        WiFiSPIM_txBufferRead  = 0u;
        WiFiSPIM_txBufferWrite = 0u;

        /* Buffer is EMPTY: disable TX FIFO NOT FULL interrupt */
        WiFiSPIM_TX_STATUS_MASK_REG &= ((uint8) ~WiFiSPIM_STS_TX_FIFO_NOT_FULL);

        WiFiSPIM_EnableTxInt();
    #endif /* (WiFiSPIM_TX_SOFTWARE_BUF_ENABLED) */
}


#if(0u != WiFiSPIM_BIDIRECTIONAL_MODE)
    /*******************************************************************************
    * Function Name: WiFiSPIM_TxEnable
    ********************************************************************************
    *
    * Summary:
    *  If the SPI master is configured to use a single bi-directional pin then this
    *  will set the bi-directional pin to transmit.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void WiFiSPIM_TxEnable(void) 
    {
        WiFiSPIM_CONTROL_REG |= WiFiSPIM_CTRL_TX_SIGNAL_EN;
    }


    /*******************************************************************************
    * Function Name: WiFiSPIM_TxDisable
    ********************************************************************************
    *
    * Summary:
    *  If the SPI master is configured to use a single bi-directional pin then this
    *  will set the bi-directional pin to receive.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void WiFiSPIM_TxDisable(void) 
    {
        WiFiSPIM_CONTROL_REG &= ((uint8) ~WiFiSPIM_CTRL_TX_SIGNAL_EN);
    }

#endif /* (0u != WiFiSPIM_BIDIRECTIONAL_MODE) */


/*******************************************************************************
* Function Name: WiFiSPIM_PutArray
********************************************************************************
*
* Summary:
*  Write available data from ROM/RAM to the TX buffer while space is available
*  in the TX buffer. Keep trying until all data is passed to the TX buffer.
*
* Parameters:
*  *buffer: Pointer to the location in RAM containing the data to send
*  byteCount: The number of bytes to move to the transmit buffer.
*
* Return:
*  None.
*
* Side Effects:
*  Will stay in this routine until all data has been sent.  May get locked in
*  this loop if data is not being initiated by the master if there is not
*  enough room in the TX FIFO.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void WiFiSPIM_PutArray(const uint8 buffer[], uint8 byteCount)
                                                                          
{
    uint8 bufIndex;

    bufIndex = 0u;

    while(byteCount > 0u)
    {
        WiFiSPIM_WriteTxData(buffer[bufIndex]);
        bufIndex++;
        byteCount--;
    }
}


/*******************************************************************************
* Function Name: WiFiSPIM_ClearFIFO
********************************************************************************
*
* Summary:
*  Clear the RX and TX FIFO's of all data for a fresh start.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  Clear status register of the component.
*
*******************************************************************************/
void WiFiSPIM_ClearFIFO(void) 
{
    uint8 enableInterrupts;

    /* Clear Hardware RX FIFO */
    while(0u !=(WiFiSPIM_RX_STATUS_REG & WiFiSPIM_STS_RX_FIFO_NOT_EMPTY))
    {
        (void) CY_GET_REG8(WiFiSPIM_RXDATA_PTR);
    }

    enableInterrupts = CyEnterCriticalSection();
    /* Clear TX FIFO */
    WiFiSPIM_AUX_CONTROL_DP0_REG |= ((uint8)  WiFiSPIM_TX_FIFO_CLR);
    WiFiSPIM_AUX_CONTROL_DP0_REG &= ((uint8) ~WiFiSPIM_TX_FIFO_CLR);

    #if(WiFiSPIM_USE_SECOND_DATAPATH)
        /* Clear TX FIFO for 2nd Datapath */
        WiFiSPIM_AUX_CONTROL_DP1_REG |= ((uint8)  WiFiSPIM_TX_FIFO_CLR);
        WiFiSPIM_AUX_CONTROL_DP1_REG &= ((uint8) ~WiFiSPIM_TX_FIFO_CLR);
    #endif /* (WiFiSPIM_USE_SECOND_DATAPATH) */
    CyExitCriticalSection(enableInterrupts);
}


/* Following functions are for version Compatibility, they are obsolete.
*  Please do not use it in new projects.
*/


/*******************************************************************************
* Function Name: WiFiSPIM_EnableInt
********************************************************************************
*
* Summary:
*  Enable internal interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Enable the internal interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void WiFiSPIM_EnableInt(void) 
{
    WiFiSPIM_EnableRxInt();
    WiFiSPIM_EnableTxInt();
}


/*******************************************************************************
* Function Name: WiFiSPIM_DisableInt
********************************************************************************
*
* Summary:
*  Disable internal interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Disable the internal interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void WiFiSPIM_DisableInt(void) 
{
    WiFiSPIM_DisableTxInt();
    WiFiSPIM_DisableRxInt();
}


/*******************************************************************************
* Function Name: WiFiSPIM_SetInterruptMode
********************************************************************************
*
* Summary:
*  Configure which status bits trigger an interrupt event.
*
* Parameters:
*  intSrc: An or'd combination of the desired status bit masks (defined in the
*  header file).
*
* Return:
*  None.
*
* Theory:
*  Enables the output of specific status bits to the interrupt controller.
*
*******************************************************************************/
void WiFiSPIM_SetInterruptMode(uint8 intSrc) 
{
    WiFiSPIM_TX_STATUS_MASK_REG  = (intSrc & ((uint8) ~WiFiSPIM_STS_SPI_IDLE));
    WiFiSPIM_RX_STATUS_MASK_REG  =  intSrc;
}


/*******************************************************************************
* Function Name: WiFiSPIM_ReadStatus
********************************************************************************
*
* Summary:
*  Read the status register for the component.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the status register.
*
* Global variables:
*  WiFiSPIM_swStatus - used to store in software status register,
*  modified every function call - resets to zero.
*
* Theory:
*  Allows the user and the API to read the status register for error detection
*  and flow control.
*
* Side Effects:
*  Clear status register of the component.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 WiFiSPIM_ReadStatus(void) 
{
    uint8 tmpStatus;

    #if(WiFiSPIM_TX_SOFTWARE_BUF_ENABLED || WiFiSPIM_RX_SOFTWARE_BUF_ENABLED)

        WiFiSPIM_DisableInt();

        tmpStatus  = WiFiSPIM_GET_STATUS_RX(WiFiSPIM_swStatusRx);
        tmpStatus |= WiFiSPIM_GET_STATUS_TX(WiFiSPIM_swStatusTx);
        tmpStatus &= ((uint8) ~WiFiSPIM_STS_SPI_IDLE);

        WiFiSPIM_swStatusTx = 0u;
        WiFiSPIM_swStatusRx = 0u;

        WiFiSPIM_EnableInt();

    #else

        tmpStatus  = WiFiSPIM_RX_STATUS_REG;
        tmpStatus |= WiFiSPIM_TX_STATUS_REG;
        tmpStatus &= ((uint8) ~WiFiSPIM_STS_SPI_IDLE);

    #endif /* (WiFiSPIM_TX_SOFTWARE_BUF_ENABLED || WiFiSPIM_RX_SOFTWARE_BUF_ENABLED) */

    return(tmpStatus);
}


/* [] END OF FILE */
