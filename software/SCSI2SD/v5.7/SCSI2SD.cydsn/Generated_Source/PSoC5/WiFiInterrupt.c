/*******************************************************************************
* File Name: WiFiInterrupt.c  
* Version 1.70
*
*  Description:
*   API for controlling the state of an interrupt.
*
*
*  Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/


#include <cydevice_trm.h>
#include <CyLib.h>
#include <WiFiInterrupt.h>


#if !defined(WiFiInterrupt__REMOVED) /* Check for removal by optimization */

/*******************************************************************************
*  Place your includes, defines and code here 
********************************************************************************/
/* `#START WiFiInterrupt_intc` */

/* `#END` */

#ifndef CYINT_IRQ_BASE
#define CYINT_IRQ_BASE      16
#endif /* CYINT_IRQ_BASE */
#ifndef CYINT_VECT_TABLE
#define CYINT_VECT_TABLE    ((cyisraddress **) CYREG_NVIC_VECT_OFFSET)
#endif /* CYINT_VECT_TABLE */

/* Declared in startup, used to set unused interrupts to. */
CY_ISR_PROTO(IntDefaultHandler);


/*******************************************************************************
* Function Name: WiFiInterrupt_Start
********************************************************************************
*
* Summary:
*  Set up the interrupt and enable it. This function disables the interrupt, 
*  sets the default interrupt vector, sets the priority from the value in the
*  Design Wide Resources Interrupt Editor, then enables the interrupt to the 
*  interrupt controller.
*
* Parameters:  
*   None
*
* Return:
*   None
*
*******************************************************************************/
void WiFiInterrupt_Start(void)
{
    /* For all we know the interrupt is active. */
    WiFiInterrupt_Disable();

    /* Set the ISR to point to the WiFiInterrupt Interrupt. */
    WiFiInterrupt_SetVector(&WiFiInterrupt_Interrupt);

    /* Set the priority. */
    WiFiInterrupt_SetPriority((uint8)WiFiInterrupt_INTC_PRIOR_NUMBER);

    /* Enable it. */
    WiFiInterrupt_Enable();
}


/*******************************************************************************
* Function Name: WiFiInterrupt_StartEx
********************************************************************************
*
* Summary:
*  Sets up the interrupt and enables it. This function disables the interrupt,
*  sets the interrupt vector based on the address passed in, sets the priority 
*  from the value in the Design Wide Resources Interrupt Editor, then enables 
*  the interrupt to the interrupt controller.
*  
*  When defining ISR functions, the CY_ISR and CY_ISR_PROTO macros should be 
*  used to provide consistent definition across compilers:
*  
*  Function definition example:
*   CY_ISR(MyISR)
*   {
*   }
*   Function prototype example:
*   CY_ISR_PROTO(MyISR);
*
* Parameters:  
*   address: Address of the ISR to set in the interrupt vector table.
*
* Return:
*   None
*
*******************************************************************************/
void WiFiInterrupt_StartEx(cyisraddress address)
{
    /* For all we know the interrupt is active. */
    WiFiInterrupt_Disable();

    /* Set the ISR to point to the WiFiInterrupt Interrupt. */
    WiFiInterrupt_SetVector(address);

    /* Set the priority. */
    WiFiInterrupt_SetPriority((uint8)WiFiInterrupt_INTC_PRIOR_NUMBER);

    /* Enable it. */
    WiFiInterrupt_Enable();
}


/*******************************************************************************
* Function Name: WiFiInterrupt_Stop
********************************************************************************
*
* Summary:
*   Disables and removes the interrupt.
*
* Parameters:  
*   None
*
* Return:
*   None
*
*******************************************************************************/
void WiFiInterrupt_Stop(void)
{
    /* Disable this interrupt. */
    WiFiInterrupt_Disable();

    /* Set the ISR to point to the passive one. */
    WiFiInterrupt_SetVector(&IntDefaultHandler);
}


/*******************************************************************************
* Function Name: WiFiInterrupt_Interrupt
********************************************************************************
*
* Summary:
*   The default Interrupt Service Routine for WiFiInterrupt.
*
*   Add custom code between the coments to keep the next version of this file
*   from over writting your code.
*
* Parameters:  
*
* Return:
*   None
*
*******************************************************************************/
CY_ISR(WiFiInterrupt_Interrupt)
{
    #ifdef WiFiInterrupt_INTERRUPT_INTERRUPT_CALLBACK
        WiFiInterrupt_Interrupt_InterruptCallback();
    #endif /* WiFiInterrupt_INTERRUPT_INTERRUPT_CALLBACK */ 

    /*  Place your Interrupt code here. */
    /* `#START WiFiInterrupt_Interrupt` */

    /* `#END` */
}


/*******************************************************************************
* Function Name: WiFiInterrupt_SetVector
********************************************************************************
*
* Summary:
*   Change the ISR vector for the Interrupt. Note calling WiFiInterrupt_Start
*   will override any effect this method would have had. To set the vector 
*   before the component has been started use WiFiInterrupt_StartEx instead.
* 
*   When defining ISR functions, the CY_ISR and CY_ISR_PROTO macros should be 
*   used to provide consistent definition across compilers:
*
*   Function definition example:
*   CY_ISR(MyISR)
*   {
*   }
*
*   Function prototype example:
*     CY_ISR_PROTO(MyISR);
*
* Parameters:
*   address: Address of the ISR to set in the interrupt vector table.
*
* Return:
*   None
*
*******************************************************************************/
void WiFiInterrupt_SetVector(cyisraddress address)
{
    cyisraddress * ramVectorTable;

    ramVectorTable = (cyisraddress *) *CYINT_VECT_TABLE;

    ramVectorTable[CYINT_IRQ_BASE + (uint32)WiFiInterrupt__INTC_NUMBER] = address;
}


/*******************************************************************************
* Function Name: WiFiInterrupt_GetVector
********************************************************************************
*
* Summary:
*   Gets the "address" of the current ISR vector for the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   Address of the ISR in the interrupt vector table.
*
*******************************************************************************/
cyisraddress WiFiInterrupt_GetVector(void)
{
    cyisraddress * ramVectorTable;

    ramVectorTable = (cyisraddress *) *CYINT_VECT_TABLE;

    return ramVectorTable[CYINT_IRQ_BASE + (uint32)WiFiInterrupt__INTC_NUMBER];
}


/*******************************************************************************
* Function Name: WiFiInterrupt_SetPriority
********************************************************************************
*
* Summary:
*   Sets the Priority of the Interrupt. 
*
*   Note calling WiFiInterrupt_Start or WiFiInterrupt_StartEx will 
*   override any effect this API would have had. This API should only be called
*   after WiFiInterrupt_Start or WiFiInterrupt_StartEx has been called. 
*   To set the initial priority for the component, use the Design-Wide Resources
*   Interrupt Editor.
*
*   Note This API has no effect on Non-maskable interrupt NMI).
*
* Parameters:
*   priority: Priority of the interrupt, 0 being the highest priority
*             PSoC 3 and PSoC 5LP: Priority is from 0 to 7.
*             PSoC 4: Priority is from 0 to 3.
*
* Return:
*   None
*
*******************************************************************************/
void WiFiInterrupt_SetPriority(uint8 priority)
{
    *WiFiInterrupt_INTC_PRIOR = priority << 5;
}


/*******************************************************************************
* Function Name: WiFiInterrupt_GetPriority
********************************************************************************
*
* Summary:
*   Gets the Priority of the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   Priority of the interrupt, 0 being the highest priority
*    PSoC 3 and PSoC 5LP: Priority is from 0 to 7.
*    PSoC 4: Priority is from 0 to 3.
*
*******************************************************************************/
uint8 WiFiInterrupt_GetPriority(void)
{
    uint8 priority;


    priority = *WiFiInterrupt_INTC_PRIOR >> 5;

    return priority;
}


/*******************************************************************************
* Function Name: WiFiInterrupt_Enable
********************************************************************************
*
* Summary:
*   Enables the interrupt to the interrupt controller. Do not call this function
*   unless ISR_Start() has been called or the functionality of the ISR_Start() 
*   function, which sets the vector and the priority, has been called.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void WiFiInterrupt_Enable(void)
{
    /* Enable the general interrupt. */
    *WiFiInterrupt_INTC_SET_EN = WiFiInterrupt__INTC_MASK;
}


/*******************************************************************************
* Function Name: WiFiInterrupt_GetState
********************************************************************************
*
* Summary:
*   Gets the state (enabled, disabled) of the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   1 if enabled, 0 if disabled.
*
*******************************************************************************/
uint8 WiFiInterrupt_GetState(void)
{
    /* Get the state of the general interrupt. */
    return ((*WiFiInterrupt_INTC_SET_EN & (uint32)WiFiInterrupt__INTC_MASK) != 0u) ? 1u:0u;
}


/*******************************************************************************
* Function Name: WiFiInterrupt_Disable
********************************************************************************
*
* Summary:
*   Disables the Interrupt in the interrupt controller.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void WiFiInterrupt_Disable(void)
{
    /* Disable the general interrupt. */
    *WiFiInterrupt_INTC_CLR_EN = WiFiInterrupt__INTC_MASK;
}


/*******************************************************************************
* Function Name: WiFiInterrupt_SetPending
********************************************************************************
*
* Summary:
*   Causes the Interrupt to enter the pending state, a software method of
*   generating the interrupt.
*
* Parameters:
*   None
*
* Return:
*   None
*
* Side Effects:
*   If interrupts are enabled and the interrupt is set up properly, the ISR is
*   entered (depending on the priority of this interrupt and other pending 
*   interrupts).
*
*******************************************************************************/
void WiFiInterrupt_SetPending(void)
{
    *WiFiInterrupt_INTC_SET_PD = WiFiInterrupt__INTC_MASK;
}


/*******************************************************************************
* Function Name: WiFiInterrupt_ClearPending
********************************************************************************
*
* Summary:
*   Clears a pending interrupt in the interrupt controller.
*
*   Note Some interrupt sources are clear-on-read and require the block 
*   interrupt/status register to be read/cleared with the appropriate block API 
*   (GPIO, UART, and so on). Otherwise the ISR will continue to remain in 
*   pending state even though the interrupt itself is cleared using this API.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void WiFiInterrupt_ClearPending(void)
{
    *WiFiInterrupt_INTC_CLR_PD = WiFiInterrupt__INTC_MASK;
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
