OBJS	= fun_server.o main.o fun.o server.o
OUT	= main, server

OBJS0	= fun_server.o main.o
SOURCE0	= fun_server.c main.c
HEADER0	= fun_main.h var.h
OUT0	= main

OBJS1	= fun.o fun_server.o server.o
SOURCE1	= fun.c fun_server.c server.c
HEADER1	= fun.h fun_server.h var.h
OUT1	= server

CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -lrt

all: main server

main: $(OBJS0) $(LFLAGS)
	$(CC) -g $(OBJS0) -o $(OUT0)

server: $(OBJS1) $(LFLAGS)
	$(CC) -g $(OBJS1) -o $(OUT1)

fun_server.o: fun_server.c
	$(CC) $(FLAGS) fun_server.c 

main.o: main.c,
	$(CC) $(FLAGS) main.c, 

fun.o: fun.c
	$(CC) $(FLAGS) fun.c 

server.o: server.c
	$(CC) $(FLAGS) server.c

clean:
	rm -f $(OBJS) $(OUT)
