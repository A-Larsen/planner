CC = gcc
build: database.c cli.c calendar.c
	${CC} database.c cli.c calendar.c -o calendar -lm -lsqlite3 
