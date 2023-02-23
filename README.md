# RASH - A minimal "POSIX" Shell

## About the project
Rash is my second attempt at the [42sh](https://github.com/sanassi/42sh) project (in C) from EPITA. The goal is to write a __POSIX Shell__ from scratch.
The project was really fun and I think I could have done better with more preparation (and time).
It's also a nice project to showcase my skills in C, and how much stuff I learned since the beginning of ING1 (3rd year at EPITA).

## Build
Rash uses the Meson Build System to build and compile.
After cloning the repo, go to the root of the repository, and type:

```bash
meson setup build
```

## Usage
In the __build__ directory, you can find the executable, __rash__.
To run the program:
```bash
./rash [options] [script] [arguments..]
```
Rash accepts the following input: a file, a string or from the standard input.
With the following options:
- -c [script], to read the input from a string.

And for debug purposes:
- -d or --debug, to print the input at each step of the parsing.
- -p or --pretty-print, to print the resulting Abstract Syntax Tree representing the input.

## Testing
In the directory tests: the folder __full-tests__ contains more than 300 tests: shell scripts to test each basic functionality
of a POSIX Shell (here bash, with the option --posix).
In the file test_suite.py, the ouput of the program is compared to that of bash (with the option --posix), to check the compliance
of rash to the POSIX standard.
