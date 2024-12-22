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

[[gnu::used]]
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

static BezBios::Sched::ConditionVariableSingle rx_cv;
static BezBios::Sched::ConditionVariableSingle tx_cv;

static void UART_IRQ(Isr_stack *)
{
	unsigned char IIR_cached = IIR;
	ThreadControlBlock * txcvthread = nullptr;
	ThreadControlBlock * rxcvthread = nullptr;
	if((IIR_cached & 0x1))
	{
		bezbios_int_ack(INT);
		return; // not our irq (spurious?/another port?)
	}
	do
	{
	bezbios_int_ack(INT);
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
		  {	
			  auto tmp = tx_cv.notify();
			  if(tmp)
		  	  txcvthread = tmp;
		  }
		  break;
	case 0x04: //recv threshold
	case 0x0C: //timeout
		while ((LSR & 0x01) && (fifo_check(&serial_rx) < FIFO_SIZE-1))
		{
		  if(fifo_check(&serial_rx) == FIFO_SIZE-2 )
		  {
			IER &= ~0x01;
		  }
		  //else
		  {
			  char tm = RBR;
			  fifo_put(&serial_rx,tm);
		  }
		}
		{	
			auto tmp = rx_cv.notify();
			if(tmp)
		  	  rxcvthread = tmp;
		}
		break;
	}
	IIR_cached = IIR;
	} while(!(IIR_cached & 0x1));
	if(txcvthread)
		bezbios_sched_sel_task(1,txcvthread);
	if(rxcvthread)
		bezbios_sched_sel_task(1,rxcvthread);

}

void bezbios_serial_init() {
	IER = 0x00;    // Disable all interrupts
	LCR = 0x80;    // Enable DLAB (set baud rate divisor)
	DLL = 0x03;    // Set divisor to 3 (lo byte) 38400 baud
	DLH = 0x00;    //                  (hi byte)
	LCR = 0x03;    // 8 bits, no parity, one stop bit
	FCR = 0x07;    // Enable FIFO, clear them, with 1-byte threshold
	MCR = 0x08;    // route interrupt
	IER = 0x01;    // receive IRQ enabled
	register_isr(INT,UART_IRQ);
	bezbios_enable_irq(INT);
}


void bezbios_serial_send(unsigned char byte) {
	ENTER_ATOMIC();
	while(true)
	{
		bool cont = fifo_check(&serial_tx);
		bool brk = fifo_put(&serial_tx,byte);		
		if(!cont)
		{
			unsigned char c=0;
			fifo_get(&serial_tx,&c);
			IER |= 0x02;
			THR = c;
		}
		if (brk)
		{
			break;
		}
		tx_cv.wait();
	}
	EXIT_ATOMIC();
}
unsigned char bezbios_serial_recv() {
	unsigned char c;
	ENTER_ATOMIC();
	while(true)
	{
		bool get = fifo_get(&serial_rx,&c);
		//bool chk = (fifo_check(&serial_rx) < FIFO_SIZE-2 );		
		if(!(IER&0x01))
		{
			IER |= 0x01;
		}
		if(get)
		{
			break;
		}
		rx_cv.wait();
	}
	EXIT_ATOMIC();
	return c;
}
