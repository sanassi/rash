repl_src = files([
  'repl.c',
  'repl.h',
])

repl_inc = include_directories('.')

repl_lib = static_library('repl', repl_src)

repl_dep = declare_dependency(
  link_with : repl_lib,
  include_directories : repl_inc
  )
