OBJS	= ultima.o scheduler.o sema.p
SOURCE	= scheduler.cpp sema.cpp ultima.cpp
HEADER	=
OUT	= phase01
CC	 = g++
FLAGS	 = -g -c -Wall
LFLAGS	 = -lncurses -lnpthread

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

ultima.o: Ultima.cpp
	$(CC) $(FLAGS) Ultima.cpp

scheduler.o: scheduler.cpp
	$(CC) $(FLAGS) scheduler.cpp

sema.o: sema.cpp
	$(CC) $(FLAGS) sema.cpp

clean:
	rm -f $(OBJS) $(OUT)
