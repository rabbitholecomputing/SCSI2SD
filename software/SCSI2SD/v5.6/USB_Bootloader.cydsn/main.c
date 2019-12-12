//	Copyright (C) 2013 Michael McMaster <michael@codesrc.com>
//

#include <project.h>


static void resetSCSI()
{
	// active low
	CyPins_SetPin(LED_0);
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
		BL_SET_RUN_TYPE(BL_START_BTLDR);
	}

	// The call to the bootloader should not return
    BL_Start();

	return 0;
}

