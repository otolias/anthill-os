arm-os
------

## Prerequisites

* A [GCC Cross compiler](https://wiki.osdev.org/GCC_Cross-Compiler) targeted for aarch64-elf.
* [Ninja](https://github.com/ninja-build/ninja)
* [QEMU](https://github.com/qemu/qemu) 2.12 or newer for emulation (optional)
* GDB for debugging (optional)

## How to build

A build helper is supplied.

To build project:
```
./construct build -c aarch64-elf
```

To build and run with QEMU:
```
./construct run -c aarch64-elf
```

To build, run with QEMU and connect GDB:
```
./construct debug -c aarch64-elf
```
