CFLAGS = -g3 -Ofast -Wall -W -Wpedantic -Wextra -lncurses 
CC=tcc

babook src/main.c: src/contacts.h src/contacts.c src/file.h src/file.h src/shared.h
	mkdir -p bin
	$(CC) $(CFLAGS) src/main.c  -o bin/babook 
install:  src/contacts.h src/contacts.c src/file.h src/file.h src/shared.h
	mkdir -p bin
	$(CC) $(CFLAGS) src/main.c  -o bin/babook && cp -f ./bin/babook $(HOME)/.local/bin/
