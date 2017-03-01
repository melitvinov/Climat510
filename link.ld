/*      Linker script.
        Based on Miro Samek's 'Building Bare-Metal ARM Systems with GNU'
*/

OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)

MEMORY
{
    ROM (rx) :       ORIGIN = 0x08000000, LENGTH = (256k)
    RAM (rwx):       ORIGIN = 0x20000000, LENGTH = (64k)
}

MIN_STACK_SIZE = 2048;

SECTIONS
{
    .reset ORIGIN(ROM):
    {
        KEEP(*(.initvectors))
    } >ROM

    . = ALIGN(4);

    .text :
    {
        /*KEEP(*(.init))*/      /* Startup code from .init-section */
	*(.text .text.*)        /* remaining code */
	*(.gnu.linkonce.t.*)
	*(.glue_7)
	*(.glue_7t)
	*(.gcc_except_table)
	*(.rodata)              /* read-only data (constants) */
	*(.rodata*)
	*(.gnu.linkonce.r.*)
        . = ALIGN(4);
    } > ROM

    _etext = .;
    PROVIDE (etext = .);

    .data ORIGIN(RAM):
    {
        __data_load = LOADADDR (.data);
        __data = .;
        KEEP(*(.vectors))
        SORT(CONSTRUCTORS) /* mt 4/2005 */
        *(.data .data.* .gnu.linkonce.d.*)
        . = ALIGN (4);
        __data_end = .;
    } >RAM AT>ROM

    .bss (NOLOAD) :
    {
        __bss = .;
        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)
        . = ALIGN (4);
        __bss_end = .;
    } >RAM

    .stack ALIGN(8):
    {
        __stack_bottom = .;
        . = ORIGIN(RAM) + LENGTH(RAM);
        . = ALIGN (8);
        __stack_top = .;
        ASSERT( SIZEOF(.stack) >= MIN_STACK_SIZE, "stack size is too small !") ;
    } >RAM

    _end = .;
    __end = .;
    PROVIDE(end = .);
}