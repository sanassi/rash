execution_src = files([
  'execution.c',
  'execution.h',
  'builtins/bool.c',
  'builtins/bool.h',
  'builtins/echo.c',
  'builtins/echo.h',
  'redir.c',
  'redir.h',
  'pipeline.h',
  'pipeline.c'
])

execution_inc = include_directories('.')

execution_lib = static_library('execution', execution_src)

execution_dep = declare_dependency(
  link_with : execution_lib,
  include_directories : execution_inc
  )
