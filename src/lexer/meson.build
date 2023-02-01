lexer_src = files([
  'lexer.c',
  'lexer.h',
  '../utils/utils.h',
])

lexer_inc = include_directories('.')

lexer_lib = static_library('lexer', lexer_src)

lexer_dep = declare_dependency(
  link_with : lexer_lib,
  include_directories : lexer_inc
  )
