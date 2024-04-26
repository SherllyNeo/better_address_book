CFLAGS = -g3 -O -Wall -W -pedantic -std=c99

babook src/main.c: 
	mkdir -p bin
	gcc $(CFLAGS) src/main.c -o bin/babook 
install: src/main.c src/main.c src/mail.c src/mail.h src/shared.h src/composer.h src/composer.c src/parser.c src/parser.h 
	mkdir -p bin
	gcc $(CFLAGS) src/main.c -o bin/babook && cp -f ./bin/babook $(HOME)/.local/bin/
