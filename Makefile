SRCS=main.c
FLAGS=-lraylib
main: $(SRCS)
	g++ $(SRCS) -o main $(FLAGS)

run: $(SRCS)
	g++ $(SRCS) -o main $(FLAGS)
	./main
