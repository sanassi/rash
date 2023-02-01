stream_src = files([
  'stream.c',
  'stream.h',
  '../utils/utils.h',
])


stream_inc = include_directories('.')

stream_lib = static_library('stream', stream_src)

stream_dep = declare_dependency(
  link_with : stream_lib,
  include_directories : stream_inc
  )
