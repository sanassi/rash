parser_src = files([
  'parser.c',
  'parser.h',
  'ast.h',
  'ast_free.c',
  'ast_free.h',
  'ast_print.c',
  'ast_print.h',
  '../utils/utils.h',
])

parser_inc = include_directories('.')

parser_lib = static_library('parser', parser_src)

parser_dep = declare_dependency(
  link_with : parser_lib,
  include_directories : parser_inc
  )
