/***************************************************************************//**
* \file .h
* \version 3.20
*
* \brief
*  This file provides private function prototypes and constants for the 
*  USBFS component. It is not intended to be used in the user project.
*
********************************************************************************
* \copyright
* Copyright 2013-2016, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_USBFS_USBFS_pvt_H)
#define CY_USBFS_USBFS_pvt_H

#include "USBFS.h"
   
#ifdef USBFS_ENABLE_AUDIO_CLASS
    #include "USBFS_audio.h"
#endif /* USBFS_ENABLE_AUDIO_CLASS */

#ifdef USBFS_ENABLE_CDC_CLASS
    #include "USBFS_cdc.h"
#endif /* USBFS_ENABLE_CDC_CLASS */

#if (USBFS_ENABLE_MIDI_CLASS)
    #include "USBFS_midi.h"
#endif /* (USBFS_ENABLE_MIDI_CLASS) */

#if (USBFS_ENABLE_MSC_CLASS)
    #include "USBFS_msc.h"
#endif /* (USBFS_ENABLE_MSC_CLASS) */

#if (USBFS_EP_MANAGEMENT_DMA)
    #if (CY_PSOC4)
        #include <CyDMA.h>
    #else
        #include <CyDmac.h>
        #if ((USBFS_EP_MANAGEMENT_DMA_AUTO) && (USBFS_EP_DMA_AUTO_OPT == 0u))
            #include "USBFS_EP_DMA_Done_isr.h"
            #include "USBFS_EP8_DMA_Done_SR.h"
            #include "USBFS_EP17_DMA_Done_SR.h"
        #endif /* ((USBFS_EP_MANAGEMENT_DMA_AUTO) && (USBFS_EP_DMA_AUTO_OPT == 0u)) */
    #endif /* (CY_PSOC4) */
#endif /* (USBFS_EP_MANAGEMENT_DMA) */

#if (USBFS_DMA1_ACTIVE)
    #include "USBFS_ep1_dma.h"
    #define USBFS_EP1_DMA_CH     (USBFS_ep1_dma_CHANNEL)
#endif /* (USBFS_DMA1_ACTIVE) */

#if (USBFS_DMA2_ACTIVE)
    #include "USBFS_ep2_dma.h"
    #define USBFS_EP2_DMA_CH     (USBFS_ep2_dma_CHANNEL)
#endif /* (USBFS_DMA2_ACTIVE) */

#if (USBFS_DMA3_ACTIVE)
    #include "USBFS_ep3_dma.h"
    #define USBFS_EP3_DMA_CH     (USBFS_ep3_dma_CHANNEL)
#endif /* (USBFS_DMA3_ACTIVE) */

#if (USBFS_DMA4_ACTIVE)
    #include "USBFS_ep4_dma.h"
    #define USBFS_EP4_DMA_CH     (USBFS_ep4_dma_CHANNEL)
#endif /* (USBFS_DMA4_ACTIVE) */

#if (USBFS_DMA5_ACTIVE)
    #include "USBFS_ep5_dma.h"
    #define USBFS_EP5_DMA_CH     (USBFS_ep5_dma_CHANNEL)
#endif /* (USBFS_DMA5_ACTIVE) */

#if (USBFS_DMA6_ACTIVE)
    #include "USBFS_ep6_dma.h"
    #define USBFS_EP6_DMA_CH     (USBFS_ep6_dma_CHANNEL)
#endif /* (USBFS_DMA6_ACTIVE) */

#if (USBFS_DMA7_ACTIVE)
    #include "USBFS_ep7_dma.h"
    #define USBFS_EP7_DMA_CH     (USBFS_ep7_dma_CHANNEL)
#endif /* (USBFS_DMA7_ACTIVE) */

#if (USBFS_DMA8_ACTIVE)
    #include "USBFS_ep8_dma.h"
    #define USBFS_EP8_DMA_CH     (USBFS_ep8_dma_CHANNEL)
#endif /* (USBFS_DMA8_ACTIVE) */


/***************************************
*     Private Variables
***************************************/

/* Generated external references for descriptors. */
extern const uint8 CYCODE USBFS_DEVICE0_DESCR[18u];
extern const uint8 CYCODE USBFS_DEVICE0_CONFIGURATION0_DESCR[41u];
extern const T_USBFS_LUT CYCODE USBFS_DEVICE0_CONFIGURATION0_INTERFACE0_TABLE[1u];
extern const T_USBFS_EP_SETTINGS_BLOCK CYCODE USBFS_DEVICE0_CONFIGURATION0_EP_SETTINGS_TABLE[2u];
extern const uint8 CYCODE USBFS_DEVICE0_CONFIGURATION0_INTERFACE_CLASS[1u];
extern const T_USBFS_LUT CYCODE USBFS_DEVICE0_CONFIGURATION0_TABLE[4u];
extern const T_USBFS_LUT CYCODE USBFS_DEVICE0_TABLE[3u];
extern const T_USBFS_LUT CYCODE USBFS_TABLE[1u];
extern const uint8 CYCODE USBFS_SN_STRING_DESCRIPTOR[10];
extern const uint8 CYCODE USBFS_STRING_DESCRIPTORS[83u];
extern T_USBFS_XFER_STATUS_BLOCK USBFS_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_IN_RPT_SCB;
extern uint8 USBFS_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_IN_BUF[
            USBFS_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_IN_BUF_SIZE];
extern T_USBFS_XFER_STATUS_BLOCK USBFS_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_RPT_SCB;
extern uint8 USBFS_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_BUF[
            USBFS_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_BUF_SIZE];
extern const uint8 CYCODE USBFS_HIDREPORT_DESCRIPTOR1[40u];
extern const T_USBFS_TD CYCODE USBFS_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_IN_RPT_TABLE[1u];
extern const T_USBFS_TD CYCODE USBFS_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_RPT_TABLE[1u];
extern const T_USBFS_LUT CYCODE USBFS_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_TABLE[5u];


extern const uint8 CYCODE USBFS_MSOS_DESCRIPTOR[USBFS_MSOS_DESCRIPTOR_LENGTH];
extern const uint8 CYCODE USBFS_MSOS_CONFIGURATION_DESCR[USBFS_MSOS_CONF_DESCR_LENGTH];
#if defined(USBFS_ENABLE_IDSN_STRING)
    extern uint8 USBFS_idSerialNumberStringDescriptor[USBFS_IDSN_DESCR_LENGTH];
#endif /* (USBFS_ENABLE_IDSN_STRING) */

extern volatile uint8 USBFS_interfaceNumber;
extern volatile uint8 USBFS_interfaceSetting[USBFS_MAX_INTERFACES_NUMBER];
extern volatile uint8 USBFS_interfaceSettingLast[USBFS_MAX_INTERFACES_NUMBER];
extern volatile uint8 USBFS_deviceAddress;
extern volatile uint8 USBFS_interfaceStatus[USBFS_MAX_INTERFACES_NUMBER];
extern const uint8 CYCODE *USBFS_interfaceClass;

extern volatile T_USBFS_EP_CTL_BLOCK USBFS_EP[USBFS_MAX_EP];
extern volatile T_USBFS_TD USBFS_currentTD;

#if (USBFS_EP_MANAGEMENT_DMA)
    #if (CY_PSOC4)
        extern const uint8 USBFS_DmaChan[USBFS_MAX_EP];
    #else
        extern uint8 USBFS_DmaChan[USBFS_MAX_EP];
        extern uint8 USBFS_DmaTd  [USBFS_MAX_EP];
    #endif /* (CY_PSOC4) */
#endif /* (USBFS_EP_MANAGEMENT_DMA) */

#if (USBFS_EP_MANAGEMENT_DMA_AUTO)
#if (CY_PSOC4)
    extern uint8  USBFS_DmaEpBurstCnt   [USBFS_MAX_EP];
    extern uint8  USBFS_DmaEpLastBurstEl[USBFS_MAX_EP];

    extern uint8  USBFS_DmaEpBurstCntBackup  [USBFS_MAX_EP];
    extern uint32 USBFS_DmaEpBufferAddrBackup[USBFS_MAX_EP];
    
    extern const uint8 USBFS_DmaReqOut     [USBFS_MAX_EP];    
    extern const uint8 USBFS_DmaBurstEndOut[USBFS_MAX_EP];
#else
    #if (USBFS_EP_DMA_AUTO_OPT == 0u)
        extern uint8 USBFS_DmaNextTd[USBFS_MAX_EP];
        extern volatile uint16 USBFS_inLength [USBFS_MAX_EP];
        extern volatile uint8  USBFS_inBufFull[USBFS_MAX_EP];
        extern const uint8 USBFS_epX_TD_TERMOUT_EN[USBFS_MAX_EP];
        extern const uint8 *USBFS_inDataPointer[USBFS_MAX_EP];
    #endif /* (USBFS_EP_DMA_AUTO_OPT == 0u) */
#endif /* CY_PSOC4 */
#endif /* (USBFS_EP_MANAGEMENT_DMA_AUTO) */

extern volatile uint8 USBFS_ep0Toggle;
extern volatile uint8 USBFS_lastPacketSize;
extern volatile uint8 USBFS_ep0Mode;
extern volatile uint8 USBFS_ep0Count;
extern volatile uint16 USBFS_transferByteCount;


/***************************************
*     Private Function Prototypes
***************************************/
void  USBFS_ReInitComponent(void)            ;
void  USBFS_HandleSetup(void)                ;
void  USBFS_HandleIN(void)                   ;
void  USBFS_HandleOUT(void)                  ;
void  USBFS_LoadEP0(void)                    ;
uint8 USBFS_InitControlRead(void)            ;
uint8 USBFS_InitControlWrite(void)           ;
void  USBFS_ControlReadDataStage(void)       ;
void  USBFS_ControlReadStatusStage(void)     ;
void  USBFS_ControlReadPrematureStatus(void) ;
uint8 USBFS_InitControlWrite(void)           ;
uint8 USBFS_InitZeroLengthControlTransfer(void) ;
void  USBFS_ControlWriteDataStage(void)      ;
void  USBFS_ControlWriteStatusStage(void)    ;
void  USBFS_ControlWritePrematureStatus(void);
uint8 USBFS_InitNoDataControlTransfer(void)  ;
void  USBFS_NoDataControlStatusStage(void)   ;
void  USBFS_InitializeStatusBlock(void)      ;
void  USBFS_UpdateStatusBlock(uint8 completionCode) ;
uint8 USBFS_DispatchClassRqst(void)          ;

void USBFS_Config(uint8 clearAltSetting) ;
void USBFS_ConfigAltChanged(void)        ;
void USBFS_ConfigReg(void)               ;
void USBFS_EpStateInit(void)             ;


const T_USBFS_LUT CYCODE *USBFS_GetConfigTablePtr(uint8 confIndex);
const T_USBFS_LUT CYCODE *USBFS_GetDeviceTablePtr(void)           ;
#if (USBFS_BOS_ENABLE)
    const T_USBFS_LUT CYCODE *USBFS_GetBOSPtr(void)               ;
#endif /* (USBFS_BOS_ENABLE) */
const uint8 CYCODE *USBFS_GetInterfaceClassTablePtr(void)                    ;
uint8 USBFS_ClearEndpointHalt(void)                                          ;
uint8 USBFS_SetEndpointHalt(void)                                            ;
uint8 USBFS_ValidateAlternateSetting(void)                                   ;

void USBFS_SaveConfig(void)      ;
void USBFS_RestoreConfig(void)   ;

#if (CY_PSOC3 || CY_PSOC5LP)
    #if (USBFS_EP_MANAGEMENT_DMA_AUTO && (USBFS_EP_DMA_AUTO_OPT == 0u))
        void USBFS_LoadNextInEP(uint8 epNumber, uint8 mode)  ;
    #endif /* (USBFS_EP_MANAGEMENT_DMA_AUTO && (USBFS_EP_DMA_AUTO_OPT == 0u)) */
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

#if defined(USBFS_ENABLE_IDSN_STRING)
    void USBFS_ReadDieID(uint8 descr[])  ;
#endif /* USBFS_ENABLE_IDSN_STRING */

#if defined(USBFS_ENABLE_HID_CLASS)
    uint8 USBFS_DispatchHIDClassRqst(void) ;
#endif /* (USBFS_ENABLE_HID_CLASS) */

#if defined(USBFS_ENABLE_AUDIO_CLASS)
    uint8 USBFS_DispatchAUDIOClassRqst(void) ;
#endif /* (USBFS_ENABLE_AUDIO_CLASS) */

#if defined(USBFS_ENABLE_CDC_CLASS)
    uint8 USBFS_DispatchCDCClassRqst(void) ;
#endif /* (USBFS_ENABLE_CDC_CLASS) */

#if (USBFS_ENABLE_MSC_CLASS)
    #if (USBFS_HANDLE_MSC_REQUESTS)
        uint8 USBFS_DispatchMSCClassRqst(void) ;
    #endif /* (USBFS_HANDLE_MSC_REQUESTS) */
#endif /* (USBFS_ENABLE_MSC_CLASS */

CY_ISR_PROTO(USBFS_EP_0_ISR);
CY_ISR_PROTO(USBFS_BUS_RESET_ISR);

#if (USBFS_SOF_ISR_ACTIVE)
    CY_ISR_PROTO(USBFS_SOF_ISR);
#endif /* (USBFS_SOF_ISR_ACTIVE) */

#if (USBFS_EP1_ISR_ACTIVE)
    CY_ISR_PROTO(USBFS_EP_1_ISR);
#endif /* (USBFS_EP1_ISR_ACTIVE) */

#if (USBFS_EP2_ISR_ACTIVE)
    CY_ISR_PROTO(USBFS_EP_2_ISR);
#endif /* (USBFS_EP2_ISR_ACTIVE) */

#if (USBFS_EP3_ISR_ACTIVE)
    CY_ISR_PROTO(USBFS_EP_3_ISR);
#endif /* (USBFS_EP3_ISR_ACTIVE) */

#if (USBFS_EP4_ISR_ACTIVE)
    CY_ISR_PROTO(USBFS_EP_4_ISR);
#endif /* (USBFS_EP4_ISR_ACTIVE) */

#if (USBFS_EP5_ISR_ACTIVE)
    CY_ISR_PROTO(USBFS_EP_5_ISR);
#endif /* (USBFS_EP5_ISR_ACTIVE) */

#if (USBFS_EP6_ISR_ACTIVE)
    CY_ISR_PROTO(USBFS_EP_6_ISR);
#endif /* (USBFS_EP6_ISR_ACTIVE) */

#if (USBFS_EP7_ISR_ACTIVE)
    CY_ISR_PROTO(USBFS_EP_7_ISR);
#endif /* (USBFS_EP7_ISR_ACTIVE) */

#if (USBFS_EP8_ISR_ACTIVE)
    CY_ISR_PROTO(USBFS_EP_8_ISR);
#endif /* (USBFS_EP8_ISR_ACTIVE) */

#if (USBFS_EP_MANAGEMENT_DMA)
    CY_ISR_PROTO(USBFS_ARB_ISR);
#endif /* (USBFS_EP_MANAGEMENT_DMA) */

#if (USBFS_DP_ISR_ACTIVE)
    CY_ISR_PROTO(USBFS_DP_ISR);
#endif /* (USBFS_DP_ISR_ACTIVE) */

#if (CY_PSOC4)
    CY_ISR_PROTO(USBFS_INTR_HI_ISR);
    CY_ISR_PROTO(USBFS_INTR_MED_ISR);
    CY_ISR_PROTO(USBFS_INTR_LO_ISR);
    #if (USBFS_LPM_ACTIVE)
        CY_ISR_PROTO(USBFS_LPM_ISR);
    #endif /* (USBFS_LPM_ACTIVE) */
#endif /* (CY_PSOC4) */

#if (USBFS_EP_MANAGEMENT_DMA_AUTO)
#if (CY_PSOC4)
    #if (USBFS_DMA1_ACTIVE)
        void USBFS_EP1_DMA_DONE_ISR(void);
    #endif /* (USBFS_DMA1_ACTIVE) */

    #if (USBFS_DMA2_ACTIVE)
        void USBFS_EP2_DMA_DONE_ISR(void);
    #endif /* (USBFS_DMA2_ACTIVE) */

    #if (USBFS_DMA3_ACTIVE)
        void USBFS_EP3_DMA_DONE_ISR(void);
    #endif /* (USBFS_DMA3_ACTIVE) */

    #if (USBFS_DMA4_ACTIVE)
        void USBFS_EP4_DMA_DONE_ISR(void);
    #endif /* (USBFS_DMA4_ACTIVE) */

    #if (USBFS_DMA5_ACTIVE)
        void USBFS_EP5_DMA_DONE_ISR(void);
    #endif /* (USBFS_DMA5_ACTIVE) */

    #if (USBFS_DMA6_ACTIVE)
        void USBFS_EP6_DMA_DONE_ISR(void);
    #endif /* (USBFS_DMA6_ACTIVE) */

    #if (USBFS_DMA7_ACTIVE)
        void USBFS_EP7_DMA_DONE_ISR(void);
    #endif /* (USBFS_DMA7_ACTIVE) */

    #if (USBFS_DMA8_ACTIVE)
        void USBFS_EP8_DMA_DONE_ISR(void);
    #endif /* (USBFS_DMA8_ACTIVE) */

#else
    #if (USBFS_EP_DMA_AUTO_OPT == 0u)
        CY_ISR_PROTO(USBFS_EP_DMA_DONE_ISR);
    #endif /* (USBFS_EP_DMA_AUTO_OPT == 0u) */
#endif /* (CY_PSOC4) */
#endif /* (USBFS_EP_MANAGEMENT_DMA_AUTO) */


/***************************************
*         Request Handlers
***************************************/

uint8 USBFS_HandleStandardRqst(void) ;
uint8 USBFS_DispatchClassRqst(void)  ;
uint8 USBFS_HandleVendorRqst(void)   ;


/***************************************
*    HID Internal references
***************************************/

#if defined(USBFS_ENABLE_HID_CLASS)
    void USBFS_FindReport(void)            ;
    void USBFS_FindReportDescriptor(void)  ;
    void USBFS_FindHidClassDecriptor(void) ;
#endif /* USBFS_ENABLE_HID_CLASS */


/***************************************
*    MIDI Internal references
***************************************/

#if defined(USBFS_ENABLE_MIDI_STREAMING)
    void USBFS_MIDI_IN_EP_Service(void)  ;
#endif /* (USBFS_ENABLE_MIDI_STREAMING) */


/***************************************
*    CDC Internal references
***************************************/

#if defined(USBFS_ENABLE_CDC_CLASS)

    typedef struct
    {
        uint8  bRequestType;
        uint8  bNotification;
        uint8  wValue;
        uint8  wValueMSB;
        uint8  wIndex;
        uint8  wIndexMSB;
        uint8  wLength;
        uint8  wLengthMSB;
        uint8  wSerialState;
        uint8  wSerialStateMSB;
    } t_USBFS_cdc_notification;

    uint8 USBFS_GetInterfaceComPort(uint8 interface) ;
    uint8 USBFS_Cdc_EpInit( const T_USBFS_EP_SETTINGS_BLOCK CYCODE *pEP, uint8 epNum, uint8 cdcComNums) ;

    extern volatile uint8  USBFS_cdc_dataInEpList[USBFS_MAX_MULTI_COM_NUM];
    extern volatile uint8  USBFS_cdc_dataOutEpList[USBFS_MAX_MULTI_COM_NUM];
    extern volatile uint8  USBFS_cdc_commInEpList[USBFS_MAX_MULTI_COM_NUM];
#endif /* (USBFS_ENABLE_CDC_CLASS) */


#endif /* CY_USBFS_USBFS_pvt_H */


/* [] END OF FILE */
