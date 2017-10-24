CC = gcc

CFLAGS = -std=c99 -Wall -Wextra -pedantic 

all: brace-topia.c
	gcc -g -Wall -o brace-topia brace-topia.c display.c
clean: 
	$(RM) bracetopia
