/*
 * uart.c
 *
 *  Created on: Nov 13, 2020
 *      Author: Korneliusz Osmenda
 */


#include <io.h>
#include <interrupts.h>
#include <uart/bezbios_uart_api.h>
#include <sched/bezbios_sched_api.h>
#include <uart/fifo.h>

static BEZBIOS_INIT_SERIAL serial_port;

constexpr unsigned short PORT = 0x3F8;
constexpr unsigned char INT = 4;

ByteIO<PORT+0> THR;
ByteIO<PORT+0> RBR;
ByteIO<PORT+0> DLL;
ByteIO<PORT+1> IER;
ByteIO<PORT+1> DLH;
ByteIO<PORT+2> IIR;
ByteIO<PORT+2> FCR;
ByteIO<PORT+3> LCR;
ByteIO<PORT+4> MCR;
ByteIO<PORT+5> LSR;
ByteIO<PORT+6> MSR;
ByteIO<PORT+7> SR;

constexpr long FIFO_SIZE = 32;

DEFINE_STATIC_FIFO(serial_rx,FIFO_SIZE);
DEFINE_STATIC_FIFO(serial_tx,FIFO_SIZE);

BezBios::Sched::ConditionVariable rx_cv;
BezBios::Sched::ConditionVariable tx_cv;

template<>
__attribute__((interrupt))
void bezbios_imp_hw_req<INT>::f(struct interrupt_frame *)
{
	unsigned char IIR_cached = IIR;
	if((IIR_cached & 0x1))
	{
		bezbios_int_ack(INT);
		return; // not our irq (spurious?/another port?)
	}
	do
	{
	switch(IIR_cached&0x0E)
	{
	case 0x02: //transmit empty
		  while (LSR & 0x20)
		  {
			  unsigned char c;
			  if(fifo_get(&serial_tx,&c))
			  {
				  THR = c;
			  }
			  else
			  {
				  IER &= ~0x02;
				  break;
			  }
		  }
		  if(tx_cv.notify_all())
			  bezbios_sched_free_cpu(1);
		  break;
	case 0x04: //recv threshold
	case 0x0C: //timeout
		  while (LSR & 0x01)
		  {
			  if(fifo_check(&serial_rx) == FIFO_SIZE-1)
				  if(rx_cv.notify_all())
					  bezbios_sched_free_cpu(1);
			  fifo_put(&serial_rx,RBR);
		  }
		  if(rx_cv.notify_all())
			  bezbios_sched_free_cpu(1);
		  break;
	}
	IIR_cached = IIR;
	} while(!(IIR_cached & 0x1));
	bezbios_int_ack(INT);
}

void bezbios_serial_init() {
	IER = 0x00; // Disable all interrupts
	LCR = 0x80;    // Enable DLAB (set baud rate divisor)
	DLL = 0x03;    // Set divisor to 3 (lo byte) 38400 baud
	DLH = 0x00;    //                  (hi byte)
	LCR = 0x03;    // 8 bits, no parity, one stop bit
	FCR = 0xC7;    // Enable FIFO, clear them, with 14-byte threshold
	MCR = 0x08;    // route interrupt
	IER = 0x01;    // receive IRQ enabled
	bezbios_enable_irq(INT);
}


void bezbios_serial_send(unsigned char byte) {
	while(true)
	{
		ENTER_ATOMIC();
		bool brk = fifo_put(&serial_tx,byte);
		IER |= 0x02;
		if (brk)
		{
			EXIT_ATOMIC();
			break;
		}
		EXIT_ATOMIC();
		tx_cv.wait();
	}
}
unsigned char bezbios_serial_recv() {
	unsigned char c;
	while(true)
	{
		ENTER_ATOMIC();
		if(fifo_get(&serial_rx,&c))
		{
			EXIT_ATOMIC();
			break;
		}
		EXIT_ATOMIC();
		rx_cv.wait();
	}
	return c;
}
