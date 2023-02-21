project('rash', 'c',
  version : '0.1',
  default_options : [
    'warning_level=3',
    'c_std=c99',
    'c_args=-D_POSIX_C_SOURCE=200809L -Wvla -Wall -Wextra -Werror -pedantic'
    ])

  subdir('src/lexer')
  subdir('src/stream')
  subdir('src/parser')
  subdir('src/execution')
  subdir('src/utils')
  subdir('src/expansion')
  subdir('src/env')
  subdir('src/args')
  subdir('src/repl')

  include_directories('tests')

_rash_src = files([ 'src/main.c'])
_rash = executable('rash',
  _rash_src,
  dependencies : [
  lexer_dep,
  stream_dep,
  parser_dep,
  utils_dep,
  execution_dep,
  expansion_dep,
  env_dep,
  args_dep,
  repl_dep
  ],
)

test_suite = find_program('tests/test.sh')
test('suite', test_suite)
