SECTIONS {
    . = 0x80000;
    .text.boot : { *(.text.boot) }
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    . = ALIGN(0x8);
    bss_start = .;
    .bss : { *(.bss*) }
    bss_end = .;
}
kernel_end = .;

bss_size = (bss_end - bss_start)>>3;
