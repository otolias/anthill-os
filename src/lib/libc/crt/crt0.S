.section ".text"

.extern _malloc_init
.extern main
.extern exit

.global _start
_start:
    ldr     x0, [sp, #16]
    bl      _malloc_init
    bl      main
    bl      exit
