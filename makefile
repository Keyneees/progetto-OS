OBJS	= main.o fun_main.o server.o fun_server.o
OUT	= main,server

OBJS0	= main.o fun_main.o
SOURCE0	= main.c fun_main.c
HEADER0	= var.h fun_main.h
OUT0	= main

OBJS1	= server.o fun_server.o
SOURCE1	= server.c fun_server.c
HEADER1	= var.h fun_server.h
OUT1	= server

CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -lpthread -lrt

all: main server

main: $(OBJS0) $(LFLAGS)
	$(CC) -g $(OBJS0) -o $(OUT0) $(LFLAGS)

server: $(OBJS1) $(LFLAGS)
	$(CC) -g $(OBJS1) -o $(OUT1) $(LFLAGS)

main.o: main.c
	$(CC) $(FLAGS) main.c 

fun_main.o: fun_main.c
	$(CC) $(FLAGS) fun_main.c 

server.o: server.c
	$(CC) $(FLAGS) $(LFLAGS) server.c 

fun_server.o: fun_server.c
	$(CC) $(FLAGS) fun_server.c 


clean:
	rm -f $(OBJS) $(OUT)
