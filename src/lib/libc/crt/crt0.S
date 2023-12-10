.section ".text"

.extern main
.extern exit

.global _start
_start:
    bl      main
    bl      exit
