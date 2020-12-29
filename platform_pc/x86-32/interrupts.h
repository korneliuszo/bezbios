/*
 * interrupts.hpp
 *
 *  Created on: Dec 10, 2020
 *      Author: Korneliusz Osmenda
 */

#ifndef PLATFORM_PC_X86_32_INTERRUPTS_H_
#define PLATFORM_PC_X86_32_INTERRUPTS_H_

#ifdef __cplusplus
extern "C" {
#endif

void bezbios_init_interrupts(void);

void bezbios_hwirq_insert(unsigned char irqn,void (*irqfn)(unsigned char));
void bezbios_int_ack(unsigned char IRQ);
void bezbios_disable_irq(unsigned char IRQline);
void bezbios_enable_irq(unsigned char IRQline);
long bezbios_get_ms();
void init_PIT();

#define BEZBIOS_INIT_PIT() \
		__attribute((constructor)) \
		static \
		void PIT_init() \
		{ \
			init_PIT(); \
		}

#ifdef __cplusplus
}
#endif

void bezbios_irq_idt(unsigned char irqn,void (*irqfn)(struct interrupt_frame *), unsigned char dpl=0, bool trap=false);


#endif /* PLATFORM_PC_X86_32_INTERRUPTS_H_ */
