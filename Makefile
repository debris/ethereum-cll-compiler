# Makefile

SRCS	= $(shell echo src/*.c)
OBJS 	= obj/bison.o obj/lex.o
OBJS	+= $(SRCS:.c=.o)

CC 	= gcc
CFLAGS	= -g -Wall -pedantic -L/usr/local/Cellar/flex/2.5.37/lib -L/usr/local/Cellar/jansson/2.5/lib
JANS_H	= -I/usr/local/Cellar/jansson/2.5/include

all:			dirs bin/cll_compiler

dirs:		
			test -d bin || mkdir bin
			test -d gen || mkdir gen
			test -d obj || mkdir obj

bin/cll_compiler:	$(OBJS)
			$(CC) $(CFLAGS) $(OBJS) -o bin/cll_compiler -lfl -lm -ljansson

obj/lex.o:		gen/lex.c
			$(CC) $(CFLAGS) -c gen/lex.c -o obj/lex.o

gen/lex.c: 	 	src/cll_compiler.lex
			flex src/cll_compiler.lex
			mv lex.yy.c gen/lex.c

obj/bison.o:		gen/bison.c
			$(CC) $(CFLAGS) -c gen/bison.c -o obj/bison.o

gen/bison.c:		src/cll_compiler.y
			bison -d -v src/cll_compiler.y
			mv cll_compiler.tab.c gen/bison.c
			mv cll_compiler.output gen/
			cmp -s cll_compiler.tab.h include/tokens.h || mv cll_compiler.tab.h include/tokens.h

%.o:			src/%.c
			$(CC) $(CFLAGS) $(JANS_H) -c $< 

obj/lex.o	 	: include/tokens.h include/cll_compiler.h

clean:		
	rm -rf gen obj bin include/tokens.h include/lex.h src/*.o

