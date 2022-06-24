# Hexdump

A simple hexdump utility.

## Installation

To install hexdump, download the source code and run `make` or if you don't have
make installed, you can run `gcc -Wall -Werror -O3 hexdump.c -o hexdump`

After compiling, move the executable where ever you like. (If on windows, don't forget to add it to your path)


## Usage

```bash
  hexdump [options]... [file]
```
### Options

```
  -h, --help            display this help and exit
  -v, --version         output version information and exit
  -o, --output [FILE]   print to FILE instead of stdout
      --output-color    when writing to file, write color escape sequences as well
      --show-ascii      display the ASCII characters
      --no-color        disable color output
``` 
                       |
## License

This software is licensed under the [ISC license](LICENSE).