test_suite_sources = [
  'fcall.c',
  'main.c',
  'mman.c',
  'mqueue.c',
  'stdio.c',
  'stdlib.c',
  'string.c',
  'unistd.c',
]

hello = executable('test_suite',
  sources : test_suite_sources,
  link_args : exec_link_args,
  dependencies : [ libc_dep ],
  pie : true,
  install : true,
)
