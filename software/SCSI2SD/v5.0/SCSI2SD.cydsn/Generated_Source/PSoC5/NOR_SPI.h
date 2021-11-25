/*******************************************************************************
* File Name: NOR_SPI.h
* Version 2.50
*
* Description:
*  Contains the function prototypes, constants and register definition
*  of the SPI Master Component.
*
* Note:
*  None
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SPIM_NOR_SPI_H)
#define CY_SPIM_NOR_SPI_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */


/***************************************
*   Conditional Compilation Parameters
***************************************/

#define NOR_SPI_INTERNAL_CLOCK             (0u)

#if(0u != NOR_SPI_INTERNAL_CLOCK)
    #include "NOR_SPI_IntClock.h"
#endif /* (0u != NOR_SPI_INTERNAL_CLOCK) */

#define NOR_SPI_MODE                       (1u)
#define NOR_SPI_DATA_WIDTH                 (8u)
#define NOR_SPI_MODE_USE_ZERO              (1u)
#define NOR_SPI_BIDIRECTIONAL_MODE         (0u)

/* Internal interrupt handling */
#define NOR_SPI_TX_BUFFER_SIZE             (4u)
#define NOR_SPI_RX_BUFFER_SIZE             (4u)
#define NOR_SPI_INTERNAL_TX_INT_ENABLED    (0u)
#define NOR_SPI_INTERNAL_RX_INT_ENABLED    (0u)

#define NOR_SPI_SINGLE_REG_SIZE            (8u)
#define NOR_SPI_USE_SECOND_DATAPATH        (NOR_SPI_DATA_WIDTH > NOR_SPI_SINGLE_REG_SIZE)

#define NOR_SPI_FIFO_SIZE                  (4u)
#define NOR_SPI_TX_SOFTWARE_BUF_ENABLED    ((0u != NOR_SPI_INTERNAL_TX_INT_ENABLED) && \
                                                     (NOR_SPI_TX_BUFFER_SIZE > NOR_SPI_FIFO_SIZE))

#define NOR_SPI_RX_SOFTWARE_BUF_ENABLED    ((0u != NOR_SPI_INTERNAL_RX_INT_ENABLED) && \
                                                     (NOR_SPI_RX_BUFFER_SIZE > NOR_SPI_FIFO_SIZE))


/***************************************
*        Data Struct Definition
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 enableState;
    uint8 cntrPeriod;
} NOR_SPI_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

void  NOR_SPI_Init(void)                           ;
void  NOR_SPI_Enable(void)                         ;
void  NOR_SPI_Start(void)                          ;
void  NOR_SPI_Stop(void)                           ;

void  NOR_SPI_EnableTxInt(void)                    ;
void  NOR_SPI_EnableRxInt(void)                    ;
void  NOR_SPI_DisableTxInt(void)                   ;
void  NOR_SPI_DisableRxInt(void)                   ;

void  NOR_SPI_Sleep(void)                          ;
void  NOR_SPI_Wakeup(void)                         ;
void  NOR_SPI_SaveConfig(void)                     ;
void  NOR_SPI_RestoreConfig(void)                  ;

void  NOR_SPI_SetTxInterruptMode(uint8 intSrc)     ;
void  NOR_SPI_SetRxInterruptMode(uint8 intSrc)     ;
uint8 NOR_SPI_ReadTxStatus(void)                   ;
uint8 NOR_SPI_ReadRxStatus(void)                   ;
void  NOR_SPI_WriteTxData(uint8 txData)  \
                                                            ;
uint8 NOR_SPI_ReadRxData(void) \
                                                            ;
uint8 NOR_SPI_GetRxBufferSize(void)                ;
uint8 NOR_SPI_GetTxBufferSize(void)                ;
void  NOR_SPI_ClearRxBuffer(void)                  ;
void  NOR_SPI_ClearTxBuffer(void)                  ;
void  NOR_SPI_ClearFIFO(void)                              ;
void  NOR_SPI_PutArray(const uint8 buffer[], uint8 byteCount) \
                                                            ;

#if(0u != NOR_SPI_BIDIRECTIONAL_MODE)
    void  NOR_SPI_TxEnable(void)                   ;
    void  NOR_SPI_TxDisable(void)                  ;
#endif /* (0u != NOR_SPI_BIDIRECTIONAL_MODE) */

CY_ISR_PROTO(NOR_SPI_TX_ISR);
CY_ISR_PROTO(NOR_SPI_RX_ISR);


/***************************************
*   Variable with external linkage
***************************************/

extern uint8 NOR_SPI_initVar;


/***************************************
*           API Constants
***************************************/

#define NOR_SPI_TX_ISR_NUMBER     ((uint8) (NOR_SPI_TxInternalInterrupt__INTC_NUMBER))
#define NOR_SPI_RX_ISR_NUMBER     ((uint8) (NOR_SPI_RxInternalInterrupt__INTC_NUMBER))

#define NOR_SPI_TX_ISR_PRIORITY   ((uint8) (NOR_SPI_TxInternalInterrupt__INTC_PRIOR_NUM))
#define NOR_SPI_RX_ISR_PRIORITY   ((uint8) (NOR_SPI_RxInternalInterrupt__INTC_PRIOR_NUM))


/***************************************
*    Initial Parameter Constants
***************************************/

#define NOR_SPI_INT_ON_SPI_DONE    ((uint8) (0u   << NOR_SPI_STS_SPI_DONE_SHIFT))
#define NOR_SPI_INT_ON_TX_EMPTY    ((uint8) (1u   << NOR_SPI_STS_TX_FIFO_EMPTY_SHIFT))
#define NOR_SPI_INT_ON_TX_NOT_FULL ((uint8) (0u << \
                                                                           NOR_SPI_STS_TX_FIFO_NOT_FULL_SHIFT))
#define NOR_SPI_INT_ON_BYTE_COMP   ((uint8) (0u  << NOR_SPI_STS_BYTE_COMPLETE_SHIFT))
#define NOR_SPI_INT_ON_SPI_IDLE    ((uint8) (0u   << NOR_SPI_STS_SPI_IDLE_SHIFT))

/* Disable TX_NOT_FULL if software buffer is used */
#define NOR_SPI_INT_ON_TX_NOT_FULL_DEF ((NOR_SPI_TX_SOFTWARE_BUF_ENABLED) ? \
                                                                        (0u) : (NOR_SPI_INT_ON_TX_NOT_FULL))

/* TX interrupt mask */
#define NOR_SPI_TX_INIT_INTERRUPTS_MASK    (NOR_SPI_INT_ON_SPI_DONE  | \
                                                     NOR_SPI_INT_ON_TX_EMPTY  | \
                                                     NOR_SPI_INT_ON_TX_NOT_FULL_DEF | \
                                                     NOR_SPI_INT_ON_BYTE_COMP | \
                                                     NOR_SPI_INT_ON_SPI_IDLE)

#define NOR_SPI_INT_ON_RX_FULL         ((uint8) (0u << \
                                                                          NOR_SPI_STS_RX_FIFO_FULL_SHIFT))
#define NOR_SPI_INT_ON_RX_NOT_EMPTY    ((uint8) (1u << \
                                                                          NOR_SPI_STS_RX_FIFO_NOT_EMPTY_SHIFT))
#define NOR_SPI_INT_ON_RX_OVER         ((uint8) (0u << \
                                                                          NOR_SPI_STS_RX_FIFO_OVERRUN_SHIFT))

/* RX interrupt mask */
#define NOR_SPI_RX_INIT_INTERRUPTS_MASK    (NOR_SPI_INT_ON_RX_FULL      | \
                                                     NOR_SPI_INT_ON_RX_NOT_EMPTY | \
                                                     NOR_SPI_INT_ON_RX_OVER)
/* Nubmer of bits to receive/transmit */
#define NOR_SPI_BITCTR_INIT            (((uint8) (NOR_SPI_DATA_WIDTH << 1u)) - 1u)


/***************************************
*             Registers
***************************************/
#if(CY_PSOC3 || CY_PSOC5)
    #define NOR_SPI_TXDATA_REG (* (reg8 *) \
                                                NOR_SPI_BSPIM_sR8_Dp_u0__F0_REG)
    #define NOR_SPI_TXDATA_PTR (  (reg8 *) \
                                                NOR_SPI_BSPIM_sR8_Dp_u0__F0_REG)
    #define NOR_SPI_RXDATA_REG (* (reg8 *) \
                                                NOR_SPI_BSPIM_sR8_Dp_u0__F1_REG)
    #define NOR_SPI_RXDATA_PTR (  (reg8 *) \
                                                NOR_SPI_BSPIM_sR8_Dp_u0__F1_REG)
#else   /* PSOC4 */
    #if(NOR_SPI_USE_SECOND_DATAPATH)
        #define NOR_SPI_TXDATA_REG (* (reg16 *) \
                                          NOR_SPI_BSPIM_sR8_Dp_u0__16BIT_F0_REG)
        #define NOR_SPI_TXDATA_PTR (  (reg16 *) \
                                          NOR_SPI_BSPIM_sR8_Dp_u0__16BIT_F0_REG)
        #define NOR_SPI_RXDATA_REG (* (reg16 *) \
                                          NOR_SPI_BSPIM_sR8_Dp_u0__16BIT_F1_REG)
        #define NOR_SPI_RXDATA_PTR (  (reg16 *) \
                                          NOR_SPI_BSPIM_sR8_Dp_u0__16BIT_F1_REG)
    #else
        #define NOR_SPI_TXDATA_REG (* (reg8 *) \
                                                NOR_SPI_BSPIM_sR8_Dp_u0__F0_REG)
        #define NOR_SPI_TXDATA_PTR (  (reg8 *) \
                                                NOR_SPI_BSPIM_sR8_Dp_u0__F0_REG)
        #define NOR_SPI_RXDATA_REG (* (reg8 *) \
                                                NOR_SPI_BSPIM_sR8_Dp_u0__F1_REG)
        #define NOR_SPI_RXDATA_PTR (  (reg8 *) \
                                                NOR_SPI_BSPIM_sR8_Dp_u0__F1_REG)
    #endif /* (NOR_SPI_USE_SECOND_DATAPATH) */
#endif     /* (CY_PSOC3 || CY_PSOC5) */

#define NOR_SPI_AUX_CONTROL_DP0_REG (* (reg8 *) \
                                        NOR_SPI_BSPIM_sR8_Dp_u0__DP_AUX_CTL_REG)
#define NOR_SPI_AUX_CONTROL_DP0_PTR (  (reg8 *) \
                                        NOR_SPI_BSPIM_sR8_Dp_u0__DP_AUX_CTL_REG)

#if(NOR_SPI_USE_SECOND_DATAPATH)
    #define NOR_SPI_AUX_CONTROL_DP1_REG  (* (reg8 *) \
                                        NOR_SPI_BSPIM_sR8_Dp_u1__DP_AUX_CTL_REG)
    #define NOR_SPI_AUX_CONTROL_DP1_PTR  (  (reg8 *) \
                                        NOR_SPI_BSPIM_sR8_Dp_u1__DP_AUX_CTL_REG)
#endif /* (NOR_SPI_USE_SECOND_DATAPATH) */

#define NOR_SPI_COUNTER_PERIOD_REG     (* (reg8 *) NOR_SPI_BSPIM_BitCounter__PERIOD_REG)
#define NOR_SPI_COUNTER_PERIOD_PTR     (  (reg8 *) NOR_SPI_BSPIM_BitCounter__PERIOD_REG)
#define NOR_SPI_COUNTER_CONTROL_REG    (* (reg8 *) NOR_SPI_BSPIM_BitCounter__CONTROL_AUX_CTL_REG)
#define NOR_SPI_COUNTER_CONTROL_PTR    (  (reg8 *) NOR_SPI_BSPIM_BitCounter__CONTROL_AUX_CTL_REG)

#define NOR_SPI_TX_STATUS_REG          (* (reg8 *) NOR_SPI_BSPIM_TxStsReg__STATUS_REG)
#define NOR_SPI_TX_STATUS_PTR          (  (reg8 *) NOR_SPI_BSPIM_TxStsReg__STATUS_REG)
#define NOR_SPI_RX_STATUS_REG          (* (reg8 *) NOR_SPI_BSPIM_RxStsReg__STATUS_REG)
#define NOR_SPI_RX_STATUS_PTR          (  (reg8 *) NOR_SPI_BSPIM_RxStsReg__STATUS_REG)

#define NOR_SPI_CONTROL_REG            (* (reg8 *) \
                                      NOR_SPI_BSPIM_BidirMode_CtrlReg__CONTROL_REG)
#define NOR_SPI_CONTROL_PTR            (  (reg8 *) \
                                      NOR_SPI_BSPIM_BidirMode_CtrlReg__CONTROL_REG)

#define NOR_SPI_TX_STATUS_MASK_REG     (* (reg8 *) NOR_SPI_BSPIM_TxStsReg__MASK_REG)
#define NOR_SPI_TX_STATUS_MASK_PTR     (  (reg8 *) NOR_SPI_BSPIM_TxStsReg__MASK_REG)
#define NOR_SPI_RX_STATUS_MASK_REG     (* (reg8 *) NOR_SPI_BSPIM_RxStsReg__MASK_REG)
#define NOR_SPI_RX_STATUS_MASK_PTR     (  (reg8 *) NOR_SPI_BSPIM_RxStsReg__MASK_REG)

#define NOR_SPI_TX_STATUS_ACTL_REG     (* (reg8 *) NOR_SPI_BSPIM_TxStsReg__STATUS_AUX_CTL_REG)
#define NOR_SPI_TX_STATUS_ACTL_PTR     (  (reg8 *) NOR_SPI_BSPIM_TxStsReg__STATUS_AUX_CTL_REG)
#define NOR_SPI_RX_STATUS_ACTL_REG     (* (reg8 *) NOR_SPI_BSPIM_RxStsReg__STATUS_AUX_CTL_REG)
#define NOR_SPI_RX_STATUS_ACTL_PTR     (  (reg8 *) NOR_SPI_BSPIM_RxStsReg__STATUS_AUX_CTL_REG)

#if(NOR_SPI_USE_SECOND_DATAPATH)
    #define NOR_SPI_AUX_CONTROLDP1     (NOR_SPI_AUX_CONTROL_DP1_REG)
#endif /* (NOR_SPI_USE_SECOND_DATAPATH) */


/***************************************
*       Register Constants
***************************************/

/* Status Register Definitions */
#define NOR_SPI_STS_SPI_DONE_SHIFT             (0x00u)
#define NOR_SPI_STS_TX_FIFO_EMPTY_SHIFT        (0x01u)
#define NOR_SPI_STS_TX_FIFO_NOT_FULL_SHIFT     (0x02u)
#define NOR_SPI_STS_BYTE_COMPLETE_SHIFT        (0x03u)
#define NOR_SPI_STS_SPI_IDLE_SHIFT             (0x04u)
#define NOR_SPI_STS_RX_FIFO_FULL_SHIFT         (0x04u)
#define NOR_SPI_STS_RX_FIFO_NOT_EMPTY_SHIFT    (0x05u)
#define NOR_SPI_STS_RX_FIFO_OVERRUN_SHIFT      (0x06u)

#define NOR_SPI_STS_SPI_DONE           ((uint8) (0x01u << NOR_SPI_STS_SPI_DONE_SHIFT))
#define NOR_SPI_STS_TX_FIFO_EMPTY      ((uint8) (0x01u << NOR_SPI_STS_TX_FIFO_EMPTY_SHIFT))
#define NOR_SPI_STS_TX_FIFO_NOT_FULL   ((uint8) (0x01u << NOR_SPI_STS_TX_FIFO_NOT_FULL_SHIFT))
#define NOR_SPI_STS_BYTE_COMPLETE      ((uint8) (0x01u << NOR_SPI_STS_BYTE_COMPLETE_SHIFT))
#define NOR_SPI_STS_SPI_IDLE           ((uint8) (0x01u << NOR_SPI_STS_SPI_IDLE_SHIFT))
#define NOR_SPI_STS_RX_FIFO_FULL       ((uint8) (0x01u << NOR_SPI_STS_RX_FIFO_FULL_SHIFT))
#define NOR_SPI_STS_RX_FIFO_NOT_EMPTY  ((uint8) (0x01u << NOR_SPI_STS_RX_FIFO_NOT_EMPTY_SHIFT))
#define NOR_SPI_STS_RX_FIFO_OVERRUN    ((uint8) (0x01u << NOR_SPI_STS_RX_FIFO_OVERRUN_SHIFT))

/* TX and RX masks for clear on read bits */
#define NOR_SPI_TX_STS_CLR_ON_RD_BYTES_MASK    (0x09u)
#define NOR_SPI_RX_STS_CLR_ON_RD_BYTES_MASK    (0x40u)

/* StatusI Register Interrupt Enable Control Bits */
/* As defined by the Register map for the AUX Control Register */
#define NOR_SPI_INT_ENABLE     (0x10u) /* Enable interrupt from statusi */
#define NOR_SPI_TX_FIFO_CLR    (0x01u) /* F0 - TX FIFO */
#define NOR_SPI_RX_FIFO_CLR    (0x02u) /* F1 - RX FIFO */
#define NOR_SPI_FIFO_CLR       (NOR_SPI_TX_FIFO_CLR | NOR_SPI_RX_FIFO_CLR)

/* Bit Counter (7-bit) Control Register Bit Definitions */
/* As defined by the Register map for the AUX Control Register */
#define NOR_SPI_CNTR_ENABLE    (0x20u) /* Enable CNT7 */

/* Bi-Directional mode control bit */
#define NOR_SPI_CTRL_TX_SIGNAL_EN  (0x01u)

/* Datapath Auxillary Control Register definitions */
#define NOR_SPI_AUX_CTRL_FIFO0_CLR         (0x01u)
#define NOR_SPI_AUX_CTRL_FIFO1_CLR         (0x02u)
#define NOR_SPI_AUX_CTRL_FIFO0_LVL         (0x04u)
#define NOR_SPI_AUX_CTRL_FIFO1_LVL         (0x08u)
#define NOR_SPI_STATUS_ACTL_INT_EN_MASK    (0x10u)

/* Component disabled */
#define NOR_SPI_DISABLED   (0u)


/***************************************
*       Macros
***************************************/

/* Returns true if componentn enabled */
#define NOR_SPI_IS_ENABLED (0u != (NOR_SPI_TX_STATUS_ACTL_REG & NOR_SPI_INT_ENABLE))

/* Retuns TX status register */
#define NOR_SPI_GET_STATUS_TX(swTxSts) ( (uint8)(NOR_SPI_TX_STATUS_REG | \
                                                          ((swTxSts) & NOR_SPI_TX_STS_CLR_ON_RD_BYTES_MASK)) )
/* Retuns RX status register */
#define NOR_SPI_GET_STATUS_RX(swRxSts) ( (uint8)(NOR_SPI_RX_STATUS_REG | \
                                                          ((swRxSts) & NOR_SPI_RX_STS_CLR_ON_RD_BYTES_MASK)) )


/***************************************
* The following code is DEPRECATED and 
* should not be used in new projects.
***************************************/

#define NOR_SPI_WriteByte   NOR_SPI_WriteTxData
#define NOR_SPI_ReadByte    NOR_SPI_ReadRxData
void  NOR_SPI_SetInterruptMode(uint8 intSrc)       ;
uint8 NOR_SPI_ReadStatus(void)                     ;
void  NOR_SPI_EnableInt(void)                      ;
void  NOR_SPI_DisableInt(void)                     ;

#define NOR_SPI_TXDATA                 (NOR_SPI_TXDATA_REG)
#define NOR_SPI_RXDATA                 (NOR_SPI_RXDATA_REG)
#define NOR_SPI_AUX_CONTROLDP0         (NOR_SPI_AUX_CONTROL_DP0_REG)
#define NOR_SPI_TXBUFFERREAD           (NOR_SPI_txBufferRead)
#define NOR_SPI_TXBUFFERWRITE          (NOR_SPI_txBufferWrite)
#define NOR_SPI_RXBUFFERREAD           (NOR_SPI_rxBufferRead)
#define NOR_SPI_RXBUFFERWRITE          (NOR_SPI_rxBufferWrite)

#define NOR_SPI_COUNTER_PERIOD         (NOR_SPI_COUNTER_PERIOD_REG)
#define NOR_SPI_COUNTER_CONTROL        (NOR_SPI_COUNTER_CONTROL_REG)
#define NOR_SPI_STATUS                 (NOR_SPI_TX_STATUS_REG)
#define NOR_SPI_CONTROL                (NOR_SPI_CONTROL_REG)
#define NOR_SPI_STATUS_MASK            (NOR_SPI_TX_STATUS_MASK_REG)
#define NOR_SPI_STATUS_ACTL            (NOR_SPI_TX_STATUS_ACTL_REG)

#define NOR_SPI_INIT_INTERRUPTS_MASK  (NOR_SPI_INT_ON_SPI_DONE     | \
                                                NOR_SPI_INT_ON_TX_EMPTY     | \
                                                NOR_SPI_INT_ON_TX_NOT_FULL_DEF  | \
                                                NOR_SPI_INT_ON_RX_FULL      | \
                                                NOR_SPI_INT_ON_RX_NOT_EMPTY | \
                                                NOR_SPI_INT_ON_RX_OVER      | \
                                                NOR_SPI_INT_ON_BYTE_COMP)
                                                
#define NOR_SPI_DataWidth                  (NOR_SPI_DATA_WIDTH)
#define NOR_SPI_InternalClockUsed          (NOR_SPI_INTERNAL_CLOCK)
#define NOR_SPI_InternalTxInterruptEnabled (NOR_SPI_INTERNAL_TX_INT_ENABLED)
#define NOR_SPI_InternalRxInterruptEnabled (NOR_SPI_INTERNAL_RX_INT_ENABLED)
#define NOR_SPI_ModeUseZero                (NOR_SPI_MODE_USE_ZERO)
#define NOR_SPI_BidirectionalMode          (NOR_SPI_BIDIRECTIONAL_MODE)
#define NOR_SPI_Mode                       (NOR_SPI_MODE)
#define NOR_SPI_DATAWIDHT                  (NOR_SPI_DATA_WIDTH)
#define NOR_SPI_InternalInterruptEnabled   (0u)

#define NOR_SPI_TXBUFFERSIZE   (NOR_SPI_TX_BUFFER_SIZE)
#define NOR_SPI_RXBUFFERSIZE   (NOR_SPI_RX_BUFFER_SIZE)

#define NOR_SPI_TXBUFFER       NOR_SPI_txBuffer
#define NOR_SPI_RXBUFFER       NOR_SPI_rxBuffer

#endif /* (CY_SPIM_NOR_SPI_H) */


/* [] END OF FILE */
