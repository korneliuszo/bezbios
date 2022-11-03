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

class BEZBIOS_INIT_PIT {
public:
	BEZBIOS_INIT_PIT()
	{
		init_PIT();
	}
};

#ifdef __cplusplus
}
#endif

struct __attribute__((packed)) Error_stack {
	unsigned long gs;
	unsigned long fs;
	unsigned long es;
	unsigned long ds;
	unsigned long edi;
	unsigned long esi;
	unsigned long ebp;
	unsigned long ebx;
	unsigned long edx;
	unsigned long ecx;
	unsigned long eax;
	unsigned long irq;
	unsigned long error_code;
	unsigned long eip;
	unsigned long cs;
	unsigned long eflags;
};
static_assert(sizeof(Error_stack) == 64, "Verifying size failed!");

struct __attribute__((packed)) Isr_stack {
	unsigned long gs;
	unsigned long fs;
	unsigned long es;
	unsigned long ds;
	unsigned long edi;
	unsigned long esi;
	unsigned long ebp;
	unsigned long ebx;
	unsigned long edx;
	unsigned long ecx;
	unsigned long eax;
	unsigned long irq;
	unsigned long eip;
	unsigned long cs;
	unsigned long eflags;
};
static_assert(sizeof(Isr_stack) == 60, "Verifying size failed!");

void gpfC (struct Error_stack *frame);
void bezbios_irq_handler(void) __attribute__((naked));
void bezbios_irq_C_handler(Isr_stack *stack) __attribute__((cdecl));

template<typename STACK, int IRQ_NO> class ISR;

template<int IRQ_NO>
class ISR<Isr_stack,IRQ_NO> {
public:
	static void fn(void) __attribute__((naked))
	{
		asm(
				".cfi_def_cfa_offset 4\n\t"
				"push %[IRQ_NO]\n\t"
				"jmp %P[FUN]\n\t"
		:
		: [IRQ_NO]"i"(IRQ_NO), [FUN]"i"(bezbios_irq_handler)
		);
	};
};
void bezbios_err_handler(void) __attribute__((naked));
void bezbios_err_C_handler(Error_stack *stack) __attribute__((cdecl));

template<int IRQ_NO>
class ISR<Error_stack, IRQ_NO> {
public:
	static void fn(void) __attribute__((naked))
	{
		asm(
				".cfi_def_cfa_offset 8\n\t"
				"push %[IRQ_NO]\n\t"
				"jmp %P[FUN]\n\t"
		:
		: [IRQ_NO]"i"(IRQ_NO), [FUN]"i"(bezbios_err_handler)
		);
	};
};


/*
template<>
class ISR<void (Error_stack *)> {
public:
	static void fn(void) asm
	(
		"call %[cfun]"
	);
};
template<>
class ISR<void (Isr_stack *)> {
public:
	static void fn(void) asm
	(
		"call %[cfun]"
	);
};
*/

void register_isr(int irq,void(*fn)(Isr_stack*));


#endif /* PLATFORM_PC_X86_32_INTERRUPTS_H_ */
