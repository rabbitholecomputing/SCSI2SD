/*******************************************************************************
* File Name: WifiStateTimer_PM.c
* Version 2.80
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "WifiStateTimer.h"

static WifiStateTimer_backupStruct WifiStateTimer_backup;


/*******************************************************************************
* Function Name: WifiStateTimer_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  WifiStateTimer_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void WifiStateTimer_SaveConfig(void) 
{
    #if (!WifiStateTimer_UsingFixedFunction)
        WifiStateTimer_backup.TimerUdb = WifiStateTimer_ReadCounter();
        WifiStateTimer_backup.InterruptMaskValue = WifiStateTimer_STATUS_MASK;
        #if (WifiStateTimer_UsingHWCaptureCounter)
            WifiStateTimer_backup.TimerCaptureCounter = WifiStateTimer_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!WifiStateTimer_UDB_CONTROL_REG_REMOVED)
            WifiStateTimer_backup.TimerControlRegister = WifiStateTimer_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: WifiStateTimer_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  WifiStateTimer_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void WifiStateTimer_RestoreConfig(void) 
{   
    #if (!WifiStateTimer_UsingFixedFunction)

        WifiStateTimer_WriteCounter(WifiStateTimer_backup.TimerUdb);
        WifiStateTimer_STATUS_MASK =WifiStateTimer_backup.InterruptMaskValue;
        #if (WifiStateTimer_UsingHWCaptureCounter)
            WifiStateTimer_SetCaptureCount(WifiStateTimer_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!WifiStateTimer_UDB_CONTROL_REG_REMOVED)
            WifiStateTimer_WriteControlRegister(WifiStateTimer_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: WifiStateTimer_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  WifiStateTimer_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void WifiStateTimer_Sleep(void) 
{
    #if(!WifiStateTimer_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(WifiStateTimer_CTRL_ENABLE == (WifiStateTimer_CONTROL & WifiStateTimer_CTRL_ENABLE))
        {
            /* Timer is enabled */
            WifiStateTimer_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            WifiStateTimer_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    WifiStateTimer_Stop();
    WifiStateTimer_SaveConfig();
}


/*******************************************************************************
* Function Name: WifiStateTimer_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  WifiStateTimer_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void WifiStateTimer_Wakeup(void) 
{
    WifiStateTimer_RestoreConfig();
    #if(!WifiStateTimer_UDB_CONTROL_REG_REMOVED)
        if(WifiStateTimer_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                WifiStateTimer_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
