OUTPUT_FORMAT("elf32-i386")
/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)
 
/* Tell where the various sections of the object files will be put in the final
   kernel image. */

MEMORY
{
    LO_RAM (wx) : ORIGIN = 0x00007C00, LENGTH = 128k
}

SECTIONS
{
	. = ORIGIN(LO_RAM);
	.text : ALIGN(1K)
	{
		*(.text)
		*(.text*)
		PROVIDE(code_start = .);
		KEEP(*(.binary_section))
		PROVIDE(code_end = .);
		*(.note.gnu.build-id)
	} > LO_RAM
}
