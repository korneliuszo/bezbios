/*
 * multiboot_header.cpp
 *
 *  Created on: Aug 24, 2021
 *      Author: Korneliusz Osmenda
 */

extern "C" {

typedef struct {
	unsigned long magic;
	unsigned long flags;
	unsigned long header_addr;
	unsigned long load_addr;
	unsigned long load_end_addr;
	unsigned long bss_end_addr;
	unsigned long entry_addr;
	unsigned long mode_type;
	unsigned long width;
	unsigned long height;
	unsigned long depth;


}multiboot_t;

extern multiboot_t Multiboot_header;

}


__attribute__((section(".multiboot")))
__attribute__((used))
multiboot_t Multiboot_header = {
		0x1BADB002,
		1,
		0x100000000 - 0x1BADB002 - 1,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
};
