/*
 * pnp.cpp
 *
 *  Created on: Sep 8, 2021
 *      Author: Korneliusz Osmenda
 */

#include "pnp.hpp"

union pnp_bios_install_struct {
	struct {
		unsigned long signature;
		unsigned char version;
		unsigned char length;
		unsigned short control;
		unsigned char checksum;

		unsigned long eventflag;
		unsigned short rmoffset;
		unsigned short rmcseg;
		unsigned short pm16offset;
		unsigned long pm16cseg;
		unsigned long deviceID;
		unsigned short rmdseg;
		unsigned long pm16dseg;
	} __attribute__((packed)) fields;
	unsigned char chars[0x21];
};


unsigned short PnP::call(unsigned short func, unsigned short arg1,
		unsigned short arg2, unsigned short arg3, unsigned short arg4,
		unsigned short arg5, unsigned short arg6, unsigned short arg7) {

	if (!found)
		return 0x82;

	Vmm86Regs in = {};
	Vmm86Regs out = {};
	Vmm86SegmentRegisters seg = {};

	unsigned short stack[8];
	stack[0] = func;
	stack[1] = arg1;
	stack[2] = arg2;
	stack[3] = arg3;
	stack[4] = arg4;
	stack[5] = arg5;
	stack[6] = arg6;
	stack[7] = arg7;


	callx86ptr(rm,&in,&out,&seg,stack);

	return out.ax;
}

PnP::PnP()
: rm({0,0}),ds(0),found(false){
	union pnp_bios_install_struct *check;
	for (check = (union pnp_bios_install_struct*) 0xf0000;
			check < (union pnp_bios_install_struct*) 0xffff0;
			check = (union pnp_bios_install_struct*)(((unsigned long) check) + 16)) {
		if (check->fields.signature
				!= (('$' << 0) + ('P' << 8) + ('n' << 16) + ('P' << 24)))
			continue;
		int len, i;
		unsigned char sum;
		len = check->fields.length;
		if (!len)
			continue;
		for (sum = 0, i = 0; i < len; i++)
			sum += check->chars[i];
		if (sum)
			continue;
		if (check->fields.version < 0x10)
			continue;
		found = true;
		rm.offset = check->fields.rmoffset;
		rm.segment = check->fields.rmcseg;
		ds = check->fields.rmdseg;
		break;
	}
}

__attribute__((section(".loram_scratch2")))
static unsigned char vm86_page2[64*1024];

unsigned short PnP::get_node_info(unsigned short *no_nodes, unsigned short *max_size)
{
	LONGADDR scratch = vmm86_to_segment(vm86_page2,true);
	unsigned short ret = call(0x00,
			scratch.offset+0,
			scratch.segment,
			scratch.offset+2,
			scratch.segment,
			ds,0,0);
	*no_nodes = *(unsigned short *)&vm86_page2[0];
	*max_size = *(unsigned short *)&vm86_page2[2];
	return ret;
}

unsigned short PnP::get_system_node(unsigned char *no_node, unsigned char **buff)
{
	LONGADDR scratch = vmm86_to_segment(vm86_page2,true);
	vm86_page2[0] = *no_node;
	vm86_page2[1] = 0;
	unsigned short ret = call(0x01,
			scratch.offset+0,
			scratch.segment,
			scratch.offset+2,
			scratch.segment,
			1,ds,0);
	*buff = &vm86_page2[2];
	*no_node = vm86_page2[0];
	return ret;
}

PnP& PnP::get_singleton()
{
	static PnP pnp;
	return pnp;
}

