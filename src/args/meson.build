args_src = files([
  'args.c',
  'args.h',
])

args_inc = include_directories('.')

args_lib = static_library('args', args_src)

args_dep = declare_dependency(
  link_with : args_lib,
  include_directories : args_inc
  )
