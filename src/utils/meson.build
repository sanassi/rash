utils_src = files([
  'utils.h',
  './file_tools.c',
  './str_utils.c',
  './hash_map/hash.c',
  './hash_map/hash_map.c',
  './hash_map/hash_map.h',
  './my_itoa/my_itoa.c',
  './vector/vector.c',
  './vector/vector.h'
])

utils_inc = include_directories('.')

utils_lib = static_library('utils', utils_src)

utils_dep = declare_dependency(
  link_with : utils_lib,
  include_directories : utils_inc
  )
