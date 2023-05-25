/*
 * pci.hpp
 *
 *  Created on: 20 gru 2022
 *      Author: nyuu
 */

#ifndef PLATFORM_PC_X86_32_PCI_HPP_
#define PLATFORM_PC_X86_32_PCI_HPP_

#include <stdint.h>
#include "memreg.hpp"
#include <sched/list.hpp>
#include <sched/bezbios_sched_api.h>
#include "interrupts.h"

class Pci {
public:
	class Pci_no {
	protected:
		friend class Pci;
		uint16_t bus;
		uint8_t slot;
		uint8_t func;
		bool validate_vid();
	public:
		Pci_no() : bus(0xffff),slot(0xff),func(0xff){};
		Pci_no(uint8_t _bus, uint8_t _slot, uint8_t _func)
		: bus(_bus),slot(_slot),func(_func){};

		bool operator==(Pci_no const& o) const;
		bool next();
		uint32_t get_reg(uint8_t offset) const;
		void set_reg(uint8_t offset, uint32_t val) const;
		bool is_valid() const;
		bool is_free() const;

	};
	class Pci_driver : public List<Pci_driver>
	{
	public:
		Pci_driver(): List(true){};
		Pci_no address;
	protected:
		friend class Pci;
		void (*isr)(void *);
		void * isr_arg;
	public:
		void attach_driver();
		bool setup_irq(void (*_isr)(void *), void * _isr_arg, uint8_t intline);
	};
private:
	Pci();
	unsigned long pci32_entry;
	bool pci_support();
	static void isr_c(Isr_stack* stack);
	void isr();
protected:
	bool link_irq(const Pci_no & dev,uint8_t intline);
	friend class Pci_driver;
	friend class Pci_no;
	Pci_driver * drivers;
	BezBios::Sched::Mutex mutex;
	bool avalible;
public:
	static Pci & get_handle();
    Pci(Pci const&) = delete;
    Pci& operator=(Pci const&) = delete;
	Memory_pool mmio_pool;
};




#endif /* PLATFORM_PC_X86_32_PCI_HPP_ */
