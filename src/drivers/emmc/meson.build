emmc_sources = [
  'emmc.c',
]

emmc_includes = [
  'emmc.h',
]

emmc_include_directories = include_directories('../../include')

install_headers(emmc_includes)

emmc = shared_library('emmc',
  sources : emmc_sources,
  include_directories : emmc_include_directories,
  build_rpath : meson.build_root(),
  dependencies : [ libc_dep ],
  name_prefix : 'mod',
  install : true,
  install_dir : 'usr' / 'modules',
)
