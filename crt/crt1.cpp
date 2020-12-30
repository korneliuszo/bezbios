/*
 * crt1.cpp
 *
 *  Created on: Nov 17, 2020
 *      Author: Korneliusz Osmenda
 */

/*
 * crt0.cc
 *
 *  Created on: Nov 16, 2020
 *      Author: Korneliusz Osmenda
 */

#include <interrupts.h>
#include <uart/bezbios_uart_api_low.h>

extern "C" {

//#include "memgccuse.h"

extern char __bss_start;
extern char __bss_end;

void
__attribute__((used))
__attribute__((noreturn))
__attribute__((section(".init")))
_cstart(void);

typedef struct {
	unsigned long magic;
	unsigned long flags;
	unsigned long header_addr;
	unsigned long load_addr;
	unsigned long load_end_addr;
	unsigned long bss_end_addr;
	unsigned long entry_addr;
	unsigned long mode_type;
	unsigned long width;
	unsigned long height;
	unsigned long depth;


}multiboot_t;

extern multiboot_t Multiboot_header;

void bezbios_main(void);

}


__attribute__((section(".multiboot")))
multiboot_t Multiboot_header = {
		0x1BADB002,
		1,
		0x100000000 - 0x1BADB002 - 1,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
};

/* These magic symbols are provided by the linker.  */
extern void (*__init_array_start []) (void) __attribute__((weak));
extern void (*__init_array_end []) (void) __attribute__((weak));
extern void (*__fini_array_start []) (void) __attribute__((weak));
extern void (*__fini_array_end []) (void) __attribute__((weak));

void
__attribute__((used))
__attribute__((noreturn))
__attribute__((section(".init")))
_cstart(void) {
	for(volatile char * i = &__bss_start; i < &__bss_end; i++)
		*i = 0x00;


	long i, count;

	bezbios_serial_bootup_init();

	bezbios_init_interrupts();

	count = __init_array_end - __init_array_start;
	for (i = 0; i < count; i++)
	__init_array_start[i] ();

	bezbios_main();

	count = __fini_array_end - __fini_array_start;
	for (i = 0; i < count; i++)
	__fini_array_start[i] ();

	asm("cli");
	while(1)
	{
		asm("hlt");
		asm("nop");
	}
	//finished
}

