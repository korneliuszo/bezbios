/*
 * pci.cpp
 *
 *  Created on: 20 gru 2022
 *      Author: nyuu
 */

#include "pci.hpp"
#include "e820.hpp"
#include "vm86.hpp"
#include "bios32.hpp"
#include "io.h"

#include <uart/tlay2_dbgout.hpp>

static constexpr uint8_t PCI_IRQ = 10;

Pci & Pci::get_handle()
{
	static Pci pci;
	return pci;
}

bool Pci::pci_support()
{
	{
		DbgOut<UartBlocking> sender;
		sender.str("PCI support").end();
	}
	uint32_t signature, eax, ebx, ecx;
	static struct {
		unsigned long address;
		unsigned short segment;
	} bios32_indirect;

	{
	ENTER_ATOMIC();

	bios32_indirect.address=pci32_entry;
	bios32_indirect.segment = 0x8;

	asm("lcall *(%p[entry])\n\t cld\n\t"
		"jc 1f\n\t"
		"xor %%ah, %%ah\n"
		"1:"
		: "=d" (signature),
		  "=a" (eax),
		  "=b" (ebx),
		  "=c" (ecx)
		: "1" (0xb101),
		  [entry] "i" (&bios32_indirect)
		: "esi","edi");
	EXIT_ATOMIC();
	}
	uint8_t status = (eax >> 8) & 0xff;

	if(signature != (('P' << 0) + ('C' << 8) + ('I' << 16) + (' ' << 24)))
		return false;
	if(status)
		return false;
	if(!(eax & 0x01))
		return false;

	{
		static uint8_t pir[4096];
		struct irq_routing_options {
			uint16_t size;
			uint8_t *table;
			uint16_t segment;
		} __attribute__((packed));
		irq_routing_options pirt = {sizeof(pir),pir,0x10};

		ENTER_ATOMIC();

		bios32_indirect.address=pci32_entry;
		bios32_indirect.segment = 0x8;

		asm ("push %%es\n\t"
			"push %%ds\n\t"
			"pop  %%es\n\t"
			"lcall *(%p[entry])\n\t cld\n\t"
			"pop %%es\n\t"
			//"jc 1f\n\t"
			//"xor %%ah, %%ah\n"
			//"1:"
			: "=a" (eax),
			  "=b" (ebx),
			  "=c" (ecx)
			: "D" (&pirt),
			  "0" (0xb10e),
			  "1" (0),
			  [entry] "i" (&bios32_indirect)
			: "edx", "esi", "memory");
		EXIT_ATOMIC();

		DbgOut<UartBlocking> sender;

		sender.str("PCI ").hex(eax).end();
		sender.str("PCI ").hex(ebx).end();

	}

	return true;
}

Pci::Pci()
{
	pci32_entry = BIOS32::get_handle().get_service_entry((('$' << 0) + ('P' << 8) + ('C' << 16) + ('I' << 24)));
	if (!pci32_entry)
		return;
	if(!pci_support())
		return;
	mmio_pool = e820_scan_hole(0x1000000,UINT32_MAX);
	if (!mmio_pool.len)
		return;

	register_isr(PCI_IRQ,Pci::isr_c);
	bezbios_enable_irq(PCI_IRQ);

	avalible = true;
	return;
}

void Pci::isr_c(Isr_stack* stack)
{
	Pci::get_handle().isr();
	bezbios_int_ack(stack->irq);
}

void Pci::isr()
{
	if(!drivers)
		return;
	if(drivers->isr)
		drivers->isr(drivers->isr_arg);
	for(Pci_driver * el=drivers->next;el!=drivers;el=el->next)
	{
		if(el->isr)
			el->isr(el->isr_arg);
	}
}

bool Pci::link_irq(const Pci_no & dev,uint8_t intline)
{
	unsigned long ret;
	ENTER_ATOMIC();

	static struct {
		unsigned long address;
		unsigned short segment;
	} bios32_indirect;

	bios32_indirect.address=pci32_entry;
	bios32_indirect.segment = 0x08;

	asm volatile(
		"int $3\n\t"
		"lcall *(%p[entry])\n\t cld\n\t"
		"jc 1f\n\t"
		"xor %%ah, %%ah\n"
		"1:"
		: "=a" (ret)
		: "0" (0xb10f),
		  "b" ((dev.bus<<8) | (dev.slot<<3) | (dev.func)),
		  "c" ((PCI_IRQ<<8)|(intline+10)),
		  [entry] "i" (&bios32_indirect)
		  :"edx", "esi","edi");
	EXIT_ATOMIC();

	DbgOut<UartBlocking> sender;
	sender.str("PCI ").hex(pci32_entry).end();
	sender.str("PCI ").hex(ret).end();


	if(ret & 0xff00)
		return false;
	uint32_t cmd = dev.get_reg(0x4);
	cmd&=~(1<<10); //irq
	dev.set_reg(0x4,cmd);
	return true;
}

bool Pci::Pci_no::operator==(Pci_no const& o) const
{
	if(o.bus != bus) return false;
	if(o.slot != slot) return false;
	if(o.func != func) return false;
	return true;
};

bool Pci::Pci_no::validate_vid()
{
	return (get_reg(0) & 0xffff) != 0xffff;
}

bool Pci::Pci_no::next()
{
	if (!Pci::get_handle().avalible)
		return false;
	if(!is_valid())
	{
		bus = 0;
		slot = 0;
		func = 0;
		return true;
	}
	else
	{
		Pci_no fn_0 = *this;
		fn_0.func = 0;
		uint8_t header_type = (fn_0.get_reg(0xC) >>16) &0xff;
		if (header_type & 0x80)
		{
			if(func<7)
			{
				Pci_no candidate_nf = *this;
				candidate_nf.func +=1;
				if(candidate_nf.validate_vid())
				{
					*this = candidate_nf;
					return true;
				}
			}
		}
		do
		{
			if (fn_0.slot < 32)
				fn_0.slot+=1;
			else
			{
				fn_0.slot = 0;
				fn_0.bus +=1;
				if(fn_0.bus == 256)
					return false;
			}
		}
		while (!fn_0.validate_vid());
		*this = fn_0;
		return true;
	}
}

uint32_t Pci::Pci_no::get_reg(uint8_t offset) const
{
	BezBios::Sched::MutexGuard lock = {Pci::get_handle().mutex};
	uint32_t address = (bus << 16) | (slot << 11) | (func << 8) | (offset & 0xfc) | (1<<31);
	outl(0xCF8,address);
	return inl(0xCFC);
}

void Pci::Pci_no::set_reg(uint8_t offset, uint32_t val) const
{
	BezBios::Sched::MutexGuard lock = {Pci::get_handle().mutex};
	uint32_t address = (bus << 16) | (slot << 11) | (func << 8) | (offset & 0xfc) | (1<<31);
	outl(0xCF8,address);
	outl(0xCFC,val);
}

bool Pci::Pci_no::is_valid() const
{
	if (bus>255) return false;
	if (slot>31) return false;
	if (func>7) return false;
	return true;
}

bool Pci::Pci_no::is_free() const
{
	Pci & handle = Pci::get_handle();
	if(!handle.drivers)
		return true;
	if(handle.drivers->address == *this)
		return false;
	for(Pci_driver* drv=handle.drivers->next;drv!=handle.drivers;drv=drv->next)
	{
		if(drv->address == *this)
			return false;
	}
	return true;
}


void Pci::Pci_driver::attach_driver()
{
	Pci & handle = Pci::get_handle();
	BezBios::Sched::MutexGuard lock = {handle.mutex};
	if(!handle.drivers)
	{
		handle.drivers = this;
	}
	else
	{
		plug(handle.drivers,false);
	}
}

bool Pci::Pci_driver::setup_irq(void (*_isr)(void *), void * _isr_arg, uint8_t intline)
{
	isr_arg = _isr_arg;
	MEM_BARRIER();
	isr=_isr;
	MEM_BARRIER();
	return Pci::get_handle().link_irq(address,intline);
}

