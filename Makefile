# use your favourite compiler
# g++ or clang
CC=gcc
SRCS=main.c
FLAGS=-lraylib
WARNS=-Wvla-larger-than=300
main: $(SRCS)
	$(CC) $(SRCS) -o main $(FLAGS) $(WARNS)

run: $(SRCS)
	$(CC) $(SRCS) -o main $(FLAGS)
	./main
