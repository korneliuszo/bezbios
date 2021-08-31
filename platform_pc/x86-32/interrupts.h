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
	unsigned long error_code;
	unsigned long eip;
	unsigned long cs;
	unsigned long eflags;
};
static_assert(sizeof(Error_stack) == 60, "Verifying size failed!");

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
	unsigned long eip;
	unsigned long cs;
	unsigned long eflags;
};
static_assert(sizeof(Isr_stack) == 56, "Verifying size failed!");

void gpfC (struct Error_stack *frame);

template<typename STACK, void CFUN(STACK *)> class ISR;

template<void CFUN(Isr_stack *)>
class ISR<Isr_stack,CFUN> {
public:
	static void fn(void) __attribute__((naked))
	{
		asm(
				".cfi_def_cfa_offset 4\n\t"
				"push %%eax\n\t"
				".cfi_adjust_cfa_offset 4\n\t"
				"push %%ecx\n\t"
				".cfi_adjust_cfa_offset 4\n\t"
				"push %%edx\n\t"
				".cfi_adjust_cfa_offset 4\n\t"
				"push %%ebx\n\t"
				".cfi_adjust_cfa_offset 4\n\t"
				"push %%ebp\n\t"
				".cfi_adjust_cfa_offset 4\n\t"
				"push %%esi\n\t"
				".cfi_adjust_cfa_offset 4\n\t"
				"push %%edi\n\t"
				".cfi_adjust_cfa_offset 4\n\t"
				"push %%ds\n\t"
				".cfi_adjust_cfa_offset 4\n\t"
				"push %%es\n\t"
				".cfi_adjust_cfa_offset 4\n\t"
				"push %%fs\n\t"
				".cfi_adjust_cfa_offset 4\n\t"
				"push %%gs\n\t"
				".cfi_adjust_cfa_offset 4\n\t"
				"mov $0x10, %%eax\n\t"
				"mov %%eax, %%ds\n\t"
				"mov %%eax, %%es\n\t"
				"mov %%eax, %%fs\n\t"
				"mov %%eax, %%gs\n\t"
				"movl %%esp, %%eax\n\t"
				"push %%eax\n\t"
				".cfi_adjust_cfa_offset 4\n\t"
				"call %P[CFUN]\n\t"
				"addl $4, %%esp\n\t"
				".cfi_adjust_cfa_offset -4\n\t"
				"pop %%gs\n\t"
				".cfi_adjust_cfa_offset -4\n\t"
				"pop %%fs\n\t"
				".cfi_adjust_cfa_offset -4\n\t"
				"pop %%es\n\t"
				".cfi_adjust_cfa_offset -4\n\t"
				"pop %%ds\n\t"
				".cfi_adjust_cfa_offset -4\n\t"
				"pop %%edi\n\t"
				".cfi_adjust_cfa_offset -4\n\t"
				"pop %%esi\n\t"
				".cfi_adjust_cfa_offset -4\n\t"
				"pop %%ebp\n\t"
				".cfi_adjust_cfa_offset -4\n\t"
				"pop %%ebx\n\t"
				".cfi_adjust_cfa_offset -4\n\t"
				"pop %%edx\n\t"
				".cfi_adjust_cfa_offset -4\n\t"
				"pop %%ecx\n\t"
				".cfi_adjust_cfa_offset -4\n\t"
				"pop %%eax\n\t"
				".cfi_adjust_cfa_offset -4\n\t"
				"iret\n\t"
		:
		: [CFUN]"i"(CFUN)
		);
	};
};

template<void CFUN(Error_stack *)>
class ISR<Error_stack, CFUN> {
public:
	static void fn(void) __attribute__((naked))
	{
		asm(
				".cfi_def_cfa_offset 8\n\t"
				"push %%eax\n\t"
				".cfi_adjust_cfa_offset 4\n\t"
				"push %%ecx\n\t"
				".cfi_adjust_cfa_offset 4\n\t"
				"push %%edx\n\t"
				".cfi_adjust_cfa_offset 4\n\t"
				"push %%ebx\n\t"
				".cfi_adjust_cfa_offset 4\n\t"
				"push %%ebp\n\t"
				".cfi_adjust_cfa_offset 4\n\t"
				"push %%esi\n\t"
				".cfi_adjust_cfa_offset 4\n\t"
				"push %%edi\n\t"
				".cfi_adjust_cfa_offset 4\n\t"
				"push %%ds\n\t"
				".cfi_adjust_cfa_offset 4\n\t"
				"push %%es\n\t"
				".cfi_adjust_cfa_offset 4\n\t"
				"push %%fs\n\t"
				".cfi_adjust_cfa_offset 4\n\t"
				"push %%gs\n\t"
				".cfi_adjust_cfa_offset 4\n\t"
				"mov $0x10, %%eax\n\t"
				"mov %%eax, %%ds\n\t"
				"mov %%eax, %%es\n\t"
				"mov %%eax, %%fs\n\t"
				"mov %%eax, %%gs\n\t"
				"movl %%esp, %%eax\n\t"
				"push %%eax\n\t"
				".cfi_adjust_cfa_offset 4\n\t"
				"call %P[CFUN]\n\t"
				"addl $4, %%esp\n\t"
				".cfi_adjust_cfa_offset -4\n\t"
				"pop %%gs\n\t"
				".cfi_adjust_cfa_offset -4\n\t"
				"pop %%fs\n\t"
				".cfi_adjust_cfa_offset -4\n\t"
				"pop %%es\n\t"
				".cfi_adjust_cfa_offset -4\n\t"
				"pop %%ds\n\t"
				".cfi_adjust_cfa_offset -4\n\t"
				"pop %%edi\n\t"
				".cfi_adjust_cfa_offset -4\n\t"
				"pop %%esi\n\t"
				".cfi_adjust_cfa_offset -4\n\t"
				"pop %%ebp\n\t"
				".cfi_adjust_cfa_offset -4\n\t"
				"pop %%ebx\n\t"
				".cfi_adjust_cfa_offset -4\n\t"
				"pop %%edx\n\t"
				".cfi_adjust_cfa_offset -4\n\t"
				"pop %%ecx\n\t"
				".cfi_adjust_cfa_offset -4\n\t"
				"pop %%eax\n\t"
				".cfi_adjust_cfa_offset -4\n\t"
				"addl $4, %%esp\n\t"
				".cfi_def_cfa_offset 4\n\t"
				"iret\n\t"
		:
		: [CFUN]"i"(CFUN)
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
template<typename STACK, void CFUN(STACK *)>
void bezbios_irq_idt(unsigned char irqn,ISR<STACK,CFUN> *irq, unsigned char dpl=0, bool trap=false);

template<unsigned char IRQ>
class bezbios_imp_hw_req {
public:
    static void f(Isr_stack *);
};


#endif /* PLATFORM_PC_X86_32_INTERRUPTS_H_ */
