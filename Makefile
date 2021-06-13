all: server client

server: obj/aurrasd.o bin/aurrasd

client: obj/aurras.o bin/aurras

bin/aurrasd: obj/aurrasd.o
    gcc -g obj/aurrasd.o -o bin/aurrasd

obj/aurrasd.o: src/aurrasd.c
    gcc -Wall -g -c src/aurrasd.c -o obj/aurrasd.o

bin/aurras: obj/aurras.o
    gcc -g obj/aurras.o -o bin/aurras

obj/aurras.o: src/aurras.c
    gcc -Wall -g -c src/aurras.c -o obj/aurras.o

d_aurrasd:
    gdb bin/aurrasd

d_aurras:
    gdb bin/aurras

clean:
    rm obj/* tmp/* bin/aurras bin/aurrasd

test:
    bin/aurras
    bin/aurras status
    bin/aurras transform samples/sample-1-so.m4a output.m4a alto eco rapidoteste
