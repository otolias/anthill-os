SECTIONS {
    . = 0xffff000000080000;
    .text : AT(0x80000) { *(.text.boot) *(.text .text.*) }
    . = ALIGN(0x1000);
    .rodata : { *(.rodata .rodata.*) }
    .data : { *(.data .data.*) }
    .bss : { *(.bss .bss.*) }
    . = ALIGN(0x1000);
    _data_end = .;
}

_data_start = LOADADDR(.rodata);
_bss_start = LOADADDR(.bss);
_bss_end = _bss_start + SIZEOF(.bss);
_data_end = _bss_end + 0x1000 - 1 & ~(0x1000 - 1);
