CFLAGS=-Wall -g

default: all

all:
		cc $(CFLAGS) server.c -o server
			$(info Executing target "all"!)
clean:
		rm server
