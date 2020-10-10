//	Copyright (C) 2013 Michael McMaster <michael@codesrc.com>
//
//	This file is part of SCSI2SD.
//
//	SCSI2SD is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	SCSI2SD is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with SCSI2SD.  If not, see <http://www.gnu.org/licenses/>.
#include <project.h>

static void resetSCSI()
{
	CyPins_SetPin(TERM_EN_0); // Active low
	
	CyPins_ClearPin(SCSI_Out_IO_raw);
	CyPins_ClearPin(SCSI_Out_BSY);
	CyPins_ClearPin(SCSI_Out_RST);
	CyPins_ClearPin(SCSI_Out_SEL);
	CyPins_ClearPin(SCSI_Out_REQ);
	CyPins_ClearPin(SCSI_Out_MSG);
	CyPins_ClearPin(SCSI_Out_CD);
	CyPins_ClearPin(SCSI_Out_DBx_DB0);
	CyPins_ClearPin(SCSI_Out_DBx_DB1);
	CyPins_ClearPin(SCSI_Out_DBx_DB2);
	CyPins_ClearPin(SCSI_Out_DBx_DB3);
	CyPins_ClearPin(SCSI_Out_DBx_DB4);
	CyPins_ClearPin(SCSI_Out_DBx_DB5);
	CyPins_ClearPin(SCSI_Out_DBx_DB6);
	CyPins_ClearPin(SCSI_Out_DBx_DB7);
	CyPins_ClearPin(SCSI_Out_DBP_raw);

	// active low
	CyPins_SetPin(LED_0);
	CyPins_SetPin(LED_1);
}

int main()
{
    resetSCSI();
	
	// We need some delay before reading the BOOTLDR pin gives us the correct
	// answer. I don't know if it's due to the pullup enabled, or use of the 
	// 32k xtal pins as GPIO immediately after power on.
	// Try to startup as fast as possible in the normal case of no-bootloader-jumper
	int limit = 100;
	while (limit > 0 && BOOTLDR_Read() == 0) // active low
	{
		CyDelayUs(100);
		limit--;
	}
	
	if (BOOTLDR_Read() == 0)
	{
		CyPins_ClearPin(LED_0);
		CyPins_ClearPin(LED_1);
		BL_SET_RUN_TYPE(BL_START_BTLDR);
	}

	// The call to the bootloader should not return
    BL_Start();

    /* CyGlobalIntEnable; */ /* Uncomment this line to enable global interrupts. */
    for(;;)
    {
		CyPins_ClearPin(LED_0);
		CyPins_ClearPin(LED_1);
        /* Place your application code here. */
    }
	return 0;
}

