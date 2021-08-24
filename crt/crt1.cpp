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
extern char __sloram_copy;
extern char __eloram_copy;
extern char __loram_copy;


void
__attribute__((noreturn))
__attribute__((section(".init")))
_cstart(void);

void bezbios_main(void);

}

/* These magic symbols are provided by the linker.  */
extern void (*__init_array_start []) (void) __attribute__((weak));
extern void (*__init_array_end []) (void) __attribute__((weak));
extern void (*__fini_array_start []) (void) __attribute__((weak));
extern void (*__fini_array_end []) (void) __attribute__((weak));

void
__attribute__((noreturn))
__attribute__((section(".init")))
_cstart(void) {
	for(volatile char * i = &__bss_start; i < &__bss_end; i++)
		*i = 0x00;

	long *pSrc, *pDest;

    pSrc = (long*)&__loram_copy;
    pDest = (long*)&__sloram_copy;

    if (pSrc != pDest) {
            for (; pDest < (long*)&__eloram_copy;) {
                    *pDest++ = *pSrc++;
            }
    }

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

