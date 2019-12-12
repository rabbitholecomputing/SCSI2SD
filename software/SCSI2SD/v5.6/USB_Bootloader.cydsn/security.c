//	Copyright (C) 2018 Michael McMaster <michael@codesrc.com>
//
// Compiled with -mpure-code
// ... actually, not supported in gcc version :-(
// So use -mslow-flash-data instead, which seems to do the job.
// See custom linker script and flash protection settings.

#include <project.h>

#include "tiny-AES-c-master/aes.h"

// This method will be LOCKED down
void  __attribute__((section(".codesrc_secure"))) codesrcDecrypt(
	const uint8_t* iv,
	uint8_t* buf,
	uint32_t count)
{
	// Need to populate the key from constants stored in this protected section
	// of flash.
	uint8_t key[16];
	key[0] = 0x5b;
	key[1] = 0x02;
	key[2] = 0xfa;
	key[3] = 0x4a;
	key[4] = 0x00;
	key[5] = 0xbf;
	key[6] = 0x97;
	key[7] = 0xc1;
	key[8] = 0xb4;
	key[9] = 0x62;
	key[10] = 0xb7;
	key[11] = 0x4b;
	key[12] = 0x3f;
	key[13] = 0x38;
	key[14] = 0xaf;
	key[15] = 0x25;

	struct AES_ctx ctx;
	AES_init_ctx_iv(&ctx, key, iv);
	for (int i = 0; i < 16; ++i) key[i] = 0;
	
	// Decrypt the given buffer
	// New scsi2sd calls proxy, passes it here.
	// and in this way, it can do a licence check
	AES_CBC_decrypt_buffer(&ctx, buf, count);
	
	// A simple memset here gets optimised out. But we need to clear
	// out the key from the context.
	AES_init_ctx_iv(&ctx, key, iv);
}

