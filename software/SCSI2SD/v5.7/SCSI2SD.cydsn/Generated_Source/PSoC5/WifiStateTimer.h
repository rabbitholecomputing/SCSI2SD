/*******************************************************************************
* File Name: WifiStateTimer.h
* Version 2.80
*
*  Description:
*     Contains the function prototypes and constants available to the timer
*     user module.
*
*   Note:
*     None
*
********************************************************************************
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CY_TIMER_WifiStateTimer_H)
#define CY_TIMER_WifiStateTimer_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */

extern uint8 WifiStateTimer_initVar;

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component Timer_v2_80 requires cy_boot v3.0 or later
#endif /* (CY_ PSOC5LP) */


/**************************************
*           Parameter Defaults
**************************************/

#define WifiStateTimer_Resolution                 16u
#define WifiStateTimer_UsingFixedFunction         1u
#define WifiStateTimer_UsingHWCaptureCounter      0u
#define WifiStateTimer_SoftwareCaptureMode        0u
#define WifiStateTimer_SoftwareTriggerMode        0u
#define WifiStateTimer_UsingHWEnable              0u
#define WifiStateTimer_EnableTriggerMode          0u
#define WifiStateTimer_InterruptOnCaptureCount    0u
#define WifiStateTimer_RunModeUsed                0u
#define WifiStateTimer_ControlRegRemoved          0u

#if defined(WifiStateTimer_TimerUDB_sCTRLReg_SyncCtl_ctrlreg__CONTROL_REG)
    #define WifiStateTimer_UDB_CONTROL_REG_REMOVED            (0u)
#elif  (WifiStateTimer_UsingFixedFunction)
    #define WifiStateTimer_UDB_CONTROL_REG_REMOVED            (0u)
#else 
    #define WifiStateTimer_UDB_CONTROL_REG_REMOVED            (1u)
#endif /* End WifiStateTimer_TimerUDB_sCTRLReg_SyncCtl_ctrlreg__CONTROL_REG */


/***************************************
*       Type defines
***************************************/


/**************************************************************************
 * Sleep Wakeup Backup structure for Timer Component
 *************************************************************************/
typedef struct
{
    uint8 TimerEnableState;
    #if(!WifiStateTimer_UsingFixedFunction)

        uint16 TimerUdb;
        uint8 InterruptMaskValue;
        #if (WifiStateTimer_UsingHWCaptureCounter)
            uint8 TimerCaptureCounter;
        #endif /* variable declarations for backing up non retention registers in CY_UDB_V1 */

        #if (!WifiStateTimer_UDB_CONTROL_REG_REMOVED)
            uint8 TimerControlRegister;
        #endif /* variable declaration for backing up enable state of the Timer */
    #endif /* define backup variables only for UDB implementation. Fixed function registers are all retention */

}WifiStateTimer_backupStruct;


/***************************************
*       Function Prototypes
***************************************/

void    WifiStateTimer_Start(void) ;
void    WifiStateTimer_Stop(void) ;

void    WifiStateTimer_SetInterruptMode(uint8 interruptMode) ;
uint8   WifiStateTimer_ReadStatusRegister(void) ;
/* Deprecated function. Do not use this in future. Retained for backward compatibility */
#define WifiStateTimer_GetInterruptSource() WifiStateTimer_ReadStatusRegister()

#if(!WifiStateTimer_UDB_CONTROL_REG_REMOVED)
    uint8   WifiStateTimer_ReadControlRegister(void) ;
    void    WifiStateTimer_WriteControlRegister(uint8 control) ;
#endif /* (!WifiStateTimer_UDB_CONTROL_REG_REMOVED) */

uint16  WifiStateTimer_ReadPeriod(void) ;
void    WifiStateTimer_WritePeriod(uint16 period) ;
uint16  WifiStateTimer_ReadCounter(void) ;
void    WifiStateTimer_WriteCounter(uint16 counter) ;
uint16  WifiStateTimer_ReadCapture(void) ;
void    WifiStateTimer_SoftwareCapture(void) ;

#if(!WifiStateTimer_UsingFixedFunction) /* UDB Prototypes */
    #if (WifiStateTimer_SoftwareCaptureMode)
        void    WifiStateTimer_SetCaptureMode(uint8 captureMode) ;
    #endif /* (!WifiStateTimer_UsingFixedFunction) */

    #if (WifiStateTimer_SoftwareTriggerMode)
        void    WifiStateTimer_SetTriggerMode(uint8 triggerMode) ;
    #endif /* (WifiStateTimer_SoftwareTriggerMode) */

    #if (WifiStateTimer_EnableTriggerMode)
        void    WifiStateTimer_EnableTrigger(void) ;
        void    WifiStateTimer_DisableTrigger(void) ;
    #endif /* (WifiStateTimer_EnableTriggerMode) */


    #if(WifiStateTimer_InterruptOnCaptureCount)
        void    WifiStateTimer_SetInterruptCount(uint8 interruptCount) ;
    #endif /* (WifiStateTimer_InterruptOnCaptureCount) */

    #if (WifiStateTimer_UsingHWCaptureCounter)
        void    WifiStateTimer_SetCaptureCount(uint8 captureCount) ;
        uint8   WifiStateTimer_ReadCaptureCount(void) ;
    #endif /* (WifiStateTimer_UsingHWCaptureCounter) */

    void WifiStateTimer_ClearFIFO(void) ;
#endif /* UDB Prototypes */

/* Sleep Retention APIs */
void WifiStateTimer_Init(void)          ;
void WifiStateTimer_Enable(void)        ;
void WifiStateTimer_SaveConfig(void)    ;
void WifiStateTimer_RestoreConfig(void) ;
void WifiStateTimer_Sleep(void)         ;
void WifiStateTimer_Wakeup(void)        ;


/***************************************
*   Enumerated Types and Parameters
***************************************/

/* Enumerated Type B_Timer__CaptureModes, Used in Capture Mode */
#define WifiStateTimer__B_TIMER__CM_NONE 0
#define WifiStateTimer__B_TIMER__CM_RISINGEDGE 1
#define WifiStateTimer__B_TIMER__CM_FALLINGEDGE 2
#define WifiStateTimer__B_TIMER__CM_EITHEREDGE 3
#define WifiStateTimer__B_TIMER__CM_SOFTWARE 4



/* Enumerated Type B_Timer__TriggerModes, Used in Trigger Mode */
#define WifiStateTimer__B_TIMER__TM_NONE 0x00u
#define WifiStateTimer__B_TIMER__TM_RISINGEDGE 0x04u
#define WifiStateTimer__B_TIMER__TM_FALLINGEDGE 0x08u
#define WifiStateTimer__B_TIMER__TM_EITHEREDGE 0x0Cu
#define WifiStateTimer__B_TIMER__TM_SOFTWARE 0x10u


/***************************************
*    Initialial Parameter Constants
***************************************/

#define WifiStateTimer_INIT_PERIOD             49999u
#define WifiStateTimer_INIT_CAPTURE_MODE       ((uint8)((uint8)1u << WifiStateTimer_CTRL_CAP_MODE_SHIFT))
#define WifiStateTimer_INIT_TRIGGER_MODE       ((uint8)((uint8)0u << WifiStateTimer_CTRL_TRIG_MODE_SHIFT))
#if (WifiStateTimer_UsingFixedFunction)
    #define WifiStateTimer_INIT_INTERRUPT_MODE (((uint8)((uint8)1u << WifiStateTimer_STATUS_TC_INT_MASK_SHIFT)) | \
                                                  ((uint8)((uint8)0 << WifiStateTimer_STATUS_CAPTURE_INT_MASK_SHIFT)))
#else
    #define WifiStateTimer_INIT_INTERRUPT_MODE (((uint8)((uint8)1u << WifiStateTimer_STATUS_TC_INT_MASK_SHIFT)) | \
                                                 ((uint8)((uint8)0 << WifiStateTimer_STATUS_CAPTURE_INT_MASK_SHIFT)) | \
                                                 ((uint8)((uint8)0 << WifiStateTimer_STATUS_FIFOFULL_INT_MASK_SHIFT)))
#endif /* (WifiStateTimer_UsingFixedFunction) */
#define WifiStateTimer_INIT_CAPTURE_COUNT      (2u)
#define WifiStateTimer_INIT_INT_CAPTURE_COUNT  ((uint8)((uint8)(1u - 1u) << WifiStateTimer_CTRL_INTCNT_SHIFT))


/***************************************
*           Registers
***************************************/

#if (WifiStateTimer_UsingFixedFunction) /* Implementation Specific Registers and Register Constants */


    /***************************************
    *    Fixed Function Registers
    ***************************************/

    #define WifiStateTimer_STATUS         (*(reg8 *) WifiStateTimer_TimerHW__SR0 )
    /* In Fixed Function Block Status and Mask are the same register */
    #define WifiStateTimer_STATUS_MASK    (*(reg8 *) WifiStateTimer_TimerHW__SR0 )
    #define WifiStateTimer_CONTROL        (*(reg8 *) WifiStateTimer_TimerHW__CFG0)
    #define WifiStateTimer_CONTROL2       (*(reg8 *) WifiStateTimer_TimerHW__CFG1)
    #define WifiStateTimer_CONTROL2_PTR   ( (reg8 *) WifiStateTimer_TimerHW__CFG1)
    #define WifiStateTimer_RT1            (*(reg8 *) WifiStateTimer_TimerHW__RT1)
    #define WifiStateTimer_RT1_PTR        ( (reg8 *) WifiStateTimer_TimerHW__RT1)

    #if (CY_PSOC3 || CY_PSOC5LP)
        #define WifiStateTimer_CONTROL3       (*(reg8 *) WifiStateTimer_TimerHW__CFG2)
        #define WifiStateTimer_CONTROL3_PTR   ( (reg8 *) WifiStateTimer_TimerHW__CFG2)
    #endif /* (CY_PSOC3 || CY_PSOC5LP) */
    #define WifiStateTimer_GLOBAL_ENABLE  (*(reg8 *) WifiStateTimer_TimerHW__PM_ACT_CFG)
    #define WifiStateTimer_GLOBAL_STBY_ENABLE  (*(reg8 *) WifiStateTimer_TimerHW__PM_STBY_CFG)

    #define WifiStateTimer_CAPTURE_LSB         (* (reg16 *) WifiStateTimer_TimerHW__CAP0 )
    #define WifiStateTimer_CAPTURE_LSB_PTR       ((reg16 *) WifiStateTimer_TimerHW__CAP0 )
    #define WifiStateTimer_PERIOD_LSB          (* (reg16 *) WifiStateTimer_TimerHW__PER0 )
    #define WifiStateTimer_PERIOD_LSB_PTR        ((reg16 *) WifiStateTimer_TimerHW__PER0 )
    #define WifiStateTimer_COUNTER_LSB         (* (reg16 *) WifiStateTimer_TimerHW__CNT_CMP0 )
    #define WifiStateTimer_COUNTER_LSB_PTR       ((reg16 *) WifiStateTimer_TimerHW__CNT_CMP0 )


    /***************************************
    *    Register Constants
    ***************************************/

    /* Fixed Function Block Chosen */
    #define WifiStateTimer_BLOCK_EN_MASK                     WifiStateTimer_TimerHW__PM_ACT_MSK
    #define WifiStateTimer_BLOCK_STBY_EN_MASK                WifiStateTimer_TimerHW__PM_STBY_MSK

    /* Control Register Bit Locations */
    /* Interrupt Count - Not valid for Fixed Function Block */
    #define WifiStateTimer_CTRL_INTCNT_SHIFT                  0x00u
    /* Trigger Polarity - Not valid for Fixed Function Block */
    #define WifiStateTimer_CTRL_TRIG_MODE_SHIFT               0x00u
    /* Trigger Enable - Not valid for Fixed Function Block */
    #define WifiStateTimer_CTRL_TRIG_EN_SHIFT                 0x00u
    /* Capture Polarity - Not valid for Fixed Function Block */
    #define WifiStateTimer_CTRL_CAP_MODE_SHIFT                0x00u
    /* Timer Enable - As defined in Register Map, part of TMRX_CFG0 register */
    #define WifiStateTimer_CTRL_ENABLE_SHIFT                  0x00u

    /* Control Register Bit Masks */
    #define WifiStateTimer_CTRL_ENABLE                        ((uint8)((uint8)0x01u << WifiStateTimer_CTRL_ENABLE_SHIFT))

    /* Control2 Register Bit Masks */
    /* As defined in Register Map, Part of the TMRX_CFG1 register */
    #define WifiStateTimer_CTRL2_IRQ_SEL_SHIFT                 0x00u
    #define WifiStateTimer_CTRL2_IRQ_SEL                      ((uint8)((uint8)0x01u << WifiStateTimer_CTRL2_IRQ_SEL_SHIFT))

    #if (CY_PSOC5A)
        /* Use CFG1 Mode bits to set run mode */
        /* As defined by Verilog Implementation */
        #define WifiStateTimer_CTRL_MODE_SHIFT                 0x01u
        #define WifiStateTimer_CTRL_MODE_MASK                 ((uint8)((uint8)0x07u << WifiStateTimer_CTRL_MODE_SHIFT))
    #endif /* (CY_PSOC5A) */
    #if (CY_PSOC3 || CY_PSOC5LP)
        /* Control3 Register Bit Locations */
        #define WifiStateTimer_CTRL_RCOD_SHIFT        0x02u
        #define WifiStateTimer_CTRL_ENBL_SHIFT        0x00u
        #define WifiStateTimer_CTRL_MODE_SHIFT        0x00u

        /* Control3 Register Bit Masks */
        #define WifiStateTimer_CTRL_RCOD_MASK  ((uint8)((uint8)0x03u << WifiStateTimer_CTRL_RCOD_SHIFT)) /* ROD and COD bit masks */
        #define WifiStateTimer_CTRL_ENBL_MASK  ((uint8)((uint8)0x80u << WifiStateTimer_CTRL_ENBL_SHIFT)) /* HW_EN bit mask */
        #define WifiStateTimer_CTRL_MODE_MASK  ((uint8)((uint8)0x03u << WifiStateTimer_CTRL_MODE_SHIFT)) /* Run mode bit mask */

        #define WifiStateTimer_CTRL_RCOD       ((uint8)((uint8)0x03u << WifiStateTimer_CTRL_RCOD_SHIFT))
        #define WifiStateTimer_CTRL_ENBL       ((uint8)((uint8)0x80u << WifiStateTimer_CTRL_ENBL_SHIFT))
    #endif /* (CY_PSOC3 || CY_PSOC5LP) */

    /*RT1 Synch Constants: Applicable for PSoC3 and PSoC5LP */
    #define WifiStateTimer_RT1_SHIFT                       0x04u
    /* Sync TC and CMP bit masks */
    #define WifiStateTimer_RT1_MASK                        ((uint8)((uint8)0x03u << WifiStateTimer_RT1_SHIFT))
    #define WifiStateTimer_SYNC                            ((uint8)((uint8)0x03u << WifiStateTimer_RT1_SHIFT))
    #define WifiStateTimer_SYNCDSI_SHIFT                   0x00u
    /* Sync all DSI inputs with Mask  */
    #define WifiStateTimer_SYNCDSI_MASK                    ((uint8)((uint8)0x0Fu << WifiStateTimer_SYNCDSI_SHIFT))
    /* Sync all DSI inputs */
    #define WifiStateTimer_SYNCDSI_EN                      ((uint8)((uint8)0x0Fu << WifiStateTimer_SYNCDSI_SHIFT))

    #define WifiStateTimer_CTRL_MODE_PULSEWIDTH            ((uint8)((uint8)0x01u << WifiStateTimer_CTRL_MODE_SHIFT))
    #define WifiStateTimer_CTRL_MODE_PERIOD                ((uint8)((uint8)0x02u << WifiStateTimer_CTRL_MODE_SHIFT))
    #define WifiStateTimer_CTRL_MODE_CONTINUOUS            ((uint8)((uint8)0x00u << WifiStateTimer_CTRL_MODE_SHIFT))

    /* Status Register Bit Locations */
    /* As defined in Register Map, part of TMRX_SR0 register */
    #define WifiStateTimer_STATUS_TC_SHIFT                 0x07u
    /* As defined in Register Map, part of TMRX_SR0 register, Shared with Compare Status */
    #define WifiStateTimer_STATUS_CAPTURE_SHIFT            0x06u
    /* As defined in Register Map, part of TMRX_SR0 register */
    #define WifiStateTimer_STATUS_TC_INT_MASK_SHIFT        (WifiStateTimer_STATUS_TC_SHIFT - 0x04u)
    /* As defined in Register Map, part of TMRX_SR0 register, Shared with Compare Status */
    #define WifiStateTimer_STATUS_CAPTURE_INT_MASK_SHIFT   (WifiStateTimer_STATUS_CAPTURE_SHIFT - 0x04u)

    /* Status Register Bit Masks */
    #define WifiStateTimer_STATUS_TC                       ((uint8)((uint8)0x01u << WifiStateTimer_STATUS_TC_SHIFT))
    #define WifiStateTimer_STATUS_CAPTURE                  ((uint8)((uint8)0x01u << WifiStateTimer_STATUS_CAPTURE_SHIFT))
    /* Interrupt Enable Bit-Mask for interrupt on TC */
    #define WifiStateTimer_STATUS_TC_INT_MASK              ((uint8)((uint8)0x01u << WifiStateTimer_STATUS_TC_INT_MASK_SHIFT))
    /* Interrupt Enable Bit-Mask for interrupt on Capture */
    #define WifiStateTimer_STATUS_CAPTURE_INT_MASK         ((uint8)((uint8)0x01u << WifiStateTimer_STATUS_CAPTURE_INT_MASK_SHIFT))

#else   /* UDB Registers and Register Constants */


    /***************************************
    *           UDB Registers
    ***************************************/

    #define WifiStateTimer_STATUS              (* (reg8 *) WifiStateTimer_TimerUDB_rstSts_stsreg__STATUS_REG )
    #define WifiStateTimer_STATUS_MASK         (* (reg8 *) WifiStateTimer_TimerUDB_rstSts_stsreg__MASK_REG)
    #define WifiStateTimer_STATUS_AUX_CTRL     (* (reg8 *) WifiStateTimer_TimerUDB_rstSts_stsreg__STATUS_AUX_CTL_REG)
    #define WifiStateTimer_CONTROL             (* (reg8 *) WifiStateTimer_TimerUDB_sCTRLReg_SyncCtl_ctrlreg__CONTROL_REG )
    
    #if(WifiStateTimer_Resolution <= 8u) /* 8-bit Timer */
        #define WifiStateTimer_CAPTURE_LSB         (* (reg8 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__F0_REG )
        #define WifiStateTimer_CAPTURE_LSB_PTR       ((reg8 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__F0_REG )
        #define WifiStateTimer_PERIOD_LSB          (* (reg8 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__D0_REG )
        #define WifiStateTimer_PERIOD_LSB_PTR        ((reg8 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__D0_REG )
        #define WifiStateTimer_COUNTER_LSB         (* (reg8 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
        #define WifiStateTimer_COUNTER_LSB_PTR       ((reg8 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
    #elif(WifiStateTimer_Resolution <= 16u) /* 8-bit Timer */
        #if(CY_PSOC3) /* 8-bit addres space */
            #define WifiStateTimer_CAPTURE_LSB         (* (reg16 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__F0_REG )
            #define WifiStateTimer_CAPTURE_LSB_PTR       ((reg16 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__F0_REG )
            #define WifiStateTimer_PERIOD_LSB          (* (reg16 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__D0_REG )
            #define WifiStateTimer_PERIOD_LSB_PTR        ((reg16 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__D0_REG )
            #define WifiStateTimer_COUNTER_LSB         (* (reg16 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
            #define WifiStateTimer_COUNTER_LSB_PTR       ((reg16 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
        #else /* 16-bit address space */
            #define WifiStateTimer_CAPTURE_LSB         (* (reg16 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__16BIT_F0_REG )
            #define WifiStateTimer_CAPTURE_LSB_PTR       ((reg16 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__16BIT_F0_REG )
            #define WifiStateTimer_PERIOD_LSB          (* (reg16 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__16BIT_D0_REG )
            #define WifiStateTimer_PERIOD_LSB_PTR        ((reg16 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__16BIT_D0_REG )
            #define WifiStateTimer_COUNTER_LSB         (* (reg16 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__16BIT_A0_REG )
            #define WifiStateTimer_COUNTER_LSB_PTR       ((reg16 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__16BIT_A0_REG )
        #endif /* CY_PSOC3 */
    #elif(WifiStateTimer_Resolution <= 24u)/* 24-bit Timer */
        #define WifiStateTimer_CAPTURE_LSB         (* (reg32 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__F0_REG )
        #define WifiStateTimer_CAPTURE_LSB_PTR       ((reg32 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__F0_REG )
        #define WifiStateTimer_PERIOD_LSB          (* (reg32 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__D0_REG )
        #define WifiStateTimer_PERIOD_LSB_PTR        ((reg32 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__D0_REG )
        #define WifiStateTimer_COUNTER_LSB         (* (reg32 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
        #define WifiStateTimer_COUNTER_LSB_PTR       ((reg32 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
    #else /* 32-bit Timer */
        #if(CY_PSOC3 || CY_PSOC5) /* 8-bit address space */
            #define WifiStateTimer_CAPTURE_LSB         (* (reg32 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__F0_REG )
            #define WifiStateTimer_CAPTURE_LSB_PTR       ((reg32 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__F0_REG )
            #define WifiStateTimer_PERIOD_LSB          (* (reg32 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__D0_REG )
            #define WifiStateTimer_PERIOD_LSB_PTR        ((reg32 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__D0_REG )
            #define WifiStateTimer_COUNTER_LSB         (* (reg32 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
            #define WifiStateTimer_COUNTER_LSB_PTR       ((reg32 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
        #else /* 32-bit address space */
            #define WifiStateTimer_CAPTURE_LSB         (* (reg32 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__32BIT_F0_REG )
            #define WifiStateTimer_CAPTURE_LSB_PTR       ((reg32 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__32BIT_F0_REG )
            #define WifiStateTimer_PERIOD_LSB          (* (reg32 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__32BIT_D0_REG )
            #define WifiStateTimer_PERIOD_LSB_PTR        ((reg32 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__32BIT_D0_REG )
            #define WifiStateTimer_COUNTER_LSB         (* (reg32 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__32BIT_A0_REG )
            #define WifiStateTimer_COUNTER_LSB_PTR       ((reg32 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__32BIT_A0_REG )
        #endif /* CY_PSOC3 || CY_PSOC5 */ 
    #endif

    #define WifiStateTimer_COUNTER_LSB_PTR_8BIT       ((reg8 *) WifiStateTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
    
    #if (WifiStateTimer_UsingHWCaptureCounter)
        #define WifiStateTimer_CAP_COUNT              (*(reg8 *) WifiStateTimer_TimerUDB_sCapCount_counter__PERIOD_REG )
        #define WifiStateTimer_CAP_COUNT_PTR          ( (reg8 *) WifiStateTimer_TimerUDB_sCapCount_counter__PERIOD_REG )
        #define WifiStateTimer_CAPTURE_COUNT_CTRL     (*(reg8 *) WifiStateTimer_TimerUDB_sCapCount_counter__CONTROL_AUX_CTL_REG )
        #define WifiStateTimer_CAPTURE_COUNT_CTRL_PTR ( (reg8 *) WifiStateTimer_TimerUDB_sCapCount_counter__CONTROL_AUX_CTL_REG )
    #endif /* (WifiStateTimer_UsingHWCaptureCounter) */


    /***************************************
    *       Register Constants
    ***************************************/

    /* Control Register Bit Locations */
    #define WifiStateTimer_CTRL_INTCNT_SHIFT              0x00u       /* As defined by Verilog Implementation */
    #define WifiStateTimer_CTRL_TRIG_MODE_SHIFT           0x02u       /* As defined by Verilog Implementation */
    #define WifiStateTimer_CTRL_TRIG_EN_SHIFT             0x04u       /* As defined by Verilog Implementation */
    #define WifiStateTimer_CTRL_CAP_MODE_SHIFT            0x05u       /* As defined by Verilog Implementation */
    #define WifiStateTimer_CTRL_ENABLE_SHIFT              0x07u       /* As defined by Verilog Implementation */

    /* Control Register Bit Masks */
    #define WifiStateTimer_CTRL_INTCNT_MASK               ((uint8)((uint8)0x03u << WifiStateTimer_CTRL_INTCNT_SHIFT))
    #define WifiStateTimer_CTRL_TRIG_MODE_MASK            ((uint8)((uint8)0x03u << WifiStateTimer_CTRL_TRIG_MODE_SHIFT))
    #define WifiStateTimer_CTRL_TRIG_EN                   ((uint8)((uint8)0x01u << WifiStateTimer_CTRL_TRIG_EN_SHIFT))
    #define WifiStateTimer_CTRL_CAP_MODE_MASK             ((uint8)((uint8)0x03u << WifiStateTimer_CTRL_CAP_MODE_SHIFT))
    #define WifiStateTimer_CTRL_ENABLE                    ((uint8)((uint8)0x01u << WifiStateTimer_CTRL_ENABLE_SHIFT))

    /* Bit Counter (7-bit) Control Register Bit Definitions */
    /* As defined by the Register map for the AUX Control Register */
    #define WifiStateTimer_CNTR_ENABLE                    0x20u

    /* Status Register Bit Locations */
    #define WifiStateTimer_STATUS_TC_SHIFT                0x00u  /* As defined by Verilog Implementation */
    #define WifiStateTimer_STATUS_CAPTURE_SHIFT           0x01u  /* As defined by Verilog Implementation */
    #define WifiStateTimer_STATUS_TC_INT_MASK_SHIFT       WifiStateTimer_STATUS_TC_SHIFT
    #define WifiStateTimer_STATUS_CAPTURE_INT_MASK_SHIFT  WifiStateTimer_STATUS_CAPTURE_SHIFT
    #define WifiStateTimer_STATUS_FIFOFULL_SHIFT          0x02u  /* As defined by Verilog Implementation */
    #define WifiStateTimer_STATUS_FIFONEMP_SHIFT          0x03u  /* As defined by Verilog Implementation */
    #define WifiStateTimer_STATUS_FIFOFULL_INT_MASK_SHIFT WifiStateTimer_STATUS_FIFOFULL_SHIFT

    /* Status Register Bit Masks */
    /* Sticky TC Event Bit-Mask */
    #define WifiStateTimer_STATUS_TC                      ((uint8)((uint8)0x01u << WifiStateTimer_STATUS_TC_SHIFT))
    /* Sticky Capture Event Bit-Mask */
    #define WifiStateTimer_STATUS_CAPTURE                 ((uint8)((uint8)0x01u << WifiStateTimer_STATUS_CAPTURE_SHIFT))
    /* Interrupt Enable Bit-Mask */
    #define WifiStateTimer_STATUS_TC_INT_MASK             ((uint8)((uint8)0x01u << WifiStateTimer_STATUS_TC_SHIFT))
    /* Interrupt Enable Bit-Mask */
    #define WifiStateTimer_STATUS_CAPTURE_INT_MASK        ((uint8)((uint8)0x01u << WifiStateTimer_STATUS_CAPTURE_SHIFT))
    /* NOT-Sticky FIFO Full Bit-Mask */
    #define WifiStateTimer_STATUS_FIFOFULL                ((uint8)((uint8)0x01u << WifiStateTimer_STATUS_FIFOFULL_SHIFT))
    /* NOT-Sticky FIFO Not Empty Bit-Mask */
    #define WifiStateTimer_STATUS_FIFONEMP                ((uint8)((uint8)0x01u << WifiStateTimer_STATUS_FIFONEMP_SHIFT))
    /* Interrupt Enable Bit-Mask */
    #define WifiStateTimer_STATUS_FIFOFULL_INT_MASK       ((uint8)((uint8)0x01u << WifiStateTimer_STATUS_FIFOFULL_SHIFT))

    #define WifiStateTimer_STATUS_ACTL_INT_EN             0x10u   /* As defined for the ACTL Register */

    /* Datapath Auxillary Control Register definitions */
    #define WifiStateTimer_AUX_CTRL_FIFO0_CLR             0x01u   /* As defined by Register map */
    #define WifiStateTimer_AUX_CTRL_FIFO1_CLR             0x02u   /* As defined by Register map */
    #define WifiStateTimer_AUX_CTRL_FIFO0_LVL             0x04u   /* As defined by Register map */
    #define WifiStateTimer_AUX_CTRL_FIFO1_LVL             0x08u   /* As defined by Register map */
    #define WifiStateTimer_STATUS_ACTL_INT_EN_MASK        0x10u   /* As defined for the ACTL Register */

#endif /* Implementation Specific Registers and Register Constants */

#endif  /* CY_TIMER_WifiStateTimer_H */


/* [] END OF FILE */
