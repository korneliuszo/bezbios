OUTPUT_FORMAT("elf32-i386")
/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)
 
/* Tell where the various sections of the object files will be put in the final
   kernel image. */

MEMORY
{
    LO_RAM (wx) : ORIGIN = 40k, LENGTH = 600k
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
	.bss BLOCK(64K) (NOLOAD) : ALIGN(4K)
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

	. = ORIGIN(LO_RAM);

	.loram_scratch BLOCK(64K) (NOLOAD) : ALIGN(64K)
	{
		*(.loram_scratch)
	} > LO_RAM AT> RAM

	.loram_scratch2 BLOCK(64K) (NOLOAD) : ALIGN(64K)
	{
		*(.loram_scratch2)
	} > LO_RAM AT> RAM
	
	.loram_code BLOCK(64K) : ALIGN(64K)
	{
		. = ALIGN(4);
		__sloram_copy = .;
		*(.loram_code)
		. = ALIGN(4);
		__eloram_copy = .;
	} > LO_RAM AT> RAM
	__loram_copy = LOADADDR(.loram_code);


	.loram_stack BLOCK(64K) (NOLOAD): ALIGN(64K)
	{
		*(.loram_stack)
	} > LO_RAM AT> RAM

	.debug_gdb_scripts 0 (INFO) :
	{
		KEEP(*(.debug_gdb_scripts))
	}

	.eh_frame 0 (INFO) :
	{
		KEEP(*(.eh_frame))
	}
}
