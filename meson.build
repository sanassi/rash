project('rash', 'c',
  version : '0.1',
  default_options : [
    'warning_level=3',
    'c_std=c99',
    'c_args=-D_POSIX_C_SOURCE=200809L -Wvla -Wall -Wextra -Werror -pedantic'
    ])

  subdir('src/utils')
  subdir('src/lexer')
  subdir('src/stream')

_rash_src = files([ 'src/main.c'])
_rash = executable('rash',
  _rash_src,
  dependencies : [
  utils_dep,
  lexer_dep,
  ],
)

