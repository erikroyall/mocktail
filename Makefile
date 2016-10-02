CFLAGS = -g -Oo

all: mocktail

mocktail: main.c lib/mongoose.c lib/v7.c
	gcc -g -o $@ main.c lib/mongoose.c lib/v7.c -lm

clean:
	-rm mocktail

.PHONY: all clean
