compile:
	gcc -Wall -O3 hexdump.c -o hexdump.exe

compile_debug:
	gcc -Wall -O3 "hexdump copy.c" -o hexdump.exe