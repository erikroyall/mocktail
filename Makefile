CFLAGS = -g -Oo

all: mocktail

mocktail: main.c lib/mongoose.c lib/duktape.c
	gcc -g -o $@ main.c lib/mongoose.c lib/duktape.c -lm

clean:
	-rm mocktail

.PHONY: all clean
