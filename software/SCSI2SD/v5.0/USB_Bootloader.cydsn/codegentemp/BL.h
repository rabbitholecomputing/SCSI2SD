/****************************************************************************//**
* \file BL.c
* \version 1.60
*
* \brief
*   Provides an API for the Bootloader. The API includes functions for starting
*   bootloading operations, validating the application and jumping to the
*   application.
*
********************************************************************************
* \copyright
* Copyright 2008-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_BOOTLOADER_BL_H)
#define CY_BOOTLOADER_BL_H

#include "cytypes.h"
#include "CyFlash.h"

#define BL_DUAL_APP_BOOTLOADER        (0u)
#define BL_BOOTLOADER_APP_VERSION     (0u)
#define BL_FAST_APP_VALIDATION        (1u)
#define BL_PACKET_CHECKSUM_CRC        (0u)
#define BL_WAIT_FOR_COMMAND           (0u)
#define BL_WAIT_FOR_COMMAND_TIME      (20u)
#define BL_BOOTLOADER_APP_VALIDATION  (1u)

#define BL_CMD_GET_FLASH_SIZE_AVAIL   (1u)
#define BL_CMD_ERASE_ROW_AVAIL        (1u)
#define BL_CMD_GET_ROW_CHKSUM_AVAIL   (1u)
#define BL_CMD_SYNC_BOOTLOADER_AVAIL  (1u)
#define BL_CMD_SEND_DATA_AVAIL        (1u)
#define BL_CMD_VERIFY_APP_CHKSUM_AVAIL (1u)
#define BL_CMD_GET_METADATA           (0u)
#define BL_CMD_VERIFY_FLS_ROW_AVAIL   (0u)
#define BL_GOLDEN_IMAGE_AVAIL         (0u)
#define BL_SECURITY_KEY_AVAIL         (0u)
#define BL_AUTO_SWITCHING_AVAIL       (1u)

#if ((0u != BL_DUAL_APP_BOOTLOADER) || \
     (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)) 
    #define BL_CMD_GET_APP_STATUS_AVAIL   (1u)
#endif  /* (0u != BL_DUAL_APP_BOOTLOADER) ||
           (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)*/

#if (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LAUNCHER)
    #define BL_COPIER_AVAIL           (0u)
#endif /*(CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LAUNCHER)*/

#if (0u != BL_SECURITY_KEY_AVAIL) 
    #define BL_SECURITY_KEY_LENGTH        (6u)
#endif    /*(0u != BL_SECURITY_KEY_AVAIL)*/

/*******************************************************************************
* Copier definitions
*******************************************************************************/
#if (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LAUNCHER)
    #if (0u != BL_COPIER_AVAIL)
    /**************************************************************************** 
    * Defines how many times Copier will try to copy the new app#0 image from
    * the temporary location to app#0 flash space (if it was not successful 
    * the first time) before giving up.
    ****************************************************************************/    
    #define BL_MAX_ATTEMPTS      (3u)
    #endif /* (0u != BL_COPIER_AVAIL) */
#endif /* (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LAUNCHER) */

#if ((CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LAUNCHER) && (!CY_PSOC3))
/*******************************************************************************
* Callback definitions
*******************************************************************************/   
/**
 \defgroup structures_group Structures
 @{
*/           
typedef struct 
{
    uint8 command;
    uint16 packetLength;
    uint8* pInputBuffer;
} BL_in_packet_type;

typedef struct 
{
    cystatus status;
    uint16 packetLength;
    uint8* pOutputBuffer;
} BL_out_packet_type;

/** @}*/

typedef void (*BL_callback_type)(BL_in_packet_type* inputPacket, 
                                               BL_out_packet_type* outputPacket);

void BL_InitCallback(BL_callback_type userCallback) \
           CYSMALL ;
#endif /* (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LAUNCHER) && (!CY_PSOC3) */
           
/*******************************************************************************
* Bootloadable applications identification
*******************************************************************************/
#define BL_MD_BTLDB_ACTIVE_0          (0x00u)

#if ((0u != BL_DUAL_APP_BOOTLOADER) || \
     (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER))
    #define BL_MD_BTLDB_ACTIVE_1      (0x01u)
    #define BL_MD_BTLDB_ACTIVE_NONE   (0x02u)

    #if (1u == BL_GOLDEN_IMAGE_AVAIL)
        #define BL_GOLDEN_IMAGE           (0x00u)
    #endif /*(1u == BL_GOLDEN_IMAGE_AVAIL)*/
#endif  /* (0u != Bootloader_DUAL_APP_BOOTLOADER) || 
           (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER) */


/* Mask used to indicate starting application */
#define BL_SCHEDULE_BTLDB             (0x80u)
#define BL_SCHEDULE_BTLDR             (0x40u)
#define BL_SCHEDULE_MASK              (0xC0u)
#define BL_SCHEDULE_BTLDR_INIT_STATE  (0x00u)

#if (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)
#if defined(__ARMCC_VERSION) || defined (__GNUC__)
    __attribute__((section (".bootloader")))
#elif defined (__ICCARM__)
    #pragma location=".bootloader"
#endif  /* defined(__ARMCC_VERSION) || defined (__GNUC__) */
extern const uint8  CYCODE BL_Checksum;
extern const uint8  CYCODE  *BL_ChecksumAccess;


#if defined(__ARMCC_VERSION) || defined (__GNUC__)
    __attribute__((section (".bootloader")))
#elif defined (__ICCARM__)
    #pragma location=".bootloader"
#endif  /* defined(__ARMCC_VERSION) || defined (__GNUC__) */
extern const uint32 CYCODE BL_SizeBytes;
extern const uint32 CYCODE *BL_SizeBytesAccess;
#endif /*CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER*/

#if (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER) 
extern uint8 BL_initVar;
extern uint8 BL_runningApp;
extern uint8 BL_isBootloading;
#endif /*CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER*/

#if ((CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LAUNCHER) && (CY_PSOC4))
    extern void BL_UserCopierCallback(void);
    #define BL_USER_COPIER_CALLBACK BL_UserCopierCallback()
#endif /* (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LAUNCHER) && (CY_PSOC4) */

/*******************************************************************************
* This variable is used by the Bootloader/Bootloadable components to schedule which
* application will be started after a software reset.
*******************************************************************************/
#if (CY_PSOC4)
    #if defined(__ARMCC_VERSION)
        __attribute__ ((section(".bootloaderruntype"), zero_init))
    #elif defined (__GNUC__)
        __attribute__ ((section(".bootloaderruntype")))
   #elif defined (__ICCARM__)
        #pragma location=".bootloaderruntype"
    #endif  /* defined(__ARMCC_VERSION) */
    extern volatile uint32 cyBtldrRunType;
#endif  /* (CY_PSOC4) */


#if ((0u != BL_DUAL_APP_BOOTLOADER) || \
     (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER))
    extern volatile uint8 BL_activeApp;
#endif  /* (0u != BL_DUAL_APP_BOOTLOADER) ||
           (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)*/


#if(CY_PSOC4)
    /* Reset Cause Observation Register */
    #define BL_RES_CAUSE_REG           (* (reg32 *) CYREG_RES_CAUSE)
    #define BL_RES_CAUSE_PTR           (  (reg32 *) CYREG_RES_CAUSE)
#else
    #define BL_RESET_SR0_REG           (* (reg8 *) CYREG_RESET_SR0)
    #define BL_RESET_SR0_PTR           (  (reg8 *) CYREG_RESET_SR0)
#endif /* (CY_PSOC4) */


/*******************************************************************************
* Get the reason for a device reset
*  Return cyBtldrRunType in the case if a software reset was the reset reason and
*  set cyBtldrRunType to zero (the Bootloader application is scheduled - that is
*  the initial clean state) and return zero.
*******************************************************************************/
#if(CY_PSOC4)
    #define BL_GET_RUN_TYPE           (cyBtldrRunType)
#else
    #define BL_GET_RUN_TYPE       (BL_RESET_SR0_REG & BL_SCHEDULE_MASK)
#endif  /* (CY_PSOC4) */


/*******************************************************************************
* Schedule Bootloader/Bootloadable to be run after a software reset
*******************************************************************************/
#if(CY_PSOC4)
    #define BL_SET_RUN_TYPE(x)                (cyBtldrRunType = (x))
#else
    #define BL_SET_RUN_TYPE(x)                (BL_RESET_SR0_REG = (x))
#endif  /* (CY_PSOC4) */


/* Returns number of flash arrays available in device. */
#ifndef CY_FLASH_NUMBER_ARRAYS
    #define CY_FLASH_NUMBER_ARRAYS                  (CYDEV_FLASH_SIZE / CYDEV_FLS_SECTOR_SIZE)
#endif /* CY_FLASH_NUMBER_ARRAYS */


/*******************************************************************************
* External References
*******************************************************************************/
/**
 \defgroup functions_group Functions
 @{
*/
#if (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)
    void CyBtldr_CheckLaunch(void)  CYSMALL ;
#endif /*CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER*/

void BL_SetFlashByte(uint32 address, uint8 runType);
void BL_Start(void) CYSMALL ;
cystatus BL_ValidateBootloadable(uint8 appId) \
            CYSMALL ;
uint8 BL_Calc8BitSum(uint32 baseAddr, uint32 start, uint32 size) CYSMALL \
                                    ;
uint32   BL_GetMetadata(uint8 field, uint8 appId) \
                                    ;
void BL_Exit(uint8 appId) CYSMALL ;

#if (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)
    void BL_HostLink(uint8 timeOut) CYLARGE ;    
    void BL_Initialize(void)  CYSMALL ;
    uint8 BL_GetRunningAppStatus(void) CYSMALL ;
    uint8 BL_GetActiveAppStatus(void) CYSMALL ;
#endif /*(CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)*/
/** @} */

#if(CY_PSOC3)
    /* Implementation for PSoC 3 resides in BL_psoc3.a51 file.  */
    void     BL_LaunchBootloadable(uint32 appAddr);
#endif  /* (CY_PSOC3) */

/* When using a custom interface as the IO Component, the user must provide these functions */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface)
#if ((CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER) || (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_BOOTLOADER) || \
     (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_MULTIAPPBOOTLOADER)) 
    extern void CyBtldrCommStart(void);
    extern void CyBtldrCommStop (void);
    extern void CyBtldrCommReset(void);
    extern cystatus CyBtldrCommWrite(uint8* buffer, uint16 size, uint16* count, uint8 timeOut);
    extern cystatus CyBtldrCommRead (uint8* buffer, uint16 size, uint16* count, uint8 timeOut);
#endif /*(CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER) || (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_BOOTLOADER) || \
         (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_MULTIAPPBOOTLOADER)*/
#endif  /* defined(CYDEV_BOOTLOADER_IO_COMP) && (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface) */

/*******************************************************************************
* BL_Initialize()
*******************************************************************************/
#define BL_BOTH_ACTIVE                (0x03u)

/**
 \defgroup constants_group Constants
 @{
*/

/** \addtogroup group_metadataFields Metadata Fields
 *  Metadata fields description 
 *  @{
 */
/*******************************************************************************
* BL_GetMetadata()
*******************************************************************************/
#define BL_GET_BTLDB_CHECKSUM         (1u)   /**< Bootloadable Application Checksum */
#define BL_GET_BTLDB_ADDR             (2u)   /**< Bootloadable Application Start Routine Address */
#define BL_GET_BTLDR_LAST_ROW         (3u)   /**< Bootloader Last Flash Row */
#define BL_GET_BTLDB_LENGTH           (4u)   /**< Bootloadable Application Length */
#define BL_GET_BTLDB_ACTIVE           (5u)   /**< Active Bootloadable Application */
#define BL_GET_BTLDB_STATUS           (6u)   /**< Bootloadable Application Verification Status */
#define BL_GET_BTLDR_APP_VERSION      (7u)   /**< Bootloader Application Version */
#define BL_GET_BTLDB_APP_VERSION      (8u)   /**< Bootloadable Application Version */
#define BL_GET_BTLDB_APP_ID           (9u)   /**< Bootloadable Application ID */
#define BL_GET_BTLDB_APP_CUST_ID      (10u)  /**< Bootloadable Application Custom ID */
#define BL_GET_BTLDB_COPY_FLAG        (11u)  /**< "need-to-copy flag */
#define BL_GET_BTLDB_USER_DATA        (12u)  /**< User data */

/** @} group_metadataFields */
/** @} constants_group */

#define BL_GET_METADATA_RESPONSE_SIZE (56u)  

/*******************************************************************************
* BL_Exit()
*******************************************************************************/
#define BL_EXIT_TO_BTLDR              (2u)
#define BL_EXIT_TO_BTLDB              (0u)
#if ((0u != BL_DUAL_APP_BOOTLOADER) || \
    (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER))
    #define BL_EXIT_TO_BTLDB_1        (0u)
    #define BL_EXIT_TO_BTLDB_2        (1u)
#endif  /* (0u != BL_DUAL_APP_BOOTLOADER) ||
           (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)*/

#if (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)
/*******************************************************************************
* In-app Bootloader definitions
*******************************************************************************/
#define BL_BOOTLOADING_IN_PROGRESS     (1u)
#define BL_BOOTLOADING_COMPLETED       (0u)

#define BL_BOOTLADING_INITIALIZED      (1u)
#define BL_BOOTLOADING_NOT_INITIALIZED (0u)

#define BL_RUNNING_APPLICATION_0       (0u)     
#define BL_RUNNING_APPLICATION_1       (1u)     
#define BL_RUNNING_APPLICATION_UNKNOWN (2u)     
#endif /*CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER*/    


/*******************************************************************************
* Input/output packet defines
*******************************************************************************/
#define BL_MIN_PKT_SIZE             (7u)   /* The minimum number of bytes in a packet */
#define BL_SIZEOF_COMMAND_BUFFER    (300u) /* Maximum number of bytes accepted in one packet plus some */

/*******************************************************************************
* Kept for backward compatibility.
*******************************************************************************/
#if ((0u != BL_DUAL_APP_BOOTLOADER) || (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER))
    #define BL_ValidateApp(x)                 BL_ValidateBootloadable((x))
    #define BL_ValidateApplication()            \
                            BL_ValidateBootloadable(BL_MD_BTLDB_ACTIVE_0)
#else
    #define BL_ValidateApplication()            \
                            BL_ValidateBootloadable(BL_MD_BTLDB_ACTIVE_0)
    #define BL_ValidateApp(x)                 BL_ValidateBootloadable((x))
#endif  /* (0u != BL_DUAL_APP_BOOTLOADER) || (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)*/
#define BL_Calc8BitFlashSum(start, size)      BL_Calc8BitSum(CY_FLASH_BASE, (start), (size))


/*******************************************************************************
* The following code is DEPRECATED and must not be used.
*******************************************************************************/
#define BL_CMD_VERIFY_ROW_AVAIL           BL_CMD_GET_ROW_CHKSUM_AVAIL

#define BL_BOOTLOADABLE_APP_VALID         (BL_BOOTLOADER_APP_VALIDATION)
#define CyBtldr_Start                                    BL_Start

#define BL_NUM_OF_FLASH_ARRAYS            (CYDEV_FLASH_SIZE / CYDEV_FLS_SECTOR_SIZE)
#define BL_META_BASE(x)                   (CYDEV_FLASH_BASE + \
                                                            (CYDEV_FLASH_SIZE - (( uint32 )(x) * CYDEV_FLS_ROW_SIZE) - \
                                                            BL_META_DATA_SIZE))
#define BL_META_ARRAY                     (BL_NUM_OF_FLASH_ARRAYS - 1u)
#define BL_META_APP_ENTRY_POINT_ADDR(x)   (BL_META_BASE(x) + \
                                                            BL_META_APP_ADDR_OFFSET)
#define BL_META_APP_BYTE_LEN(x)           (BL_META_BASE(x) + \
                                                            BL_META_APP_BYTE_LEN_OFFSET)
#define BL_META_APP_RUN_ADDR(x)           (BL_META_BASE(x) + \
                                                            BL_META_APP_RUN_TYPE_OFFSET)
#define BL_META_APP_ACTIVE_ADDR(x)        (BL_META_BASE(x) + \
                                                            BL_META_APP_ACTIVE_OFFSET)
#define BL_META_APP_VERIFIED_ADDR(x)      (BL_META_BASE(x) + \
                                                            BL_META_APP_VERIFIED_OFFSET)
#define BL_META_APP_BLDBL_VER_ADDR(x)     (BL_META_BASE(x) + \
                                                            BL_META_APP_BL_BUILD_VER_OFFSET)
#define BL_META_APP_VER_ADDR(x)           (BL_META_BASE(x) + \
                                                            BL_META_APP_VER_OFFSET)
#define BL_META_APP_ID_ADDR(x)            (BL_META_BASE(x) + \
                                                            BL_META_APP_ID_OFFSET)
#define BL_META_APP_CUST_ID_ADDR(x)       (BL_META_BASE(x) + \
                                                            BL_META_APP_CUST_ID_OFFSET)
#define BL_META_LAST_BLDR_ROW_ADDR(x)     (BL_META_BASE(x) + \
                                                            BL_META_APP_BL_LAST_ROW_OFFSET)
#define BL_META_CHECKSUM_ADDR(x)          (BL_META_BASE(x) + \
                                                            BL_META_APP_CHECKSUM_OFFSET)
#if(0u == BL_DUAL_APP_BOOTLOADER)
    #define BL_MD_BASE                    BL_META_BASE(0u)

    #if(!CY_PSOC4)
        #define BL_MD_ROW                 ((CY_FLASH_NUMBER_ROWS / BL_NUM_OF_FLASH_ARRAYS) \
                                                        - 1u)
    #else
        #define BL_MD_ROW                 (CY_FLASH_NUMBER_ROWS - 1u)
    #endif /* (CY_PSOC4) */

    #define BL_MD_CHECKSUM_ADDR           BL_META_CHECKSUM_ADDR(0u)
    #define BL_MD_LAST_BLDR_ROW_ADDR      BL_META_LAST_BLDR_ROW_ADDR(0u)
    #define BL_MD_APP_BYTE_LEN            BL_META_APP_BYTE_LEN(0u)
    #define BL_MD_APP_VERIFIED_ADDR       BL_META_APP_VERIFIED_ADDR(0u)
    #define BL_MD_APP_ENTRY_POINT_ADDR    BL_META_APP_ENTRY_POINT_ADDR(0u)
    #define BL_MD_APP_RUN_ADDR            BL_META_APP_RUN_ADDR(0u)
#else
    #if(!CY_PSOC4)
        #define BL_MD_ROW(x)              ((CY_FLASH_NUMBER_ROWS / BL_NUM_OF_FLASH_ARRAYS) \
                                                        - 1u - ( uint32 )(x))
    #else
        #define BL_MD_ROW(x)              (CY_FLASH_NUMBER_ROWS - 1u - ( uint32 )(x))
    #endif /* (CY_PSOC4) */

    #define BL_MD_CHECKSUM_ADDR           BL_META_CHECKSUM_ADDR(appId)
    #define BL_MD_LAST_BLDR_ROW_ADDR      BL_META_LAST_BLDR_ROW_ADDR(appId)
    #define BL_MD_APP_BYTE_LEN            BL_META_APP_BYTE_LEN(appId)
    #define BL_MD_APP_VERIFIED_ADDR       BL_META_APP_VERIFIED_ADDR(appId)
    #define BL_MD_APP_ENTRY_POINT_ADDR    \
                                                BL_META_APP_ENTRY_POINT_ADDR(BL_activeApp)
    #define BL_MD_APP_RUN_ADDR            BL_META_APP_RUN_ADDR(BL_activeApp)
#endif  /* (0u == BL_DUAL_APP_BOOTLOADER) */

#define BL_P_APP_ACTIVE(x)                ((uint8 CYCODE *) BL_META_APP_ACTIVE_ADDR(x))
#define BL_MD_PTR_CHECKSUM                ((uint8  CYCODE *) BL_MD_CHECKSUM_ADDR)
#define BL_MD_PTR_APP_ENTRY_POINT         ((BL_APP_ADDRESS CYCODE *) \
                                                                BL_MD_APP_ENTRY_POINT_ADDR)
#define BL_MD_PTR_LAST_BLDR_ROW            ((uint16 CYCODE *) BL_MD_LAST_BLDR_ROW_ADDR)
#define BL_MD_PTR_APP_BYTE_LEN             ((BL_APP_ADDRESS CYCODE *) \
                                                                BL_MD_APP_BYTE_LEN)
#define BL_MD_PTR_APP_RUN_ADDR             ((uint8  CYCODE *) BL_MD_APP_RUN_ADDR)
#define BL_MD_PTR_APP_VERIFIED             ((uint8  CYCODE *) BL_MD_APP_VERIFIED_ADDR)
#define BL_MD_PTR_APP_BLD_BL_VER           ((uint16 CYCODE *) BL_MD_APP_BLDBL_VER_ADDR)
#define BL_MD_PTR_APP_VER                  ((uint16 CYCODE *) BL_MD_APP_VER_ADDR)
#define BL_MD_PTR_APP_ID                   ((uint16 CYCODE *) BL_MD_APP_ID_ADDR)
#define BL_MD_PTR_APP_CUST_ID              ((uint32 CYCODE *) BL_MD_APP_CUST_ID_ADDR)
#if(CY_PSOC3)
    #define BL_APP_ADDRESS                    uint16
    #define BL_GET_CODE_DATA(idx)             (*((uint8  CYCODE *) (idx)))
    #define BL_GET_CODE_WORD(idx)             (*((uint32 CYCODE *) (idx)))
    #define BL_META_APP_ADDR_OFFSET           (3u)
    #define BL_META_APP_BL_LAST_ROW_OFFSET    (7u)
    #define BL_META_APP_BYTE_LEN_OFFSET       (11u)
    #define BL_META_APP_RUN_TYPE_OFFSET       (15u)
#else
    #define BL_APP_ADDRESS                    uint32
    #define BL_GET_CODE_DATA(idx)             (*((uint8  *)(CYDEV_FLASH_BASE + (idx))))
    #define BL_GET_CODE_WORD(idx)             (*((uint32 *)(CYDEV_FLASH_BASE + (idx))))
    #define BL_META_APP_ADDR_OFFSET           (1u)
    #define BL_META_APP_BL_LAST_ROW_OFFSET    (5u)
    #define BL_META_APP_BYTE_LEN_OFFSET       (9u)
    #define BL_META_APP_RUN_TYPE_OFFSET       (13u)
#endif /* (CY_PSOC3) */

#define BL_META_APP_ACTIVE_OFFSET             (16u)
#define BL_META_APP_VERIFIED_OFFSET           (17u)
#define BL_META_APP_BL_BUILD_VER_OFFSET       (18u)
#define BL_META_APP_ID_OFFSET                 (20u)
#define BL_META_APP_VER_OFFSET                (22u)
#define BL_META_APP_CUST_ID_OFFSET            (24u)

#if (CY_PSOC4)
    #define BL_GET_REG16(x)   ((uint16)(                                                          \
                                                (( uint16 )(( uint16 )CY_GET_XTND_REG8((x)     )       ))   |   \
                                                (( uint16 )(( uint16 )CY_GET_XTND_REG8((x) + 1u) <<  8u))       \
                                            ))

    #define BL_GET_REG32(x)   (                                                                    \
                                                (( uint32 )(( uint32 ) CY_GET_XTND_REG8((x)     )       ))   |   \
                                                (( uint32 )(( uint32 ) CY_GET_XTND_REG8((x) + 1u) <<  8u))   |   \
                                                (( uint32 )(( uint32 ) CY_GET_XTND_REG8((x) + 2u) << 16u))   |   \
                                                (( uint32 )(( uint32 ) CY_GET_XTND_REG8((x) + 3u) << 24u))       \
                                            )
#endif  /* (CY_PSOC4) */
#define BL_META_APP_CHECKSUM_OFFSET           (0u)
#define BL_META_DATA_SIZE                     (64u)
#if(CY_PSOC4)
    extern uint8 appRunType;
#endif  /* (CY_PSOC4) */

#if(CY_PSOC4)
    #define BL_SOFTWARE_RESET                 CY_SET_REG32(CYREG_CM0_AIRCR, 0x05FA0004u)
#else
    #define BL_SOFTWARE_RESET                 CY_SET_REG8(CYREG_RESET_CR2, 0x01u)
#endif  /* (CY_PSOC4) */

#define BL_SetFlashRunType(runType)        BL_SetFlashByte( \
                                                            BL_MD_APP_RUN_ADDR(0), (runType))

#define BL_START_APP                  (BL_SCHEDULE_BTLDB)
#define BL_START_BTLDR                (BL_SCHEDULE_BTLDR)

/* Some PSoC Creator versions are used to generate only one name types */
#if !defined (CYDEV_FLASH_BASE)
    #define CYDEV_FLASH_BASE                                (CYDEV_FLS_BASE)
#endif /* !defined (CYDEV_FLASH_BASE) */

#if !defined (CYDEV_FLASH_SIZE)
    #define CYDEV_FLASH_SIZE                                (CYDEV_FLS_SIZE)
#endif /* CYDEV_FLASH_SIZE */


#endif /* CY_BOOTLOADER_BL_H */


/* [] END OF FILE */
