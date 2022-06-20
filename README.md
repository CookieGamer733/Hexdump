# Hexdump

A simple hexdump utility.

## Installation

### Windows

To install hexdump, download the source code and run `make` or if you don't have
make installed, you can run `gcc -Wall -Werror -O3 hexdump.c -o hexdump.exe`

After compiling, move the executable to the location where you want it to be executed and add it to your PATH.

After installation, you can run the executable by typing `hexdump` in the command line.

### Linux

To install hexdump, download the source code and run `make` or if you don't have
make installed, you can run `gcc -Wall -Werror -O3 hexdump.c -o hexdump`

After compiling, move the executable to `usr/bin/`.

After installation, you can run the executable by typing `./hexdump` in the command line.

## Usage

```bash
  hexdump [OPTIONS] [FILE]
```

  * `-h` `--help`: Prints the help message.
  * `-v` `--version`: Prints the version of the program.
  * `--show-ascii`: Prints ASCII text in the sidebar.
  * `--no-color`: Disables color output

## License

This software is licensed under the [ISC license](LICENSE).