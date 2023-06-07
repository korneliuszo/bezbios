/*
 * interrupts.cpp
 *
 *  Created on: Dec 9, 2020
 *      Author: Korneliusz Osmenda
 */

#include "io.h"
#include "interrupts.h"

static ByteIO<0x20> PIC1_CMD;
static ByteIO<0x21> PIC1_DAT;
static ByteIO<0xA0> PIC2_CMD;
static ByteIO<0xA1> PIC2_DAT;

struct __attribute__((packed, aligned(8))) IDTDescr {
   volatile unsigned short offset_1; // offset bits 0..15
   volatile unsigned short selector; // a code segment selector in GDT or LDT
   volatile unsigned char zero;      // unused, set to 0
   volatile unsigned char type_attr; // type and attributes, see below
   volatile unsigned short offset_2; // offset bits 16..31
};

static_assert(sizeof(IDTDescr) == 8, "Verifying size failed!");

static struct IDTDescr idt[256] ={};

struct __attribute__((packed)) IDTSegment
{
	unsigned short len;
	struct IDTDescr *idt;
};

static_assert(sizeof(IDTSegment) == 6, "Verifying size failed!");

static struct IDTSegment idt_seg = {256*sizeof(*idt)-1, idt};

static void bezbios_irq_idt(unsigned char irqn, void(*irq)(void), unsigned char dpl=0, bool trap=false)
{
	if(irq == nullptr)
	{
		idt[irqn].type_attr = 0;
		idt[irqn].selector = 0;
		idt[irqn].offset_1 = 0;
		idt[irqn].offset_2 = 0;
	}
	else
	{
		idt[irqn].selector = 0x08; // see GDT table
		idt[irqn].offset_1 = ((unsigned long)irq)&0xffff;
		idt[irqn].offset_2 = ((unsigned long)irq)>>16;
		idt[irqn].type_attr = 0x80 | dpl << 5 | (trap?0xf:0xe);
	}
}

void bezbios_int_ack(unsigned char IRQ)
{
	if(IRQ >= 8)
		PIC2_CMD = 0x20; //EOI
	PIC1_CMD = 0x20; //EOI
}

static void bezbios_int_default(Isr_stack *stack)
{
    	bezbios_int_ack(stack->irq); //eat spurious interrupts
}

void bezbios_irq_handler(void)
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
			".cfi_adjust_cfa_offset -4\n\t"
			"iret\n\t"
	:
	: [CFUN]"i"(bezbios_irq_C_handler)
	);

}

void bezbios_err_handler(void)
{
	asm(
			".cfi_def_cfa_offset 12\n\t"
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
			"addl $8, %%esp\n\t"
			".cfi_adjust_cfa_offset -8\n\t"
			"iret\n\t"
	:
	: [CFUN]"i"(bezbios_err_C_handler)
	);
};

void bezbios_err_C_handler(Error_stack *stack)
{
	gpfC(stack);
}


static void(* volatile irq_handler[16])(Isr_stack*);

void bezbios_irq_C_handler(Isr_stack *stack)
{
	if (stack->irq < 16)
		irq_handler[stack->irq](stack);
	else if (stack->irq == 0x0103)
		int3isr(stack);
}

void register_isr(int irq,void(*fn)(Isr_stack*))
{
	ENTER_ATOMIC();
	irq_handler[irq]=fn;
	EXIT_ATOMIC();
}

template<int I>
class _hwirqloop_init {
public:
    static inline void f()
    {
    	unsigned char irq_offset=(I<8)?0x20:(0x28-8);
    	bezbios_irq_idt(I+irq_offset,ISR<Isr_stack,I>::fn);
        _hwirqloop_init<I+1>::f();
    }
};
template<>
class _hwirqloop_init<16> {
public:
    static inline void f()
    { }
};

void bezbios_init_interrupts(void)
{
	PIC1_CMD = 0x10 | 0x01; //init and do 4word initialization
	PIC2_CMD = 0x10 | 0x01;
	io_wait(0x01);
	PIC1_DAT = 0x20; //0x20-0x27 - IRQ 0-7
	PIC2_DAT = 0x28; //0x28-0x2f - IRQ 8-15
	io_wait(0x01);
	PIC1_DAT = 4; // slave on IRQ2
	PIC2_DAT = 2; // master number 2
	io_wait(0x01);
	PIC1_DAT = 0x01; // 8086 mode
	PIC2_DAT = 0x01; //
	io_wait(0x01);
	PIC1_DAT = 0xff; // disable all interrupts
	PIC2_DAT = 0xff;

	_hwirqloop_init<0>::f();
	for(int i=0;i<16;i++)
		register_isr(i,bezbios_int_default);

	bezbios_irq_idt(0x0D,ISR<Error_stack,0x0D>::fn);
	bezbios_irq_idt(3,ISR<Isr_stack,0x0103>::fn);

	asm volatile("lidt (%0) " :  : "r"(&idt_seg));
	asm volatile("sti"); //now we can start interrupts
}

void bezbios_disable_irq(unsigned char IRQline) {
    if(IRQline < 8) {
        PIC1_DAT|= (1 << IRQline);
    } else {
        PIC2_DAT|= (1 << (IRQline-8));
    }
}

void bezbios_enable_irq(unsigned char IRQline) {
    if(IRQline < 8) {
        PIC1_DAT&= ~(1 << IRQline);
    } else {
        PIC2_DAT&= ~(1 << (IRQline-8));
    }
}

