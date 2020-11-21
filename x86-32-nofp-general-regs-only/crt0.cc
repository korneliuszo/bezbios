/*
 * crt0.cc
 *
 *  Created on: Nov 16, 2020
 *      Author: Korneliusz Osmenda
 */

extern "C" {

extern int __stack_end;

void
__attribute__((used))
__attribute__((naked))
__attribute__((section(".init"))) _start(void);


void
__attribute__((used))
__attribute__((naked))
__attribute__((section(".init"))) _pstart(void);
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

#define GDT_BASE_LOW(x) (x &0xFFFF)
#define GDT_BASE_MID(x) ((x>>16) &0xFF)
#define GDT_BASE_HIGH(x) ((x>>(16+8)) &0xFF)

#define GDT_ACCES_CXR_0 (1<<7 | 0 << 5 | 1<<4 | 1 <<3 | 0 << 2 | 1 << 1 | 0 << 0)
#define GDT_ACCES_DRW_0 (1<<7 | 0 << 5 | 1<<4 | 0 <<3 | 0 << 2 | 1 << 1 | 0 << 0)

#define GDT_LIMIT_LOW_4K(x) ((x>>12) & 0xFFFF)
#define GDT_LIMIT_HIGH_4K(x) ((x>>(12+16)) & 0xF)
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
};

static
const
__attribute__((used))
struct
{
	unsigned short limit;
	gdt_table_t * table;
} __attribute__((packed)) gdt_pointer
= {sizeof(gdt_table)-1,gdt_table};

void
__attribute__((used))
__attribute__((naked))
__attribute__((section(".init"))) _start(void) {
	asm volatile("lgdt (%0) " :  : "r"(&gdt_pointer));
#ifdef CONFIG_FROM_REAL_MODE
	asm (
		"mov %%cr0, %%eax\n\t"
			"orb $1, %%al\n\t"      // set PE (Protection Enable) bit in CR0 (Control Register 0)
			"mov %%eax, %%cr0" : : : "eax");
#endif
	asm volatile("jmp $0x08,$_pstart");
}

void
__attribute__((used))
__attribute__((naked))
__attribute__((section(".init"))) _pstart(void) {
	asm volatile(
			"movw $0x10, %%ax\n\t"
			"movw %%ax, %%ds\n\t"
			"movw %%ax, %%es\n\t"
			"movw %%ax, %%fs\n\t"
			"movw %%ax, %%gs\n\t"
			"movw %%ax, %%ss\n\t"
			: : : "ax");
	asm volatile("movl %0, %%esp" : : "i"(&__stack_end));
	asm volatile("movl %0, %%ebp" : : "i"(&__stack_end));
	asm volatile("jmp _cstart");
}
