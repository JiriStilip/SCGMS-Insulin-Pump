/**
 * BlueThunder Linker Script for the Raspberry Pi!
 *
 *
 *
 **/
MEMORY
{
    RESERVED    (r)    : ORIGIN = 0x00000000, LENGTH = 32K
    INIT_RAM    (rwx)  : ORIGIN = 0x00008000, LENGTH = 32K
    RAM         (rwx)  : ORIGIN = 0x00010000, LENGTH = 128M
}

ENTRY(_start)

SECTIONS {
    /*
	 * Our init section allows us to place the bootstrap code at address 0x8000
	 *
	 * This is where the Graphics processor forces the ARM to start execution.
	 * However, the interrupt vector code remains at 0x0000, and so we must copy the correct
	 * branch instructions to 0x0000 - 0x001C to get the processor to handle interrupts.
	 *
	 */
    .init : {
        KEEP(*(.init))
    } > INIT_RAM = 0

    .module_entries : {
        __module_entries_start = .;
        KEEP(*(.module_entries))
        KEEP(*(.module_entries.*))
        __module_entries_end = .;
        __module_entries_size = SIZEOF(.module_entries);
    } > INIT_RAM

    /**
     * This is the main code section, essentially of unlimited size (128Mb).
     *
     **/
    .text : {
        *(.text)
    } > RAM

    /*
    * Next, we put the data.
    */
    .data : {
        *(.data)
    } > RAM

    .bss :
    {
        __bss_start__ = .;
        *(.bss)
        *(.bss.*)
        __bss_end__ = .;
    } > RAM

    /* Constructors and Destructors */
   .ctors : {
        KEEP(*(.ctors))
        KEEP(*(.ctors.*))
    } > RAM

    .dtors : {
        KEEP(*(.dtors))
        KEEP(*(.dtors.*))
    } > RAM


    /* Initialization functions which run before main(), such as global constructors. */
    .init_array : ALIGN( 4 ) {

    /* preinit data */
    PROVIDE_HIDDEN ( __preinit_array_start = . );
    KEEP( *( .preinit_array ) )
    PROVIDE_HIDDEN( __preinit_array_end = . );

    . = ALIGN(4);
    /* init data */
    PROVIDE_HIDDEN ( __init_array_start = . );
    KEEP( *( SORT( .init_array.* ) ) )
    KEEP(*(.init_array))
    PROVIDE_HIDDEN ( __init_array_end = . );
    } > RAM

    .ARM.exidx : {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > RAM

    /* define a global symbol _etext just after the last code byte */
    _etext = .;
    end = .;

    /* Stack starts at the top of the RAM and moves down! */
    _estack = ORIGIN(RAM) + LENGTH(RAM);
}

