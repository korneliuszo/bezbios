/*
 * stack_protector_iface.c
 *
 *  Created on: Nov 23, 2020
 *      Author: Korneliusz Osmenda
 */

__attribute__((weak,noreturn))
__attribute__((noinline, used))
void __stack_chk_fail(void)
{
	asm("cli");
	while(1)
	{
		asm("hlt");
		asm("nop");
	}
}

