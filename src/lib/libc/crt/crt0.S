.section ".text"

.extern main
.extern exit

.global _start
_start:
    ldr     x0, [sp, #16]
    bl      main
    bl      exit
