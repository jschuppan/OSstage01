OBJS	= Ultima.o scheduler.o sema.o
SOURCE	= scheduler.cpp sema.cpp ultima.cpp
HEADER	=
OUT	= phase01
CC	 = g++
FLAGS	 = -g -c -Wall
LFLAGS	 = -lncurses -lpthread

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

Ultima.o: Ultima.cpp
	$(CC) $(FLAGS) Ultima.cpp

scheduler.o: scheduler.cpp
	$(CC) $(FLAGS) scheduler.cpp

sema.o: sema.cpp
	$(CC) $(FLAGS) sema.cpp

clean:
	rm -f $(OBJS) $(OUT)
