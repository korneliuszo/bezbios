OUTPUT_FORMAT("elf32-i386")
/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)
 
/* Tell where the various sections of the object files will be put in the final
   kernel image. */

MEMORY
{
    LO_RAM (wx) : ORIGIN = 0x00007E00, LENGTH = 400k
}

SECTIONS
{
	. = ORIGIN(LO_RAM);
	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	.text : ALIGN(8)
	{
     *(.lo_init)  /* Start here after reset.  */

	. = ALIGN(8);

    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
    KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
    PROVIDE_HIDDEN (__init_array_end = .);

    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
    KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
    PROVIDE_HIDDEN (__fini_array_end = .);

     *(.init)  /* Start here after reset.  */
    KEEP (*(.init))
		*(.text)
		*(.text*)
    *(.fini)  /* Infinite loop after program termination.  */
    KEEP (*(.fini))
    *(.note.gnu.build-id)
	} > LO_RAM
 
	/* Read-only data. */
	.rodata : ALIGN(4K)
	{
		*(.rodata)
		*(.rodata*)
	} > LO_RAM
 
	/* Read-write data (initialized) */
	.data : ALIGN(4K)
	{
		*(.data)
		*(.data*)
	} > LO_RAM
 
 	.loram_code : ALIGN(16)
	{
		. = ALIGN(4);
		__sloram_copy = .;
		*(.loram_code)
		. = ALIGN(4);
		__eloram_copy = .;
	} > LO_RAM
	__loram_copy = LOADADDR(.loram_code);
 
	/* Read-write data (uninitialized) and stack */
	.bss (NOLOAD) : ALIGN(4K)
	{
     PROVIDE (__bss_start = .) ;
		*(COMMON)
		*(.bss)
		*(.bss*)
	PROVIDE (__bss_end = .) ;
		
	} > LO_RAM
	.stack (NOLOAD) : ALIGN(4K)
	{
    PROVIDE (__stack_start = .) ;
		. = . + 4K;
	PROVIDE (__stack_end = .) ;
		
	} > LO_RAM

	.loram_stack (NOLOAD): ALIGN(4K)
	{
		*(.loram_stack)
	} > LO_RAM

	.loram_scratch BLOCK(64K) (NOLOAD) : ALIGN(64K)
	{
		*(.loram_scratch)
	} > LO_RAM
	
	.loram_scratch2 BLOCK(64K) (NOLOAD) : ALIGN(64K)
	{
		*(.loram_scratch2)
	} > LO_RAM

	.debug_gdb_scripts 0 (INFO) :
	{
		*(.debug_gdb_scripts)
	}
}
