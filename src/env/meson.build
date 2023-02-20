env_src = files([
  'env.c',
  'env.h',
  '../utils/utils.h',
])

env_inc = include_directories('.')

env_lib = static_library('env', env_src)

env_dep = declare_dependency(
  link_with : env_lib,
  include_directories : env_inc
  )
