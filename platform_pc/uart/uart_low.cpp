/*
 * uart_low.cpp
 *
 *  Created on: Dec 30, 2020
 *      Author: Korneliusz Osmenda
 */

#include <io.h>
#include <uart/bezbios_uart_api_low.h>

#define PORT 0x3F8

void bezbios_serial_bootup_init() {
   outb(PORT + 1, 0x00);    // Disable all interrupts
   outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   outb(PORT + 1, 0x00);    //                  (hi byte)
   outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
   outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   //outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   bezbios_low_write_serial('\n');
}

void bezbios_low_write_serial(char a) {
   while ((inb(PORT + 5) & 0x20) == 0);

   outb(PORT,a);
}

void bezbios_low_wait()
{
	while ((inb(PORT + 5) & 0x20) == 0);
}
