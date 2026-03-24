SECTIONS {
    . = 0x80000;
    .text : { *(.text.boot) *(.text .text.*) }
    . = ALIGN(0x1000);
    _data_start = .;
    .rodata : { *(.rodata .rodata.*) }
    .data : { *(.data .data.*) }
    _bss_start = .;
    .bss : { *(.bss .bss.*) }
    _bss_end = .;
    . = ALIGN(0x1000);
    _data_end = .;
}

_bss_size = (_bss_end - _bss_start)>>3;
