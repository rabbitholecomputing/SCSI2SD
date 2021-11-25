/*******************************************************************************
* File Name: NOR_SPI.c
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

#include "NOR_SPI_PVT.h"

#if(NOR_SPI_TX_SOFTWARE_BUF_ENABLED)
    volatile uint8 NOR_SPI_txBuffer[NOR_SPI_TX_BUFFER_SIZE];
    volatile uint8 NOR_SPI_txBufferFull;
    volatile uint8 NOR_SPI_txBufferRead;
    volatile uint8 NOR_SPI_txBufferWrite;
#endif /* (NOR_SPI_TX_SOFTWARE_BUF_ENABLED) */

#if(NOR_SPI_RX_SOFTWARE_BUF_ENABLED)
    volatile uint8 NOR_SPI_rxBuffer[NOR_SPI_RX_BUFFER_SIZE];
    volatile uint8 NOR_SPI_rxBufferFull;
    volatile uint8 NOR_SPI_rxBufferRead;
    volatile uint8 NOR_SPI_rxBufferWrite;
#endif /* (NOR_SPI_RX_SOFTWARE_BUF_ENABLED) */

uint8 NOR_SPI_initVar = 0u;

volatile uint8 NOR_SPI_swStatusTx;
volatile uint8 NOR_SPI_swStatusRx;


/*******************************************************************************
* Function Name: NOR_SPI_Init
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
void NOR_SPI_Init(void) 
{
    /* Initialize the Bit counter */
    NOR_SPI_COUNTER_PERIOD_REG = NOR_SPI_BITCTR_INIT;

    /* Init TX ISR  */
    #if(0u != NOR_SPI_INTERNAL_TX_INT_ENABLED)
        CyIntDisable         (NOR_SPI_TX_ISR_NUMBER);
        CyIntSetPriority     (NOR_SPI_TX_ISR_NUMBER,  NOR_SPI_TX_ISR_PRIORITY);
        (void) CyIntSetVector(NOR_SPI_TX_ISR_NUMBER, &NOR_SPI_TX_ISR);
    #endif /* (0u != NOR_SPI_INTERNAL_TX_INT_ENABLED) */

    /* Init RX ISR  */
    #if(0u != NOR_SPI_INTERNAL_RX_INT_ENABLED)
        CyIntDisable         (NOR_SPI_RX_ISR_NUMBER);
        CyIntSetPriority     (NOR_SPI_RX_ISR_NUMBER,  NOR_SPI_RX_ISR_PRIORITY);
        (void) CyIntSetVector(NOR_SPI_RX_ISR_NUMBER, &NOR_SPI_RX_ISR);
    #endif /* (0u != NOR_SPI_INTERNAL_RX_INT_ENABLED) */

    /* Clear any stray data from the RX and TX FIFO */
    NOR_SPI_ClearFIFO();

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

    (void) NOR_SPI_ReadTxStatus(); /* Clear Tx status and swStatusTx */
    (void) NOR_SPI_ReadRxStatus(); /* Clear Rx status and swStatusRx */

    /* Configure TX and RX interrupt mask */
    NOR_SPI_TX_STATUS_MASK_REG = NOR_SPI_TX_INIT_INTERRUPTS_MASK;
    NOR_SPI_RX_STATUS_MASK_REG = NOR_SPI_RX_INIT_INTERRUPTS_MASK;
}


/*******************************************************************************
* Function Name: NOR_SPI_Enable
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
void NOR_SPI_Enable(void) 
{
    uint8 enableInterrupts;

    enableInterrupts = CyEnterCriticalSection();
    NOR_SPI_COUNTER_CONTROL_REG |= NOR_SPI_CNTR_ENABLE;
    NOR_SPI_TX_STATUS_ACTL_REG  |= NOR_SPI_INT_ENABLE;
    NOR_SPI_RX_STATUS_ACTL_REG  |= NOR_SPI_INT_ENABLE;
    CyExitCriticalSection(enableInterrupts);

    #if(0u != NOR_SPI_INTERNAL_CLOCK)
        NOR_SPI_IntClock_Enable();
    #endif /* (0u != NOR_SPI_INTERNAL_CLOCK) */

    NOR_SPI_EnableTxInt();
    NOR_SPI_EnableRxInt();
}


/*******************************************************************************
* Function Name: NOR_SPI_Start
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
*  NOR_SPI_initVar - used to check initial configuration, modified on
*  first function call.
*
* Theory:
*  Enable the clock input to enable operation.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void NOR_SPI_Start(void) 
{
    if(0u == NOR_SPI_initVar)
    {
        NOR_SPI_Init();
        NOR_SPI_initVar = 1u;
    }

    NOR_SPI_Enable();
}


/*******************************************************************************
* Function Name: NOR_SPI_Stop
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
void NOR_SPI_Stop(void) 
{
    uint8 enableInterrupts;

    enableInterrupts = CyEnterCriticalSection();
    NOR_SPI_TX_STATUS_ACTL_REG &= ((uint8) ~NOR_SPI_INT_ENABLE);
    NOR_SPI_RX_STATUS_ACTL_REG &= ((uint8) ~NOR_SPI_INT_ENABLE);
    CyExitCriticalSection(enableInterrupts);

    #if(0u != NOR_SPI_INTERNAL_CLOCK)
        NOR_SPI_IntClock_Disable();
    #endif /* (0u != NOR_SPI_INTERNAL_CLOCK) */

    NOR_SPI_DisableTxInt();
    NOR_SPI_DisableRxInt();
}


/*******************************************************************************
* Function Name: NOR_SPI_EnableTxInt
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
void NOR_SPI_EnableTxInt(void) 
{
    #if(0u != NOR_SPI_INTERNAL_TX_INT_ENABLED)
        CyIntEnable(NOR_SPI_TX_ISR_NUMBER);
    #endif /* (0u != NOR_SPI_INTERNAL_TX_INT_ENABLED) */
}


/*******************************************************************************
* Function Name: NOR_SPI_EnableRxInt
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
void NOR_SPI_EnableRxInt(void) 
{
    #if(0u != NOR_SPI_INTERNAL_RX_INT_ENABLED)
        CyIntEnable(NOR_SPI_RX_ISR_NUMBER);
    #endif /* (0u != NOR_SPI_INTERNAL_RX_INT_ENABLED) */
}


/*******************************************************************************
* Function Name: NOR_SPI_DisableTxInt
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
void NOR_SPI_DisableTxInt(void) 
{
    #if(0u != NOR_SPI_INTERNAL_TX_INT_ENABLED)
        CyIntDisable(NOR_SPI_TX_ISR_NUMBER);
    #endif /* (0u != NOR_SPI_INTERNAL_TX_INT_ENABLED) */
}


/*******************************************************************************
* Function Name: NOR_SPI_DisableRxInt
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
void NOR_SPI_DisableRxInt(void) 
{
    #if(0u != NOR_SPI_INTERNAL_RX_INT_ENABLED)
        CyIntDisable(NOR_SPI_RX_ISR_NUMBER);
    #endif /* (0u != NOR_SPI_INTERNAL_RX_INT_ENABLED) */
}


/*******************************************************************************
* Function Name: NOR_SPI_SetTxInterruptMode
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
void NOR_SPI_SetTxInterruptMode(uint8 intSrc) 
{
    NOR_SPI_TX_STATUS_MASK_REG = intSrc;
}


/*******************************************************************************
* Function Name: NOR_SPI_SetRxInterruptMode
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
void NOR_SPI_SetRxInterruptMode(uint8 intSrc) 
{
    NOR_SPI_RX_STATUS_MASK_REG  = intSrc;
}


/*******************************************************************************
* Function Name: NOR_SPI_ReadTxStatus
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
*  NOR_SPI_swStatusTx - used to store in software status register,
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
uint8 NOR_SPI_ReadTxStatus(void) 
{
    uint8 tmpStatus;

    #if(NOR_SPI_TX_SOFTWARE_BUF_ENABLED)
        /* Disable TX interrupt to protect global veriables */
        NOR_SPI_DisableTxInt();

        tmpStatus = NOR_SPI_GET_STATUS_TX(NOR_SPI_swStatusTx);
        NOR_SPI_swStatusTx = 0u;

        NOR_SPI_EnableTxInt();

    #else

        tmpStatus = NOR_SPI_TX_STATUS_REG;

    #endif /* (NOR_SPI_TX_SOFTWARE_BUF_ENABLED) */

    return(tmpStatus);
}


/*******************************************************************************
* Function Name: NOR_SPI_ReadRxStatus
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
*  NOR_SPI_swStatusRx - used to store in software Rx status register,
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
uint8 NOR_SPI_ReadRxStatus(void) 
{
    uint8 tmpStatus;

    #if(NOR_SPI_RX_SOFTWARE_BUF_ENABLED)
        /* Disable RX interrupt to protect global veriables */
        NOR_SPI_DisableRxInt();

        tmpStatus = NOR_SPI_GET_STATUS_RX(NOR_SPI_swStatusRx);
        NOR_SPI_swStatusRx = 0u;

        NOR_SPI_EnableRxInt();

    #else

        tmpStatus = NOR_SPI_RX_STATUS_REG;

    #endif /* (NOR_SPI_RX_SOFTWARE_BUF_ENABLED) */

    return(tmpStatus);
}


/*******************************************************************************
* Function Name: NOR_SPI_WriteTxData
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
*  NOR_SPI_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer, modified every function
*  call if TX Software Buffer is used.
*  NOR_SPI_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer.
*  NOR_SPI_txBuffer[NOR_SPI_TX_BUFFER_SIZE] - used to store
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
void NOR_SPI_WriteTxData(uint8 txData) 
{
    #if(NOR_SPI_TX_SOFTWARE_BUF_ENABLED)

        uint8 tempStatus;
        uint8 tmpTxBufferRead;

        /* Block if TX buffer is FULL: don't overwrite */
        do
        {
            tmpTxBufferRead = NOR_SPI_txBufferRead;
            if(0u == tmpTxBufferRead)
            {
                tmpTxBufferRead = (NOR_SPI_TX_BUFFER_SIZE - 1u);
            }
            else
            {
                tmpTxBufferRead--;
            }

        }while(tmpTxBufferRead == NOR_SPI_txBufferWrite);

        /* Disable TX interrupt to protect global veriables */
        NOR_SPI_DisableTxInt();

        tempStatus = NOR_SPI_GET_STATUS_TX(NOR_SPI_swStatusTx);
        NOR_SPI_swStatusTx = tempStatus;


        if((NOR_SPI_txBufferRead == NOR_SPI_txBufferWrite) &&
           (0u != (NOR_SPI_swStatusTx & NOR_SPI_STS_TX_FIFO_NOT_FULL)))
        {
            /* Put data element into the TX FIFO */
            CY_SET_REG8(NOR_SPI_TXDATA_PTR, txData);
        }
        else
        {
            /* Add to the TX software buffer */
            NOR_SPI_txBufferWrite++;
            if(NOR_SPI_txBufferWrite >= NOR_SPI_TX_BUFFER_SIZE)
            {
                NOR_SPI_txBufferWrite = 0u;
            }

            if(NOR_SPI_txBufferWrite == NOR_SPI_txBufferRead)
            {
                NOR_SPI_txBufferRead++;
                if(NOR_SPI_txBufferRead >= NOR_SPI_TX_BUFFER_SIZE)
                {
                    NOR_SPI_txBufferRead = 0u;
                }
                NOR_SPI_txBufferFull = 1u;
            }

            NOR_SPI_txBuffer[NOR_SPI_txBufferWrite] = txData;

            NOR_SPI_TX_STATUS_MASK_REG |= NOR_SPI_STS_TX_FIFO_NOT_FULL;
        }

        NOR_SPI_EnableTxInt();

    #else
        /* Wait until TX FIFO has a place */
        while(0u == (NOR_SPI_TX_STATUS_REG & NOR_SPI_STS_TX_FIFO_NOT_FULL))
        {
        }

        /* Put data element into the TX FIFO */
        CY_SET_REG8(NOR_SPI_TXDATA_PTR, txData);

    #endif /* (NOR_SPI_TX_SOFTWARE_BUF_ENABLED) */
}


/*******************************************************************************
* Function Name: NOR_SPI_ReadRxData
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
*  NOR_SPI_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer.
*  NOR_SPI_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer, modified every function
*  call if RX Software Buffer is used.
*  NOR_SPI_rxBuffer[NOR_SPI_RX_BUFFER_SIZE] - used to store
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
uint8 NOR_SPI_ReadRxData(void) 
{
    uint8 rxData;

    #if(NOR_SPI_RX_SOFTWARE_BUF_ENABLED)

        /* Disable RX interrupt to protect global veriables */
        NOR_SPI_DisableRxInt();

        if(NOR_SPI_rxBufferRead != NOR_SPI_rxBufferWrite)
        {
            if(0u == NOR_SPI_rxBufferFull)
            {
                NOR_SPI_rxBufferRead++;
                if(NOR_SPI_rxBufferRead >= NOR_SPI_RX_BUFFER_SIZE)
                {
                    NOR_SPI_rxBufferRead = 0u;
                }
            }
            else
            {
                NOR_SPI_rxBufferFull = 0u;
            }
        }

        rxData = NOR_SPI_rxBuffer[NOR_SPI_rxBufferRead];

        NOR_SPI_EnableRxInt();

    #else

        rxData = CY_GET_REG8(NOR_SPI_RXDATA_PTR);

    #endif /* (NOR_SPI_RX_SOFTWARE_BUF_ENABLED) */

    return(rxData);
}


/*******************************************************************************
* Function Name: NOR_SPI_GetRxBufferSize
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
*  NOR_SPI_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer.
*  NOR_SPI_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer.
*
* Side Effects:
*  Clear status register of the component.
*
*******************************************************************************/
uint8 NOR_SPI_GetRxBufferSize(void) 
{
    uint8 size;

    #if(NOR_SPI_RX_SOFTWARE_BUF_ENABLED)

        /* Disable RX interrupt to protect global veriables */
        NOR_SPI_DisableRxInt();

        if(NOR_SPI_rxBufferRead == NOR_SPI_rxBufferWrite)
        {
            size = 0u;
        }
        else if(NOR_SPI_rxBufferRead < NOR_SPI_rxBufferWrite)
        {
            size = (NOR_SPI_rxBufferWrite - NOR_SPI_rxBufferRead);
        }
        else
        {
            size = (NOR_SPI_RX_BUFFER_SIZE - NOR_SPI_rxBufferRead) + NOR_SPI_rxBufferWrite;
        }

        NOR_SPI_EnableRxInt();

    #else

        /* We can only know if there is data in the RX FIFO */
        size = (0u != (NOR_SPI_RX_STATUS_REG & NOR_SPI_STS_RX_FIFO_NOT_EMPTY)) ? 1u : 0u;

    #endif /* (NOR_SPI_TX_SOFTWARE_BUF_ENABLED) */

    return(size);
}


/*******************************************************************************
* Function Name: NOR_SPI_GetTxBufferSize
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
*  NOR_SPI_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer.
*  NOR_SPI_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer.
*
* Side Effects:
*  Clear status register of the component.
*
*******************************************************************************/
uint8  NOR_SPI_GetTxBufferSize(void) 
{
    uint8 size;

    #if(NOR_SPI_TX_SOFTWARE_BUF_ENABLED)
        /* Disable TX interrupt to protect global veriables */
        NOR_SPI_DisableTxInt();

        if(NOR_SPI_txBufferRead == NOR_SPI_txBufferWrite)
        {
            size = 0u;
        }
        else if(NOR_SPI_txBufferRead < NOR_SPI_txBufferWrite)
        {
            size = (NOR_SPI_txBufferWrite - NOR_SPI_txBufferRead);
        }
        else
        {
            size = (NOR_SPI_TX_BUFFER_SIZE - NOR_SPI_txBufferRead) + NOR_SPI_txBufferWrite;
        }

        NOR_SPI_EnableTxInt();

    #else

        size = NOR_SPI_TX_STATUS_REG;

        if(0u != (size & NOR_SPI_STS_TX_FIFO_EMPTY))
        {
            size = 0u;
        }
        else if(0u != (size & NOR_SPI_STS_TX_FIFO_NOT_FULL))
        {
            size = 1u;
        }
        else
        {
            size = NOR_SPI_FIFO_SIZE;
        }

    #endif /* (NOR_SPI_TX_SOFTWARE_BUF_ENABLED) */

    return(size);
}


/*******************************************************************************
* Function Name: NOR_SPI_ClearRxBuffer
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
*  NOR_SPI_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer, modified every function
*  call - resets to zero.
*  NOR_SPI_rxBufferRead - used for the account of the bytes which
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
void NOR_SPI_ClearRxBuffer(void) 
{
    /* Clear Hardware RX FIFO */
    while(0u !=(NOR_SPI_RX_STATUS_REG & NOR_SPI_STS_RX_FIFO_NOT_EMPTY))
    {
        (void) CY_GET_REG8(NOR_SPI_RXDATA_PTR);
    }

    #if(NOR_SPI_RX_SOFTWARE_BUF_ENABLED)
        /* Disable RX interrupt to protect global veriables */
        NOR_SPI_DisableRxInt();

        NOR_SPI_rxBufferFull  = 0u;
        NOR_SPI_rxBufferRead  = 0u;
        NOR_SPI_rxBufferWrite = 0u;

        NOR_SPI_EnableRxInt();
    #endif /* (NOR_SPI_RX_SOFTWARE_BUF_ENABLED) */
}


/*******************************************************************************
* Function Name: NOR_SPI_ClearTxBuffer
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
*  NOR_SPI_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer, modified every function
*  call - resets to zero.
*  NOR_SPI_txBufferRead - used for the account of the bytes which
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
void NOR_SPI_ClearTxBuffer(void) 
{
    uint8 enableInterrupts;

    enableInterrupts = CyEnterCriticalSection();
    /* Clear TX FIFO */
    NOR_SPI_AUX_CONTROL_DP0_REG |= ((uint8)  NOR_SPI_TX_FIFO_CLR);
    NOR_SPI_AUX_CONTROL_DP0_REG &= ((uint8) ~NOR_SPI_TX_FIFO_CLR);

    #if(NOR_SPI_USE_SECOND_DATAPATH)
        /* Clear TX FIFO for 2nd Datapath */
        NOR_SPI_AUX_CONTROL_DP1_REG |= ((uint8)  NOR_SPI_TX_FIFO_CLR);
        NOR_SPI_AUX_CONTROL_DP1_REG &= ((uint8) ~NOR_SPI_TX_FIFO_CLR);
    #endif /* (NOR_SPI_USE_SECOND_DATAPATH) */
    CyExitCriticalSection(enableInterrupts);

    #if(NOR_SPI_TX_SOFTWARE_BUF_ENABLED)
        /* Disable TX interrupt to protect global veriables */
        NOR_SPI_DisableTxInt();

        NOR_SPI_txBufferFull  = 0u;
        NOR_SPI_txBufferRead  = 0u;
        NOR_SPI_txBufferWrite = 0u;

        /* Buffer is EMPTY: disable TX FIFO NOT FULL interrupt */
        NOR_SPI_TX_STATUS_MASK_REG &= ((uint8) ~NOR_SPI_STS_TX_FIFO_NOT_FULL);

        NOR_SPI_EnableTxInt();
    #endif /* (NOR_SPI_TX_SOFTWARE_BUF_ENABLED) */
}


#if(0u != NOR_SPI_BIDIRECTIONAL_MODE)
    /*******************************************************************************
    * Function Name: NOR_SPI_TxEnable
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
    void NOR_SPI_TxEnable(void) 
    {
        NOR_SPI_CONTROL_REG |= NOR_SPI_CTRL_TX_SIGNAL_EN;
    }


    /*******************************************************************************
    * Function Name: NOR_SPI_TxDisable
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
    void NOR_SPI_TxDisable(void) 
    {
        NOR_SPI_CONTROL_REG &= ((uint8) ~NOR_SPI_CTRL_TX_SIGNAL_EN);
    }

#endif /* (0u != NOR_SPI_BIDIRECTIONAL_MODE) */


/*******************************************************************************
* Function Name: NOR_SPI_PutArray
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
void NOR_SPI_PutArray(const uint8 buffer[], uint8 byteCount)
                                                                          
{
    uint8 bufIndex;

    bufIndex = 0u;

    while(byteCount > 0u)
    {
        NOR_SPI_WriteTxData(buffer[bufIndex]);
        bufIndex++;
        byteCount--;
    }
}


/*******************************************************************************
* Function Name: NOR_SPI_ClearFIFO
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
void NOR_SPI_ClearFIFO(void) 
{
    uint8 enableInterrupts;

    /* Clear Hardware RX FIFO */
    while(0u !=(NOR_SPI_RX_STATUS_REG & NOR_SPI_STS_RX_FIFO_NOT_EMPTY))
    {
        (void) CY_GET_REG8(NOR_SPI_RXDATA_PTR);
    }

    enableInterrupts = CyEnterCriticalSection();
    /* Clear TX FIFO */
    NOR_SPI_AUX_CONTROL_DP0_REG |= ((uint8)  NOR_SPI_TX_FIFO_CLR);
    NOR_SPI_AUX_CONTROL_DP0_REG &= ((uint8) ~NOR_SPI_TX_FIFO_CLR);

    #if(NOR_SPI_USE_SECOND_DATAPATH)
        /* Clear TX FIFO for 2nd Datapath */
        NOR_SPI_AUX_CONTROL_DP1_REG |= ((uint8)  NOR_SPI_TX_FIFO_CLR);
        NOR_SPI_AUX_CONTROL_DP1_REG &= ((uint8) ~NOR_SPI_TX_FIFO_CLR);
    #endif /* (NOR_SPI_USE_SECOND_DATAPATH) */
    CyExitCriticalSection(enableInterrupts);
}


/* Following functions are for version Compatibility, they are obsolete.
*  Please do not use it in new projects.
*/


/*******************************************************************************
* Function Name: NOR_SPI_EnableInt
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
void NOR_SPI_EnableInt(void) 
{
    NOR_SPI_EnableRxInt();
    NOR_SPI_EnableTxInt();
}


/*******************************************************************************
* Function Name: NOR_SPI_DisableInt
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
void NOR_SPI_DisableInt(void) 
{
    NOR_SPI_DisableTxInt();
    NOR_SPI_DisableRxInt();
}


/*******************************************************************************
* Function Name: NOR_SPI_SetInterruptMode
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
void NOR_SPI_SetInterruptMode(uint8 intSrc) 
{
    NOR_SPI_TX_STATUS_MASK_REG  = (intSrc & ((uint8) ~NOR_SPI_STS_SPI_IDLE));
    NOR_SPI_RX_STATUS_MASK_REG  =  intSrc;
}


/*******************************************************************************
* Function Name: NOR_SPI_ReadStatus
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
*  NOR_SPI_swStatus - used to store in software status register,
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
uint8 NOR_SPI_ReadStatus(void) 
{
    uint8 tmpStatus;

    #if(NOR_SPI_TX_SOFTWARE_BUF_ENABLED || NOR_SPI_RX_SOFTWARE_BUF_ENABLED)

        NOR_SPI_DisableInt();

        tmpStatus  = NOR_SPI_GET_STATUS_RX(NOR_SPI_swStatusRx);
        tmpStatus |= NOR_SPI_GET_STATUS_TX(NOR_SPI_swStatusTx);
        tmpStatus &= ((uint8) ~NOR_SPI_STS_SPI_IDLE);

        NOR_SPI_swStatusTx = 0u;
        NOR_SPI_swStatusRx = 0u;

        NOR_SPI_EnableInt();

    #else

        tmpStatus  = NOR_SPI_RX_STATUS_REG;
        tmpStatus |= NOR_SPI_TX_STATUS_REG;
        tmpStatus &= ((uint8) ~NOR_SPI_STS_SPI_IDLE);

    #endif /* (NOR_SPI_TX_SOFTWARE_BUF_ENABLED || NOR_SPI_RX_SOFTWARE_BUF_ENABLED) */

    return(tmpStatus);
}


/* [] END OF FILE */
