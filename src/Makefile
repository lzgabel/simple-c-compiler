CFLAGS := -Wall -W -std=gnu++11
CC=g++
parser: lexer.cc gen.cc parser.cc
	$(CC)  $^ -o $@ $(CFLAGS)

clean:
	rm -rf parser
.PHONY: all
