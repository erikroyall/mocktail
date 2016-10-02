CFLAGS = -g -Oo

all: mocktail

mocktail: main.c lib/mongoose.c 
	gcc -g -o $@ main.c lib/mongoose.c -Llib -ljerry-core -ljerry-libm

clean:
	-rm mocktail

.PHONY: all clean
