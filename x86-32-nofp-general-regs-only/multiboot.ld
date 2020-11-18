OUTPUT_FORMAT("elf32-i386")
/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)
 
/* Tell where the various sections of the object files will be put in the final
   kernel image. */

MEMORY
{
    RAM (wx) : ORIGIN = 1M, LENGTH = 1M
}

SECTIONS
{
	. = ORIGIN(RAM);
	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	.text BLOCK(64K) : ALIGN(4K)
	{
	
		*(.multiboot)
	. = ALIGN(8);
 	KEEP(*(.multiboot))

  	PROVIDE (__init_array_start = .);
  	KEEP (*(SORT(.init_array.*)))
  	KEEP (*(.init_array ))
  	PROVIDE (__init_array_end = .);
  	
  	PROVIDE (__fini_array_start = .);
  	KEEP (*(SORT(.fini_array.*)))
  	KEEP (*(.fini_array ))
  	PROVIDE (__fini_array_end = .);

     *(.init)  /* Start here after reset.  */
    KEEP (*(.init))
		*(.text)
		*(.text*)
    *(.fini)  /* Infinite loop after program termination.  */
    KEEP (*(.fini))
	} > RAM
 
	/* Read-only data. */
	.rodata BLOCK(64K) : ALIGN(4K)
	{
		*(.rodata)
		*(.rodata*)
	} > RAM
 
	/* Read-write data (initialized) */
	.data BLOCK(64K) : ALIGN(4K)
	{
		*(.data)
		*(.data*)
	} > RAM
 
	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(64K) : ALIGN(4K)
	{
     PROVIDE (__bss_start = .) ;
		*(COMMON)
		*(.bss)
		*(.bss*)
	PROVIDE (__bss_end = .) ;
		
	} > RAM
	.stack BLOCK(64K) (NOLOAD) : ALIGN(4K)
	{
    PROVIDE (__stack_start = .) ;
		. = . + 4K;
	PROVIDE (__stack_end = .) ;
		
	} > RAM
}
