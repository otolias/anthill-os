arm-os
------

## Prerequisites

* A [GCC Cross compiler](https://wiki.osdev.org/GCC_Cross-Compiler) targeted for aarch64-elf.
* [Meson](https://github.com/mesonbuild/meson)
* [Ninja](https://github.com/ninja-build/ninja)
* [QEMU](https://github.com/qemu/qemu) 2.12 or newer
* GDB for debugging (optional)
* [Unity](https://github.com/ThrowTheSwitch/Unity) for unit tests (optional)

## Build Instructions

```
meson setup --cross-file aarch64-elf-txt build
meson install -C build
```

This will build the project and create the sysroot under build/sysroot

## Testing

To run tests, you must have Unity installed

```
meson test -C build
```

This will compile and run tests natively

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
```c```
