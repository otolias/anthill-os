.section ".text"

.extern main

.global _start
_start:
    bl      main
