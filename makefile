all: main server

main: 
	gcc -o main main.c fun_main.c var.h fun_main.h -lpthread -lrt -fcommon

server: 
	gcc -o server server.c fun_server.c var.h fun_server.h -lpthread -lrt -fcommon
