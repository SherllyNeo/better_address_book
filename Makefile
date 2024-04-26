CFLAGS = -g3 -O -Wall -W -pedantic -std=c99

babook src/main.c: src/contacts.h src/contacts.c src/file.h src/file.h src/shared.h
	mkdir -p bin
	gcc $(CFLAGS) src/main.c  -o bin/babook 
install:  src/contacts.h src/contacts.c src/file.h src/file.h src/shared.h
	mkdir -p bin
	gcc $(CFLAGS) src/main.c  -o bin/babook && cp -f ./bin/babook $(HOME)/.local/bin/
