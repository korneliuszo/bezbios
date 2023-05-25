/*
 * yenta_socket.cpp
 *
 *  Created on: 2 sty 2023
 *      Author: nyuu
 */

#include "yenta_socket.hpp"
#include <sched/cwrap.hpp>
#include <uart/tlay2_dbgout.hpp>

struct __attribute__((packed)) yenta_regs {
	uint32_t event;
	uint32_t mask;
	uint32_t present;
	uint32_t force;
	uint32_t control;
};

YentaSocket::YentaSocket() : drv(), base_addr()
{
	DbgOut<UartBlocking> sender;
	sender.str("Yenta constr").end();

	for(Pci::Pci_no candidate;candidate.next();)
	{

		if(!candidate.is_free())
			continue;
		uint32_t classc = candidate.get_reg(0x8);
		if((classc&0xffffff00)!=0x06070000) //cardbus bridge
			continue;

		Pci & pci = Pci::get_handle();

		sender.str("Yenta").hex((uint32_t)pci.mmio_pool.addr).end();
		sender.str("Yenta").hex(pci.mmio_pool.len).end();
		sender.str("Yenta").hex((uint32_t)pci.mmio_pool.first).end();

		if(!pci.mmio_pool.allocate(base_addr,4*1024,12))
			return;


		drv.address = candidate;
		drv.attach_driver();

		uint32_t intline = (drv.address.get_reg(0x3c)>>8)&0xff;
		bool ret = drv.setup_irq(Cwrap<YentaSocket,&YentaSocket::isr>,this, intline-1);
		sender.str("Yenta test").hex(intline).hex(ret).end();

		uint32_t cmd = drv.address.get_reg(0x4);
		cmd|=(1<<1); //memory IO
		drv.address.set_reg(0x4,cmd);

		drv.address.set_reg(0x10,(uint32_t)base_addr.get_addr());

		volatile yenta_regs *regs = (volatile yenta_regs *)base_addr.get_addr();
		regs->mask= 0xf;
		sender.str("Yenta mask").hex(regs->mask).end();
		sender.str("Yenta event").hex(regs->event).end();

		sender.str("Yenta Alloc").end();
		return;
	}
	return;
}

void YentaSocket::isr()
{
	volatile yenta_regs *regs =(volatile yenta_regs *) base_addr.get_addr();
	uint32_t event = regs->event;
	DbgOut<UartBlocking> sender;
	sender.str("Yenta ISR").hex(event).end();
	regs->event = 0xf;
}
