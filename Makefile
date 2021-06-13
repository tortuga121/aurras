INCLDS = -I includes
all: server client

server: obj/aurrasd.o bin/aurrasd

client: obj/aurras.o bin/aurras

bin/aurrasd: obj/aurrasd.o obj/utilities.o obj/filters.o
	gcc -g $(INCLDS) obj/aurrasd.o obj/utilities.o obj/filters.o -o bin/aurrasd

obj/aurrasd.o: src/aurrasd.c
	gcc -Wall $(INCLDS) -g -c src/aurrasd.c -o obj/aurrasd.o

bin/aurras: obj/aurras.o obj/utilities.o filters.o
	gcc -g $(INCLDS) obj/aurras.o obj/utilities.o obj/filters.o -o bin/aurras

obj/aurras.o: src/aurras.c
	gcc -Wall $(INCLDS) -g -c src/aurras.c -o obj/aurras.o

bin/utilities: obj/utilities.o
	gcc -g $(INCLDS) obj/utilities.o -o bin/utilities

obj/utilities.o: src/utilities.c
	gcc -Wall $(INCLDS) -g -c src/utilities.c -o obj/utilities.o

bin/filters: obj/filters.o obj/utilities.o
	gcc -g $(INCLDS) obj/filters.o -o bin/filters

obj/filters.o: src/filters.c
	gcc -Wall  $(INCLDS) -g -c src/filters.c -o obj/filters.o

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
