/*
 * crt0.cc
 *
 *  Created on: Nov 16, 2020
 *      Author: Korneliusz Osmenda
 */

extern "C" {

#include "tss.h"
}

typedef struct
{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed)) gdt_table_t;

static_assert(sizeof(gdt_table_t) == 8, "Verifying size failed!");


#define GDT_BASE_LOW(x) (x & 0xFFFF)
#define GDT_BASE_MID(x) ((x>>16) & 0xFF)
#define GDT_BASE_HIGH(x) ((x>>(16+8)) & 0xFF)

__attribute__((section(".data")))
TSS_io tss_io;

#define GDT_ACCES_CXR_0 (1<<7 | 0 << 5 | 1<<4 | 1 <<3 | 0 << 2 | 1 << 1 | 0 << 0)
#define GDT_ACCES_DRW_0 (1<<7 | 0 << 5 | 1<<4 | 0 <<3 | 0 << 2 | 1 << 1 | 0 << 0)

#define GDT_LIMIT_LOW_4K(x) ((x>>12) & 0xFFFF)
#define GDT_LIMIT_HIGH_4K(x) ((x>>(12+16)) & 0xF)
#define GDT_LIMIT_LOW_1B(x) ((x) & 0xFFFF)
#define GDT_LIMIT_HIGH_1B(x) (((x)>>(16)) & 0xF)
#define GDT_GRANULARITY_4K_32 (1<<7 | 1 << 6)

static
gdt_table_t gdt_table[] =
{
		{0,0,0,0,0,0}, //null descriptor
		{
			GDT_LIMIT_LOW_4K(0xFFFFFFFF),
			GDT_BASE_LOW(0x00000000),
			GDT_BASE_MID(0x00000000),
			GDT_ACCES_CXR_0,
			GDT_LIMIT_HIGH_4K(0xFFFFFFFF)|
			GDT_GRANULARITY_4K_32,
			GDT_BASE_HIGH(0x00000000)
		}, //code descriptor
		{
			GDT_LIMIT_LOW_4K(0xFFFFFFFF),
			GDT_BASE_LOW(0x00000000),
			GDT_BASE_MID(0x00000000),
			GDT_ACCES_DRW_0,
			GDT_LIMIT_HIGH_4K(0xFFFFFFFF)|
			GDT_GRANULARITY_4K_32,
			GDT_BASE_HIGH(0x00000000)
		}, //data descriptor
		{
			GDT_LIMIT_LOW_1B(sizeof(TSS)-1),
			0,
			0,
			(0<<7)|(0<<5)|(1<<3)|(1<<0), //TSS
			GDT_LIMIT_HIGH_1B(sizeof(TSS)-1)|
			0x40,
			0,
		}, //v86 TSS descriptor
};

struct  __attribute__((packed)) Gdt_pointer
{
	unsigned short limit;
	gdt_table_t * table;
};

static_assert(sizeof(Gdt_pointer) == 6, "Verifying size failed!");

extern "C"
{
Gdt_pointer gdt_pointer = {sizeof(gdt_table)-1,gdt_table};

__attribute__((cdecl))
__attribute__((section(".init")))
void tss_init(void)
{
	tss_io.t.io_base = (DWORD)(&(tss_io.io_map)) - (DWORD)(&(tss_io));
    for (int i = 0; i < 2047; i++) tss_io.io_map[i] = 0;
    tss_io.io_map[2047] = 0xFF000000;


    gdt_table[3].base_low = GDT_BASE_LOW((unsigned long)&tss_io.t);
    gdt_table[3].base_middle = GDT_BASE_MID((unsigned long)&tss_io.t);
    gdt_table[3].base_high = GDT_BASE_HIGH((unsigned long)&tss_io.t);
    gdt_table[3].access |= 1<<7; //now present
    asm volatile("lgdt %P0 " :  : "i"(&gdt_pointer));
//	asm volatile("ltr %w0" : : "r"(0x18));
}
}

void tss_clear_busy(void)
{
	asm volatile("cli");
	gdt_table[3].access &= 0xfd;
    asm volatile("lgdt %P0 " :  : "i"(&gdt_pointer));
    asm volatile(
    		"jmp $0x08,$tss_clear_jmp\n\t"
    		"tss_clear_jmp:");
    asm volatile("sti");
}
