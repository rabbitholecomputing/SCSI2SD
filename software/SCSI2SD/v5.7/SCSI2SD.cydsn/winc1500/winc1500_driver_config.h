
#ifndef __WINC1500_DRIVER_CONFIG_H
#define __WINC1500_DRIVER_CONFIG_H

//#define USING_PICTAIL     // one of these must be defined, but not both
#define USING_CLICK_BOARD


// Uncomment this define if Host MCU is big-endian.  
// Comment out this define if Host MCU is little-endian.
//#define HOST_MCU_BIG_ENDIAN

// Set the size of host MCU pointer, in bytes.  Pointer sizes vary depending on the MCU 
// architecture.  The pointer size can be determined by executing the code: 
//    int x = sizeof(int *);  
#define __SAMD21J18A__
#if defined(__PIC32MX__) || defined(__PIC32MZ__)
    #define M2M_POINTER_SIZE_IN_BYTES     4
#elif defined(__C30__)
    #define M2M _POINTER_SIZE_IN_BYTES     2
#elif defined(__XC8)
    #define M2M_POINTER_SIZE_IN_BYTES     3 
#elif defined(__SAMD21J18A__)
    #define M2M_POINTER_SIZE_IN_BYTES     4
#else    
    #error "Need to define M2M_POINTER_SIZE_IN_BYTES"
#endif



// comment out those features not needed
#define M2M_ENABLE_ERROR_CHECKING        // error checking code
#define M2M_ENABLE_PRNG                  // prng (psuedo-random number) code
#define M2M_ENABLE_SOFT_AP_MODE          // SoftAP feature
#define M2M_ENABLE_WPS                   // WPS feature
#define M2M_WIFI_ENABLE_P2P                    // P2P feature
#define M2M_ENABLE_HTTP_PROVISION_MODE   // HTTP provisioning mode
#define M2M_ENABLE_SCAN_MODE             // Wi-Fi scanning
#define M2M_ENABLE_SPI_FLASH             // supports host firmware update utility

// The WINC1500, by default, outputs debug information from its UART.  Unless 
// debugging, this should be disabled to save power and increase WINC1500 efficiency.
// Comment out this define to enable the WINC1500 debug output.
#define M2M_DISABLE_FIRMWARE_LOG





#if !defined(USING_PICTAIL) && !defined(USING_CLICK_BOARD)
    #error "Must define either USING_PICTAIL or USING_CLICK_BOARD"
#endif

#if defined(USING_PICTAIL) && defined(USING_CLICK_BOARD)
    #error "Must define either USING_PICTAIL or USING_CLICK_BOARD, but not both"
#endif

#endif // __WINC1500_DRIVER_CONFIG_H
