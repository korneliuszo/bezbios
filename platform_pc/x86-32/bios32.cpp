/*
 * bios32.cpp
 *
 *  Created on: 29 gru 2022
 *      Author: nyuu
 */

#include "bios32.hpp"
#include "io.h"

BIOS32 & BIOS32::get_handle()
{
	static BIOS32 bios32;
	return bios32;
}

BIOS32::BIOS32()
{
	union Entry{
	struct __attribute__((packed)){
		uint32_t sig;
		uint32_t ep;
		uint8_t rev;
		uint8_t len;
		uint8_t chksum;
		uint8_t resvd[5];
	} s;
	uint8_t chksum[16];
	} *entry;

	for(entry = (union Entry*)0xe0000; entry< (union Entry*)0xFFFFF;entry++)
	{
		if(entry->s.sig != (('_' << 0) + ('3' << 8) + ('2' << 16) + ('_' << 24)))
			continue;
		if(entry->s.len != 1)
			continue;
		uint8_t sum = 0;
		for(int i=0;i<16;i++)
			sum+=entry->chksum[i];
		if(sum)
			continue;
		if(entry->s.rev !=0)
			continue;
		b32_entry = entry->s.ep;
		return;
	}
	b32_entry = 0;
}

unsigned long BIOS32::get_service_entry(uint32_t service)
{
	if(!b32_entry)
		return 0;

	unsigned char return_code;	/* %al */
	unsigned long address;		/* %ebx */
	unsigned long length;		/* %ecx */
	unsigned long entry;		/* %edx */

	ENTER_ATOMIC();

	static struct {
		unsigned long address;
		unsigned short segment;
	} bios32_indirect;

	bios32_indirect.address=b32_entry;
	bios32_indirect.segment = 0x8;

	asm("lcall *(%p[entry])\n\t cld"
		: "=a" (return_code),
		  "=b" (address),
		  "=c" (length),
		  "=d" (entry)
		: "0" (service),
		  "1" (0),
		  [entry] "i" (&bios32_indirect));
	EXIT_ATOMIC();

	if(return_code)
		return 0;
	return address+entry;
}

