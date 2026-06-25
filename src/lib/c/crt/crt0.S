.section ".text"

.extern __libc_deinit
.extern __libc_init
.extern exit
.extern main

.global _start
_start:
    bl      __libc_init
    bl      main
    bl      __libc_deinit
    bl      exit
