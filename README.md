Anthill OS
----------

A distributed operating system for the aarch64 architecture.

## Prerequisites

* A cross compiler. Either:
    - gcc [cross compiler for aarch64-elf](https://wiki.osdev.org/GCC_Cross-Compiler).
    - clang
* [Meson](https://github.com/mesonbuild/meson)
* [Ninja](https://github.com/ninja-build/ninja)
* [QEMU](https://github.com/qemu/qemu) (currently works only up to version 8.2.1)
* GDB for debugging (optional)

## Building

Using gcc:

```
meson setup --cross-file cross/aarch64-elf.ini --cross-file cross/gcc.ini build
meson install -C build
```
Using clang:
```
meson setup --cross-file cross/aarch64-elf.ini --cross-file cross/clang.ini build
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
