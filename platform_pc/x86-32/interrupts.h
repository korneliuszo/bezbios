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

void bezbios_int_ack(unsigned char IRQ);
void bezbios_disable_irq(unsigned char IRQline);
void bezbios_enable_irq(unsigned char IRQline);
long bezbios_get_ms();
void init_PIT();

void bezbios_delay_ms(int delay);

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

template<unsigned char IRQ>
class bezbios_imp_hw_req {
public:
    static void f(struct interrupt_frame *);
};

#endif /* PLATFORM_PC_X86_32_INTERRUPTS_H_ */
