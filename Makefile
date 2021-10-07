cfiles := $(wildcard *.c)

compile: $(cfiles) $(hfiles)
	gcc -O2 -o high_level_compiler $(cfiles) -lm