NAME=pars
CC=g++

all:
	$(CC) -o $(NAME) *.cpp

clean:
	rm  $(NAME)