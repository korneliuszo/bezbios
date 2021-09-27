/*
 * vm86.cpp
 *
 *  Created on: Aug 17, 2021
 *      Author: Korneliusz Osmenda
 */

#include "tss.h"
#include "vm86.hpp"
#include "sched/bezbios_sched_api.h"
#include "io.h"
#include <uart/tlay2_dbgout.hpp>


__attribute__((used))
__attribute__((section(".loram_code")))
static unsigned char vm86_isr[] = { 0xcd, 0x48, 0xf4};	/* int 48h; hlt	*/
//static unsigned char vm86_isr[] = { 0xEB, 0xFE, 0xF4 };	/* busyloop, hlt	*/
//static unsigned char vm86_isr[] = { 0xF4 };	/*hlt	*/

static BezBios::Sched::Mutex vmm86_mutex;

__attribute__((section(".loram_stack")))
static unsigned char vm86_stack[4096];


LONGADDR vmm86_to_segment(void * ptr, bool codesection)
{
	LONGADDR ret;
	if(codesection)
	{
		ret.segment = ((unsigned long) ptr >> 4)&0xFFFF;
		ret.offset = (unsigned long)ptr & 0x000F;
	}
	else
	{
		ret.segment = ((unsigned long) ptr >> 4)&0xF000;
		ret.offset = (unsigned long)ptr & 0xFFFF;
	}
	return ret;
}

struct VM86RUNPARAMS{
	LONGADDR code;
	LONGADDR stack;
	unsigned long eflags;
};

class VM86RegPtr {
	unsigned long *segment;
	unsigned long *offset;
public:
	VM86RegPtr(unsigned long *_segment, unsigned long *_offset)
		: segment(_segment), offset(_offset) {};
	template<typename SIZE>
	SIZE popC(){
		unsigned long addr = ((unsigned long)(*segment)<<4) | (unsigned long)*offset;
		SIZE * ptr = (SIZE *) addr;
		*offset+=sizeof(SIZE);
		return *ptr;
	}
	void setC(unsigned long _segment, unsigned long _offset)
	{
		*segment = _segment;
		*offset = _offset;
	}
	template<typename SIZE>
	SIZE popS(){
		unsigned long addr = ((unsigned long)(*segment)<<4) | (unsigned long)*offset;
		SIZE * ptr = (SIZE *) addr;
		*offset+=sizeof(SIZE);
		return *ptr;
	}
	template<typename SIZE>
	void pushS(SIZE val){
		*offset-=sizeof(SIZE);
		unsigned long addr = ((unsigned long)(*segment)<<4) | (unsigned long)*offset;
		SIZE * ptr = (SIZE *) addr;
		*ptr = val;
		return;
	}
};

template<typename LOSTACK>
struct __attribute__((packed)) VM86_stack : LOSTACK{
	unsigned long es;
	unsigned long ds;
	unsigned long fs;
	unsigned long gs;
};

static_assert(sizeof(VM86_stack<User_stack<Error_stack>>) == 84, "Verifying size failed!");


static Vmm86Regs * vmm86_return;

static constexpr unsigned long PUSH_MASK = ~(1<<17 | 1<<16); //VM RF
static constexpr unsigned long POP_MASK = ~(1<<17 | 1<<16 | 3<<12 | 1<<9); //VM RF IOPL IF
static constexpr unsigned long POP_SET = (1<<17 | 0<<12); //VM IOPL = 0
static unsigned long vm86_ie;

static unsigned short vm86_init_stackptr;

bool vm86_handle_gpf(Error_stack *frame)
{
	VM86_stack<User_stack<Error_stack>> *stack = (VM86_stack<User_stack<Error_stack>>*)frame;
	VM86RegPtr ip(&stack->cs,&stack->eip);
	VM86RegPtr ss(&stack->ss,&stack->esp);
    unsigned char i_byte = ip.popC<unsigned char>();
    if (i_byte == 0x67) //ADDRESS_SIZE_PREFIX
    {
    	i_byte = ip.popC<unsigned char>();
    }
    switch(i_byte)
    {
    case 0xf4: //hlt
    {
    	if(vm86_init_stackptr != stack->esp)
    	{
    		DbgOut<UartBlocking> sender;
    		sender.str("OOPS!! VM86 USES HALT").end();
    	}
    	vmm86_return->ax = frame->eax;
    	vmm86_return->bx = frame->ebx;
    	vmm86_return->cx = frame->ecx;
    	vmm86_return->dx = frame->edx;
    	vmm86_return->si = frame->esi;
    	vmm86_return->di = frame->edi;
    	vmm86_return->bp = frame->ebp;
    	vmm86_return->eflags = frame->eflags;
    	asm("jmp vmx86_ret"::: "memory");
    	break;
    }
    case 0x66: //OPERAND_SIZE_PREFIX
    {
    	i_byte = ip.popC<unsigned char>();
    	switch(i_byte)
    	{
    	case 0x9C: // PUSHF
    	{
    		ss.pushS<unsigned long>(stack->eflags & PUSH_MASK);
    		return true;
    	}
    	case 0x9D: //POPF
    	{
    		unsigned long eflags = ss.popS<unsigned long>();
    		stack->eflags = (eflags & POP_MASK) | POP_SET | vm86_ie;
    		return true;
    	}
    	case 0xEF: /* outl (%dx), ax */
    	{
    		outl(stack->edx, stack->eax);
    		return true;
    	}
    	case 0xE7: /* outl port, al */
    	{
    		outl(ip.popC<unsigned char>(), stack->eax);
    		return true;
    	}
       	case 0xE5: /* inl al, port */
        	{
        	unsigned long val = inl(ip.popC<unsigned char>());
        	stack->eax = val;
        	return true;
        }
       	case 0xED: /* inl al, (%dx) */
        	{
        	unsigned long val = inl(stack->edx);
        	stack->eax = val;
        	return true;
        }
    	default:
    		break;
    	}
    	break;
    }
    case 0xFA: // CLI
    	return true;
    case 0xFB: //STI
    	return true;
    case 0x9C: //PUSHF
    {
		ss.pushS<unsigned short>(stack->eflags & PUSH_MASK);
		return true;
    }
	case 0x9D: //POPF
	{
		unsigned long eflags = ss.popS<unsigned short>();
		stack->eflags = (eflags & POP_MASK) | POP_SET | vm86_ie;
		return true;
	}
	case 0xCD: //INTn
	{
		i_byte = ip.popC<unsigned char>();
		ss.pushS<unsigned short>(stack->eflags & PUSH_MASK);
		ss.pushS<unsigned short>(stack->cs);
		ss.pushS<unsigned short>(stack->eip);
		unsigned short * ivt_vect =0;
		stack->eip = ivt_vect[i_byte*2];
		stack->cs = ivt_vect[i_byte*2+1];
		return true;
	}
	case 0xCF: //IRET
	{
		stack->eip = ss.popS<unsigned short>();
		stack->cs =  ss.popS<unsigned short>();
		unsigned long eflags = ss.popS<unsigned short>();
		stack->eflags = (eflags & POP_MASK) | POP_SET | vm86_ie;
		return true;
	}
	case 0xEE: /* outb (%dx), al */
	{
		outb(stack->edx, stack->eax);
		return true;
	}
	case 0xEF: /* outw (%dx), ax */
	{
		outs(stack->edx, stack->eax);
		return true;
	}
	case 0xE6: /* outb port, al */
	{
		outb(ip.popC<unsigned char>(), stack->eax);
		return true;
	}
	case 0xE7: /* outw port, al */
	{
		outs(ip.popC<unsigned char>(), stack->eax);
		return true;
	}
	case 0xE4: /* inb al, port */
	{
		unsigned char val = inb(ip.popC<unsigned char>());
		stack->eax = (stack->eax &  ~0xff) | val;
		return true;
	}
   	case 0xE5: /* inw al, port */
    	{
    	unsigned short val = ins(ip.popC<unsigned char>());
    	stack->eax = (stack->eax &  ~0xffff) | val;
    	return true;
    }
	case 0xEC: /* inb al, (%dx) */
	{
		unsigned char val = inb(stack->edx);
		stack->eax = (stack->eax &  ~0xff) | val;
		return true;
	}
   	case 0xED: /* inw al, (%dx) */
    {
    	unsigned short val = ins(stack->edx);
    	stack->eax = (stack->eax &  ~0xffff) | val;
    	return true;
    }
    default:
    	break;
    }

	return false;
}

static unsigned long vmm86_sp;
static Vmm86SegmentRegisters vmm86_seg;
static Vmm86Regs vmm86_in;
static VM86RUNPARAMS vmm86_run;

__attribute__((noinline))
static void callx86_nomut(const Vmm86Regs * in, Vmm86Regs * out, Vmm86SegmentRegisters *seg, VM86RUNPARAMS * run)
{
	vmm86_return = out;
	vmm86_seg = *seg;
	vmm86_in = *in;
	vmm86_run = *run;

	vm86_init_stackptr = run->stack.offset;

    asm(
    		"pushf\n\t"
    		"cli\n\t"
    		"movl %%esp, (%P[vmm86_sp])\n\t"

    		"movl %%esp, (%P[tsp])\n\t"
    		"movw %%ss, (%P[tss])\n\t"

    		"mov $0x18, %%eax\n\t"
    		"ltr %%ax\n\t"

    		"pushfl\n\t" // NT flag should be disabled
    		".cfi_adjust_cfa_offset 4\n\t"
    		"btr %%esp, 14\n\t"
    		"popfl\n\t"
    		".cfi_adjust_cfa_offset -4\n\t"

    		"movl (%P[sgs]), %%eax\n\t"
    		"push %%eax\n\t"
    		".cfi_adjust_cfa_offset 4\n\t"
    		"movl (%P[sfs]), %%eax\n\t"
    		"push %%eax\n\t"
    		".cfi_adjust_cfa_offset 4\n\t"
    		"movl (%P[sds]), %%eax\n\t"
    		"push %%eax\n\t"
    		".cfi_adjust_cfa_offset 4\n\t"
    		"movl (%P[ses]), %%eax\n\t"
    		"push %%eax\n\t"
    		".cfi_adjust_cfa_offset 4\n\t"
    		"movl (%P[rss]), %%eax\n\t"
    		"push %%eax\n\t"
    		".cfi_adjust_cfa_offset 4\n\t"
    		"movl (%P[rsp]), %%eax\n\t"
    		"push %%eax\n\t"
    		".cfi_adjust_cfa_offset 4\n\t"
    		"movl (%P[ref]), %%eax\n\t"
    		"push %%eax\n\t"
    		".cfi_adjust_cfa_offset 4\n\t"
    		"movl (%P[rcs]), %%eax\n\t"
    		"push %%eax\n\t"
    		".cfi_adjust_cfa_offset 4\n\t"
    		"movl (%P[rip]), %%eax\n\t"
    		"push %%eax\n\t"
    		".cfi_adjust_cfa_offset 4\n\t"

    		"movl (%P[iax]), %%eax\n\t"
    		"movl (%P[ibx]), %%ebx\n\t"
    		"movl (%P[icx]), %%ecx\n\t"
    		"movl (%P[idx]), %%edx\n\t"
    		"movl (%P[isi]), %%esi\n\t"
    		"movl (%P[idi]), %%edi\n\t"
    		"movl (%P[ibp]), %%ebp\n\t"

    		"iretl\n\t"
    		".cfi_adjust_cfa_offset -36\n\t"

			"vmx86_ret:\n\t"
    		"movl (%P[vmm86_sp]), %%esp\n\t"
    		"popfl\n\t"
		: :
		[iax] "i" (&vmm86_in.ax),
		[ibx] "i" (&vmm86_in.bx),
		[icx] "i" (&vmm86_in.cx),
		[idx] "i" (&vmm86_in.dx),
		[isi] "i" (&vmm86_in.si),
		[idi] "i" (&vmm86_in.di),
		[ibp] "i" (&vmm86_in.bp),
		[sds] "i" (&vmm86_seg.ds),
		[ses] "i" (&vmm86_seg.es),
		[sfs] "i" (&vmm86_seg.fs),
		[sgs] "i" (&vmm86_seg.gs),
//		[rcs] "i" (&vmm86_run.code.segment),
//		[rip] "i" (&vmm86_run.code.offset),
//		[rss] "i" (&vmm86_run.stack.segment),
//		[rsp] "i" (&vmm86_run.stack.offset),
		[rcs] "i" (&vmm86_run.code.segment),
		[rip] "i" (&vmm86_run.code.offset),
		[rss] "i" (&vmm86_run.stack.segment),
		[rsp] "i" (&vmm86_run.stack.offset),
		[ref] "i" (&vmm86_run.eflags),
		[tsp] "i" (&tss_io.t.esp0),
		[tss] "i" (&tss_io.t.ss0),
		[vmm86_sp] "i"(&vmm86_sp)
		:	"eax","ebx","ecx","edx","esi","edi","ebp","memory");
    tss_clear_busy();
}

void callx86int(unsigned char isr, const Vmm86Regs * in, Vmm86Regs * out, Vmm86SegmentRegisters *seg)
{
	vmm86_mutex.aquire();
	vm86_isr[1] = isr;

	VM86RUNPARAMS rparm;

	rparm.code = vmm86_to_segment(vm86_isr,true);
	rparm.stack = vmm86_to_segment(vm86_stack+sizeof(vm86_stack)-1);

	unsigned long eflags = getflags();

	rparm.eflags = 0x00020000 //VM86, IOPL=0
				| (eflags & (1<<9)); //copy interrupt flag
	vm86_ie = eflags & (1<<9);
	callx86_nomut(in,out,seg,&rparm);

	vmm86_mutex.release();
}

__attribute__((used))
__attribute__((section(".loram_code")))
static unsigned char vm86_lc[] = { 0x9a,0x00,0x00,0x00,0x00, 0xf4};	/* far call; hlt	*/

void callx86ptr(LONGADDR entry, const Vmm86Regs * in, Vmm86Regs * out, Vmm86SegmentRegisters *seg, unsigned short stack[8])
{
	vmm86_mutex.aquire();

	vm86_lc[1] = entry.offset;
	vm86_lc[2] = entry.offset>>8;
	vm86_lc[3] = entry.segment;
	vm86_lc[4] = entry.segment>>8;

	VM86RUNPARAMS rparm;

	rparm.code = vmm86_to_segment(vm86_lc,true);
	rparm.stack = vmm86_to_segment(vm86_stack+sizeof(vm86_stack)-1);

	VM86RegPtr ss(&rparm.stack.segment,&rparm.stack.offset);

	for(int i=7;i>=0;i--)
		ss.pushS<unsigned short>(stack[i]);

	unsigned long eflags = getflags();

	rparm.eflags = 0x00020000 //VM86, IOPL=0
				| (eflags & (1<<9)); //copy interrupt flag
	vm86_ie = eflags & (1<<9);
	callx86_nomut(in,out,seg,&rparm);

	vmm86_mutex.release();
}
