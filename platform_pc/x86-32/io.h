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

static inline unsigned long inl(unsigned short port)
{
	unsigned long ret;
	asm volatile ("inl %1, %0" : "=a"(ret): "Nd"(port));
	return ret;
}

static inline void outl(unsigned short port,unsigned long val)
{
	asm volatile ("outl %0, %1" : : "a"(val), "Nd"(port));
}

static inline void io_wait(unsigned char point)
{
    asm volatile ( "outb %0, $0x81" : : "a"(point) );
}

static inline unsigned long cli()
{
	unsigned long ret;
    asm volatile("# __raw_save_flags\n\t"
             "pushf ; pop %0 \n\t"
    		 "cli"
             : "=rm" (ret)
             : /* no input */
             : "memory");
	return ret;
}

static inline unsigned long getflags()
{
	unsigned long ret;
    asm volatile("# __raw_save_flags\n\t"
             "pushf ; pop %0"
             : "=rm" (ret)
             : /* no input */
             : "cc", "memory");
	return ret;
}


static inline void sti(unsigned long prev)
{
    asm volatile("# __raw_restore_flags\n\t"
             "push %0 ; popf"
             : /* no output */
             : "rm" (prev)
             : "memory","cc");
}

#ifdef __cplusplus

template
<unsigned short PORT_OUT, unsigned short PORT_IN = PORT_OUT>
class ByteIO {
public:
	operator unsigned char() const
	{
		return inb(PORT_IN);
	}
	unsigned char operator =(const unsigned char& byte)
	{
		outb(PORT_OUT,byte);
		return byte;
	}
	unsigned char operator |=(const unsigned char& byte)
	{
		unsigned long _is_interrupt = cli();
		unsigned char rval=byte | inb(PORT_IN);
		outb(PORT_OUT,rval);
		sti(_is_interrupt);
		return rval;
	}
	unsigned char operator &=(const unsigned char& byte)
	{
		unsigned long _is_interrupt = cli();
		unsigned char rval=byte & inb(PORT_IN);
		outb(PORT_OUT,rval);
		sti(_is_interrupt);
		return rval;
	}
};

#endif

#define ENTER_ATOMIC() \
	unsigned long _is_interrupt = cli()
#define EXIT_ATOMIC() \
	sti(_is_interrupt)

#define MEM_BARRIER() \
	asm volatile ("" : : : "memory")

#ifdef __cplusplus

class HWLOCK {
	unsigned long _is_interrupt;
public:
	HWLOCK() {_is_interrupt = cli();}
	~HWLOCK() { sti(_is_interrupt);}
};

#endif

#endif /* PLATFORM_PC_X86_32_IO_H_ */
