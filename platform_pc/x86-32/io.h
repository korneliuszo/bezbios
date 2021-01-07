/*
 * io.h
 *
 *  Created on: Dec 9, 2020
 *      Author: Korneliusz Osmenda
 */

#ifndef PLATFORM_PC_X86_32_IO_H_
#define PLATFORM_PC_X86_32_IO_H_

static inline unsigned char inb(unsigned short port)
{
	unsigned char ret;
	asm volatile ("inb %1, %0" : "=a"(ret): "Nd"(port));
	return ret;
}

static inline void outb(unsigned short port,unsigned char val)
{
	asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline unsigned short ins(unsigned short port)
{
	unsigned short ret;
	asm volatile ("in %1, %0" : "=a"(ret): "Nd"(port));
	return ret;
}

static inline void outs(unsigned short port,unsigned short val)
{
	asm volatile ("out %0, %1" : : "a"(val), "Nd"(port));
}

static inline void io_wait(unsigned char point)
{
    asm volatile ( "outb %0, $0x80" : : "a"(point) );
}

template
<unsigned short PORT>
class ByteIO {
public:
	operator unsigned char() const
	{
		return inb(PORT);
	}
	unsigned char operator =(const unsigned char& byte)
	{
		outb(PORT,byte);
		return byte;
	}
	unsigned char operator |=(const unsigned char& byte)
	{
		unsigned char rval=byte | inb(PORT);
		outb(PORT,rval);
		return rval;
	}
	unsigned char operator &=(const unsigned char& byte)
	{
		unsigned char rval=byte & inb(PORT);
		outb(PORT,rval);
		return rval;
	}
};

static inline unsigned long cli()
{
	unsigned long ret;
    asm volatile("# __raw_save_flags\n\t"
             "pushf ; pop %0"
             : "=rm" (ret)
             : /* no input */
             : "memory");
	asm volatile("cli");
	return ret;
}
static inline void sti(unsigned long prev)
{
    asm volatile("# __raw_restore_flags\n\t"
             "push %0 ; popf"
             : /* no output */
             : "rm" (prev)
             : "memory");
}


#define ENTER_ATOMIC() \
	unsigned long _is_interrupt = cli()
#define EXIT_ATOMIC() \
	sti(_is_interrupt)


#endif /* PLATFORM_PC_X86_32_IO_H_ */
