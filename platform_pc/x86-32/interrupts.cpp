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

struct __attribute__((packed)) IDTDescr {
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

template<typename STACK, void CFUN(STACK *)>
void bezbios_irq_idt(unsigned char irqn,ISR<STACK,CFUN> *irq, unsigned char dpl, bool trap)
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
		idt[irqn].offset_1 = ((unsigned long)&irq->fn)&0xffff;
		idt[irqn].offset_2 = ((unsigned long)&irq->fn)>>16;
		idt[irqn].type_attr = 0x80 | dpl << 5 | (trap?0xf:0xe);
	}
}

void bezbios_int_ack(unsigned char IRQ)
{
	if(IRQ >= 8)
		PIC2_CMD = 0x20; //EOI
	PIC1_CMD = 0x20; //EOI
}

template<int I>
class _hwirqloop_init {
public:
    static inline void f()
    {
    	unsigned char irq_offset=(I<8)?0x20:(0x28-8);
    	ISR<Isr_stack,bezbios_imp_hw_req<I>::f> irq;
    	bezbios_irq_idt(I+irq_offset,&irq);
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
	static ISR<Error_stack,gpfC> gpf_isr;
	bezbios_irq_idt(0x0D,&gpf_isr);

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

