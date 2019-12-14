/*******************************************************************************
* File Name: WiFiSPIM.h
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

#if !defined(CY_SPIM_WiFiSPIM_H)
#define CY_SPIM_WiFiSPIM_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */


/***************************************
*   Conditional Compilation Parameters
***************************************/

#define WiFiSPIM_INTERNAL_CLOCK             (0u)

#if(0u != WiFiSPIM_INTERNAL_CLOCK)
    #include "WiFiSPIM_IntClock.h"
#endif /* (0u != WiFiSPIM_INTERNAL_CLOCK) */

#define WiFiSPIM_MODE                       (1u)
#define WiFiSPIM_DATA_WIDTH                 (8u)
#define WiFiSPIM_MODE_USE_ZERO              (1u)
#define WiFiSPIM_BIDIRECTIONAL_MODE         (0u)

/* Internal interrupt handling */
#define WiFiSPIM_TX_BUFFER_SIZE             (4u)
#define WiFiSPIM_RX_BUFFER_SIZE             (4u)
#define WiFiSPIM_INTERNAL_TX_INT_ENABLED    (0u)
#define WiFiSPIM_INTERNAL_RX_INT_ENABLED    (0u)

#define WiFiSPIM_SINGLE_REG_SIZE            (8u)
#define WiFiSPIM_USE_SECOND_DATAPATH        (WiFiSPIM_DATA_WIDTH > WiFiSPIM_SINGLE_REG_SIZE)

#define WiFiSPIM_FIFO_SIZE                  (4u)
#define WiFiSPIM_TX_SOFTWARE_BUF_ENABLED    ((0u != WiFiSPIM_INTERNAL_TX_INT_ENABLED) && \
                                                     (WiFiSPIM_TX_BUFFER_SIZE > WiFiSPIM_FIFO_SIZE))

#define WiFiSPIM_RX_SOFTWARE_BUF_ENABLED    ((0u != WiFiSPIM_INTERNAL_RX_INT_ENABLED) && \
                                                     (WiFiSPIM_RX_BUFFER_SIZE > WiFiSPIM_FIFO_SIZE))


/***************************************
*        Data Struct Definition
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 enableState;
    uint8 cntrPeriod;
} WiFiSPIM_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

void  WiFiSPIM_Init(void)                           ;
void  WiFiSPIM_Enable(void)                         ;
void  WiFiSPIM_Start(void)                          ;
void  WiFiSPIM_Stop(void)                           ;

void  WiFiSPIM_EnableTxInt(void)                    ;
void  WiFiSPIM_EnableRxInt(void)                    ;
void  WiFiSPIM_DisableTxInt(void)                   ;
void  WiFiSPIM_DisableRxInt(void)                   ;

void  WiFiSPIM_Sleep(void)                          ;
void  WiFiSPIM_Wakeup(void)                         ;
void  WiFiSPIM_SaveConfig(void)                     ;
void  WiFiSPIM_RestoreConfig(void)                  ;

void  WiFiSPIM_SetTxInterruptMode(uint8 intSrc)     ;
void  WiFiSPIM_SetRxInterruptMode(uint8 intSrc)     ;
uint8 WiFiSPIM_ReadTxStatus(void)                   ;
uint8 WiFiSPIM_ReadRxStatus(void)                   ;
void  WiFiSPIM_WriteTxData(uint8 txData)  \
                                                            ;
uint8 WiFiSPIM_ReadRxData(void) \
                                                            ;
uint8 WiFiSPIM_GetRxBufferSize(void)                ;
uint8 WiFiSPIM_GetTxBufferSize(void)                ;
void  WiFiSPIM_ClearRxBuffer(void)                  ;
void  WiFiSPIM_ClearTxBuffer(void)                  ;
void  WiFiSPIM_ClearFIFO(void)                              ;
void  WiFiSPIM_PutArray(const uint8 buffer[], uint8 byteCount) \
                                                            ;

#if(0u != WiFiSPIM_BIDIRECTIONAL_MODE)
    void  WiFiSPIM_TxEnable(void)                   ;
    void  WiFiSPIM_TxDisable(void)                  ;
#endif /* (0u != WiFiSPIM_BIDIRECTIONAL_MODE) */

CY_ISR_PROTO(WiFiSPIM_TX_ISR);
CY_ISR_PROTO(WiFiSPIM_RX_ISR);


/***************************************
*   Variable with external linkage
***************************************/

extern uint8 WiFiSPIM_initVar;


/***************************************
*           API Constants
***************************************/

#define WiFiSPIM_TX_ISR_NUMBER     ((uint8) (WiFiSPIM_TxInternalInterrupt__INTC_NUMBER))
#define WiFiSPIM_RX_ISR_NUMBER     ((uint8) (WiFiSPIM_RxInternalInterrupt__INTC_NUMBER))

#define WiFiSPIM_TX_ISR_PRIORITY   ((uint8) (WiFiSPIM_TxInternalInterrupt__INTC_PRIOR_NUM))
#define WiFiSPIM_RX_ISR_PRIORITY   ((uint8) (WiFiSPIM_RxInternalInterrupt__INTC_PRIOR_NUM))


/***************************************
*    Initial Parameter Constants
***************************************/

#define WiFiSPIM_INT_ON_SPI_DONE    ((uint8) (0u   << WiFiSPIM_STS_SPI_DONE_SHIFT))
#define WiFiSPIM_INT_ON_TX_EMPTY    ((uint8) (1u   << WiFiSPIM_STS_TX_FIFO_EMPTY_SHIFT))
#define WiFiSPIM_INT_ON_TX_NOT_FULL ((uint8) (0u << \
                                                                           WiFiSPIM_STS_TX_FIFO_NOT_FULL_SHIFT))
#define WiFiSPIM_INT_ON_BYTE_COMP   ((uint8) (0u  << WiFiSPIM_STS_BYTE_COMPLETE_SHIFT))
#define WiFiSPIM_INT_ON_SPI_IDLE    ((uint8) (0u   << WiFiSPIM_STS_SPI_IDLE_SHIFT))

/* Disable TX_NOT_FULL if software buffer is used */
#define WiFiSPIM_INT_ON_TX_NOT_FULL_DEF ((WiFiSPIM_TX_SOFTWARE_BUF_ENABLED) ? \
                                                                        (0u) : (WiFiSPIM_INT_ON_TX_NOT_FULL))

/* TX interrupt mask */
#define WiFiSPIM_TX_INIT_INTERRUPTS_MASK    (WiFiSPIM_INT_ON_SPI_DONE  | \
                                                     WiFiSPIM_INT_ON_TX_EMPTY  | \
                                                     WiFiSPIM_INT_ON_TX_NOT_FULL_DEF | \
                                                     WiFiSPIM_INT_ON_BYTE_COMP | \
                                                     WiFiSPIM_INT_ON_SPI_IDLE)

#define WiFiSPIM_INT_ON_RX_FULL         ((uint8) (0u << \
                                                                          WiFiSPIM_STS_RX_FIFO_FULL_SHIFT))
#define WiFiSPIM_INT_ON_RX_NOT_EMPTY    ((uint8) (0u << \
                                                                          WiFiSPIM_STS_RX_FIFO_NOT_EMPTY_SHIFT))
#define WiFiSPIM_INT_ON_RX_OVER         ((uint8) (0u << \
                                                                          WiFiSPIM_STS_RX_FIFO_OVERRUN_SHIFT))

/* RX interrupt mask */
#define WiFiSPIM_RX_INIT_INTERRUPTS_MASK    (WiFiSPIM_INT_ON_RX_FULL      | \
                                                     WiFiSPIM_INT_ON_RX_NOT_EMPTY | \
                                                     WiFiSPIM_INT_ON_RX_OVER)
/* Nubmer of bits to receive/transmit */
#define WiFiSPIM_BITCTR_INIT            (((uint8) (WiFiSPIM_DATA_WIDTH << 1u)) - 1u)


/***************************************
*             Registers
***************************************/
#if(CY_PSOC3 || CY_PSOC5)
    #define WiFiSPIM_TXDATA_REG (* (reg8 *) \
                                                WiFiSPIM_BSPIM_sR8_Dp_u0__F0_REG)
    #define WiFiSPIM_TXDATA_PTR (  (reg8 *) \
                                                WiFiSPIM_BSPIM_sR8_Dp_u0__F0_REG)
    #define WiFiSPIM_RXDATA_REG (* (reg8 *) \
                                                WiFiSPIM_BSPIM_sR8_Dp_u0__F1_REG)
    #define WiFiSPIM_RXDATA_PTR (  (reg8 *) \
                                                WiFiSPIM_BSPIM_sR8_Dp_u0__F1_REG)
#else   /* PSOC4 */
    #if(WiFiSPIM_USE_SECOND_DATAPATH)
        #define WiFiSPIM_TXDATA_REG (* (reg16 *) \
                                          WiFiSPIM_BSPIM_sR8_Dp_u0__16BIT_F0_REG)
        #define WiFiSPIM_TXDATA_PTR (  (reg16 *) \
                                          WiFiSPIM_BSPIM_sR8_Dp_u0__16BIT_F0_REG)
        #define WiFiSPIM_RXDATA_REG (* (reg16 *) \
                                          WiFiSPIM_BSPIM_sR8_Dp_u0__16BIT_F1_REG)
        #define WiFiSPIM_RXDATA_PTR (  (reg16 *) \
                                          WiFiSPIM_BSPIM_sR8_Dp_u0__16BIT_F1_REG)
    #else
        #define WiFiSPIM_TXDATA_REG (* (reg8 *) \
                                                WiFiSPIM_BSPIM_sR8_Dp_u0__F0_REG)
        #define WiFiSPIM_TXDATA_PTR (  (reg8 *) \
                                                WiFiSPIM_BSPIM_sR8_Dp_u0__F0_REG)
        #define WiFiSPIM_RXDATA_REG (* (reg8 *) \
                                                WiFiSPIM_BSPIM_sR8_Dp_u0__F1_REG)
        #define WiFiSPIM_RXDATA_PTR (  (reg8 *) \
                                                WiFiSPIM_BSPIM_sR8_Dp_u0__F1_REG)
    #endif /* (WiFiSPIM_USE_SECOND_DATAPATH) */
#endif     /* (CY_PSOC3 || CY_PSOC5) */

#define WiFiSPIM_AUX_CONTROL_DP0_REG (* (reg8 *) \
                                        WiFiSPIM_BSPIM_sR8_Dp_u0__DP_AUX_CTL_REG)
#define WiFiSPIM_AUX_CONTROL_DP0_PTR (  (reg8 *) \
                                        WiFiSPIM_BSPIM_sR8_Dp_u0__DP_AUX_CTL_REG)

#if(WiFiSPIM_USE_SECOND_DATAPATH)
    #define WiFiSPIM_AUX_CONTROL_DP1_REG  (* (reg8 *) \
                                        WiFiSPIM_BSPIM_sR8_Dp_u1__DP_AUX_CTL_REG)
    #define WiFiSPIM_AUX_CONTROL_DP1_PTR  (  (reg8 *) \
                                        WiFiSPIM_BSPIM_sR8_Dp_u1__DP_AUX_CTL_REG)
#endif /* (WiFiSPIM_USE_SECOND_DATAPATH) */

#define WiFiSPIM_COUNTER_PERIOD_REG     (* (reg8 *) WiFiSPIM_BSPIM_BitCounter__PERIOD_REG)
#define WiFiSPIM_COUNTER_PERIOD_PTR     (  (reg8 *) WiFiSPIM_BSPIM_BitCounter__PERIOD_REG)
#define WiFiSPIM_COUNTER_CONTROL_REG    (* (reg8 *) WiFiSPIM_BSPIM_BitCounter__CONTROL_AUX_CTL_REG)
#define WiFiSPIM_COUNTER_CONTROL_PTR    (  (reg8 *) WiFiSPIM_BSPIM_BitCounter__CONTROL_AUX_CTL_REG)

#define WiFiSPIM_TX_STATUS_REG          (* (reg8 *) WiFiSPIM_BSPIM_TxStsReg__STATUS_REG)
#define WiFiSPIM_TX_STATUS_PTR          (  (reg8 *) WiFiSPIM_BSPIM_TxStsReg__STATUS_REG)
#define WiFiSPIM_RX_STATUS_REG          (* (reg8 *) WiFiSPIM_BSPIM_RxStsReg__STATUS_REG)
#define WiFiSPIM_RX_STATUS_PTR          (  (reg8 *) WiFiSPIM_BSPIM_RxStsReg__STATUS_REG)

#define WiFiSPIM_CONTROL_REG            (* (reg8 *) \
                                      WiFiSPIM_BSPIM_BidirMode_CtrlReg__CONTROL_REG)
#define WiFiSPIM_CONTROL_PTR            (  (reg8 *) \
                                      WiFiSPIM_BSPIM_BidirMode_CtrlReg__CONTROL_REG)

#define WiFiSPIM_TX_STATUS_MASK_REG     (* (reg8 *) WiFiSPIM_BSPIM_TxStsReg__MASK_REG)
#define WiFiSPIM_TX_STATUS_MASK_PTR     (  (reg8 *) WiFiSPIM_BSPIM_TxStsReg__MASK_REG)
#define WiFiSPIM_RX_STATUS_MASK_REG     (* (reg8 *) WiFiSPIM_BSPIM_RxStsReg__MASK_REG)
#define WiFiSPIM_RX_STATUS_MASK_PTR     (  (reg8 *) WiFiSPIM_BSPIM_RxStsReg__MASK_REG)

#define WiFiSPIM_TX_STATUS_ACTL_REG     (* (reg8 *) WiFiSPIM_BSPIM_TxStsReg__STATUS_AUX_CTL_REG)
#define WiFiSPIM_TX_STATUS_ACTL_PTR     (  (reg8 *) WiFiSPIM_BSPIM_TxStsReg__STATUS_AUX_CTL_REG)
#define WiFiSPIM_RX_STATUS_ACTL_REG     (* (reg8 *) WiFiSPIM_BSPIM_RxStsReg__STATUS_AUX_CTL_REG)
#define WiFiSPIM_RX_STATUS_ACTL_PTR     (  (reg8 *) WiFiSPIM_BSPIM_RxStsReg__STATUS_AUX_CTL_REG)

#if(WiFiSPIM_USE_SECOND_DATAPATH)
    #define WiFiSPIM_AUX_CONTROLDP1     (WiFiSPIM_AUX_CONTROL_DP1_REG)
#endif /* (WiFiSPIM_USE_SECOND_DATAPATH) */


/***************************************
*       Register Constants
***************************************/

/* Status Register Definitions */
#define WiFiSPIM_STS_SPI_DONE_SHIFT             (0x00u)
#define WiFiSPIM_STS_TX_FIFO_EMPTY_SHIFT        (0x01u)
#define WiFiSPIM_STS_TX_FIFO_NOT_FULL_SHIFT     (0x02u)
#define WiFiSPIM_STS_BYTE_COMPLETE_SHIFT        (0x03u)
#define WiFiSPIM_STS_SPI_IDLE_SHIFT             (0x04u)
#define WiFiSPIM_STS_RX_FIFO_FULL_SHIFT         (0x04u)
#define WiFiSPIM_STS_RX_FIFO_NOT_EMPTY_SHIFT    (0x05u)
#define WiFiSPIM_STS_RX_FIFO_OVERRUN_SHIFT      (0x06u)

#define WiFiSPIM_STS_SPI_DONE           ((uint8) (0x01u << WiFiSPIM_STS_SPI_DONE_SHIFT))
#define WiFiSPIM_STS_TX_FIFO_EMPTY      ((uint8) (0x01u << WiFiSPIM_STS_TX_FIFO_EMPTY_SHIFT))
#define WiFiSPIM_STS_TX_FIFO_NOT_FULL   ((uint8) (0x01u << WiFiSPIM_STS_TX_FIFO_NOT_FULL_SHIFT))
#define WiFiSPIM_STS_BYTE_COMPLETE      ((uint8) (0x01u << WiFiSPIM_STS_BYTE_COMPLETE_SHIFT))
#define WiFiSPIM_STS_SPI_IDLE           ((uint8) (0x01u << WiFiSPIM_STS_SPI_IDLE_SHIFT))
#define WiFiSPIM_STS_RX_FIFO_FULL       ((uint8) (0x01u << WiFiSPIM_STS_RX_FIFO_FULL_SHIFT))
#define WiFiSPIM_STS_RX_FIFO_NOT_EMPTY  ((uint8) (0x01u << WiFiSPIM_STS_RX_FIFO_NOT_EMPTY_SHIFT))
#define WiFiSPIM_STS_RX_FIFO_OVERRUN    ((uint8) (0x01u << WiFiSPIM_STS_RX_FIFO_OVERRUN_SHIFT))

/* TX and RX masks for clear on read bits */
#define WiFiSPIM_TX_STS_CLR_ON_RD_BYTES_MASK    (0x09u)
#define WiFiSPIM_RX_STS_CLR_ON_RD_BYTES_MASK    (0x40u)

/* StatusI Register Interrupt Enable Control Bits */
/* As defined by the Register map for the AUX Control Register */
#define WiFiSPIM_INT_ENABLE     (0x10u) /* Enable interrupt from statusi */
#define WiFiSPIM_TX_FIFO_CLR    (0x01u) /* F0 - TX FIFO */
#define WiFiSPIM_RX_FIFO_CLR    (0x02u) /* F1 - RX FIFO */
#define WiFiSPIM_FIFO_CLR       (WiFiSPIM_TX_FIFO_CLR | WiFiSPIM_RX_FIFO_CLR)

/* Bit Counter (7-bit) Control Register Bit Definitions */
/* As defined by the Register map for the AUX Control Register */
#define WiFiSPIM_CNTR_ENABLE    (0x20u) /* Enable CNT7 */

/* Bi-Directional mode control bit */
#define WiFiSPIM_CTRL_TX_SIGNAL_EN  (0x01u)

/* Datapath Auxillary Control Register definitions */
#define WiFiSPIM_AUX_CTRL_FIFO0_CLR         (0x01u)
#define WiFiSPIM_AUX_CTRL_FIFO1_CLR         (0x02u)
#define WiFiSPIM_AUX_CTRL_FIFO0_LVL         (0x04u)
#define WiFiSPIM_AUX_CTRL_FIFO1_LVL         (0x08u)
#define WiFiSPIM_STATUS_ACTL_INT_EN_MASK    (0x10u)

/* Component disabled */
#define WiFiSPIM_DISABLED   (0u)


/***************************************
*       Macros
***************************************/

/* Returns true if componentn enabled */
#define WiFiSPIM_IS_ENABLED (0u != (WiFiSPIM_TX_STATUS_ACTL_REG & WiFiSPIM_INT_ENABLE))

/* Retuns TX status register */
#define WiFiSPIM_GET_STATUS_TX(swTxSts) ( (uint8)(WiFiSPIM_TX_STATUS_REG | \
                                                          ((swTxSts) & WiFiSPIM_TX_STS_CLR_ON_RD_BYTES_MASK)) )
/* Retuns RX status register */
#define WiFiSPIM_GET_STATUS_RX(swRxSts) ( (uint8)(WiFiSPIM_RX_STATUS_REG | \
                                                          ((swRxSts) & WiFiSPIM_RX_STS_CLR_ON_RD_BYTES_MASK)) )


/***************************************
* The following code is DEPRECATED and 
* should not be used in new projects.
***************************************/

#define WiFiSPIM_WriteByte   WiFiSPIM_WriteTxData
#define WiFiSPIM_ReadByte    WiFiSPIM_ReadRxData
void  WiFiSPIM_SetInterruptMode(uint8 intSrc)       ;
uint8 WiFiSPIM_ReadStatus(void)                     ;
void  WiFiSPIM_EnableInt(void)                      ;
void  WiFiSPIM_DisableInt(void)                     ;

#define WiFiSPIM_TXDATA                 (WiFiSPIM_TXDATA_REG)
#define WiFiSPIM_RXDATA                 (WiFiSPIM_RXDATA_REG)
#define WiFiSPIM_AUX_CONTROLDP0         (WiFiSPIM_AUX_CONTROL_DP0_REG)
#define WiFiSPIM_TXBUFFERREAD           (WiFiSPIM_txBufferRead)
#define WiFiSPIM_TXBUFFERWRITE          (WiFiSPIM_txBufferWrite)
#define WiFiSPIM_RXBUFFERREAD           (WiFiSPIM_rxBufferRead)
#define WiFiSPIM_RXBUFFERWRITE          (WiFiSPIM_rxBufferWrite)

#define WiFiSPIM_COUNTER_PERIOD         (WiFiSPIM_COUNTER_PERIOD_REG)
#define WiFiSPIM_COUNTER_CONTROL        (WiFiSPIM_COUNTER_CONTROL_REG)
#define WiFiSPIM_STATUS                 (WiFiSPIM_TX_STATUS_REG)
#define WiFiSPIM_CONTROL                (WiFiSPIM_CONTROL_REG)
#define WiFiSPIM_STATUS_MASK            (WiFiSPIM_TX_STATUS_MASK_REG)
#define WiFiSPIM_STATUS_ACTL            (WiFiSPIM_TX_STATUS_ACTL_REG)

#define WiFiSPIM_INIT_INTERRUPTS_MASK  (WiFiSPIM_INT_ON_SPI_DONE     | \
                                                WiFiSPIM_INT_ON_TX_EMPTY     | \
                                                WiFiSPIM_INT_ON_TX_NOT_FULL_DEF  | \
                                                WiFiSPIM_INT_ON_RX_FULL      | \
                                                WiFiSPIM_INT_ON_RX_NOT_EMPTY | \
                                                WiFiSPIM_INT_ON_RX_OVER      | \
                                                WiFiSPIM_INT_ON_BYTE_COMP)
                                                
#define WiFiSPIM_DataWidth                  (WiFiSPIM_DATA_WIDTH)
#define WiFiSPIM_InternalClockUsed          (WiFiSPIM_INTERNAL_CLOCK)
#define WiFiSPIM_InternalTxInterruptEnabled (WiFiSPIM_INTERNAL_TX_INT_ENABLED)
#define WiFiSPIM_InternalRxInterruptEnabled (WiFiSPIM_INTERNAL_RX_INT_ENABLED)
#define WiFiSPIM_ModeUseZero                (WiFiSPIM_MODE_USE_ZERO)
#define WiFiSPIM_BidirectionalMode          (WiFiSPIM_BIDIRECTIONAL_MODE)
#define WiFiSPIM_Mode                       (WiFiSPIM_MODE)
#define WiFiSPIM_DATAWIDHT                  (WiFiSPIM_DATA_WIDTH)
#define WiFiSPIM_InternalInterruptEnabled   (0u)

#define WiFiSPIM_TXBUFFERSIZE   (WiFiSPIM_TX_BUFFER_SIZE)
#define WiFiSPIM_RXBUFFERSIZE   (WiFiSPIM_RX_BUFFER_SIZE)

#define WiFiSPIM_TXBUFFER       WiFiSPIM_txBuffer
#define WiFiSPIM_RXBUFFER       WiFiSPIM_rxBuffer

#endif /* (CY_SPIM_WiFiSPIM_H) */


/* [] END OF FILE */
