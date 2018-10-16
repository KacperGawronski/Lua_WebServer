CFLAGS=-pthread -llua5.3
OBJS=$(shell find ./ -name "*.c"|sed 's/.cpp/.o/g'|sed 's/^\.\///g')
all: webserver
webserver: $(OBJS)
	gcc $(OBJS) -Bstatic -o webserver $(CFLAGS)
.c.o:
	gcc -c -o $@ $< $(CFLAGS)
