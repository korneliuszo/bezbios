/*
 * uart.c
 *
 *  Created on: Nov 13, 2020
 *      Author: Korneliusz Osmenda
 */

static __inline unsigned char
inb (unsigned short int __port)
{
  unsigned char _v;

  __asm__ __volatile__ ("inb %w1,%0":"=a" (_v):"Nd" (__port));
  return _v;
}

static __inline void
outb (unsigned char __value, unsigned short int __port)
{
  __asm__ __volatile__ ("outb %b0,%w1": :"a" (__value), "Nd" (__port));
}

#define PORT 0x3f8   ///* COM1 */

static
__attribute__((used))
__attribute__((constructor))
void init_serial() {
   outb(0x00, PORT + 1);    // Disable all interrupts
   outb(0x80, PORT + 3);    // Enable DLAB (set baud rate divisor)
   outb(0x03, PORT + 0);    // Set divisor to 3 (lo byte) 38400 baud
   outb(0x00, PORT + 1);    //                  (hi byte)
   outb(0x03, PORT + 3);    // 8 bits, no parity, one stop bit
   outb(0xC7, PORT + 2);    // Enable FIFO, clear them, with 14-byte threshold
   //outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

static
int serial_received() {
   return inb(PORT + 5) & 1;
}

char bezbios_low_read_serial() {
   while (serial_received() == 0);

   return inb(PORT);
}

static
int is_transmit_empty() {
   return inb(PORT + 5) & 0x20;
}

void bezbios_low_write_serial(char a) {
   while (is_transmit_empty() == 0);

   outb(a,PORT);
}
