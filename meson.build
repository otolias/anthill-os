project('arm_os', 'c',
  default_options : [
    'buildtype=debugoptimized',
    'b_lundef=false',
    'warning_level=2',
    'prefix=' + meson.build_root() / 'sysroot',
    'libdir=usr/lib',
    'includedir=usr/include',
  ],
)

gcc = find_program('c')
objcopy = find_program('objcopy')
tar = find_program('tar')

kernel_sources = []
kernel_includes = []

subdir('src' / 'boot')
subdir('src' / 'drivers')
subdir('src' / 'kernel')

subdir('src' / 'lib' / 'libc')
subdir('src' / 'drivers' / 'emmc')

install_headers(kernel_includes)

kernel_elf = executable('kernel8.elf',
  sources : kernel_sources,
  include_directories : include_directories('src' / 'include'),
  c_args : ['-ffreestanding'],
  link_args : ['-nostdlib', '-T', meson.source_root() / 'linker.ld'],
)

kernel_bin = custom_target('kernel8.bin',
  input : [ kernel_elf ],
  output : [ 'kernel8.bin' ],
  command : [objcopy,
    '-O', 'binary',
    '@INPUT@',
    '@OUTPUT@',
  ],
  build_by_default : true,
)

# Generate ramdisk
meson.add_install_script('create_ramdisk', tar)
