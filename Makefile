NAME=parser
CC=g++

all:
	$(CC) -o $(NAME) *.cpp
debug:
	$(CC) -g -o $(NAME) *.cpp
clean:
	rm  $(NAME)
