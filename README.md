Anthill OS
----------

A distributed operating system for the aarch64 architecture.

## Prerequisites

* A [GCC Cross compiler](https://wiki.osdev.org/GCC_Cross-Compiler) targeted for aarch64-elf.
* [Meson](https://github.com/mesonbuild/meson)
* [Ninja](https://github.com/ninja-build/ninja)
* [QEMU](https://github.com/qemu/qemu) (currently works only up to version 8.2.1)
* GDB for debugging (optional)

## Build Instructions

```
meson setup --cross-file aarch64-elf-txt build
meson install -C build
```

This will build the project and create the sysroot under build/sysroot

## Running

To run with QEMU:
```
./run
```

To run with QEMU and connect GDB:
```
./run -d
```

This will start and halt qemu, connect gdb and load the symbols. To continue, type
```c```.
