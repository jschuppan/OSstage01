OBJS	= ultima.o scheduler.o sema.p
SOURCE	= scheduler.cpp sema.cpp ultima.cpp
HEADER	=
OUT	= phase01
CC	 = g++
FLAGS	 = -g -c -Wall
LFLAGS	 = -lncurses -lnpthread

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

ultima.o: ultima.cpp
	$(CC) $(FLAGS) ultima.cpp

scheduler.o: scheduler.cpp
	$(CC) $(FLAGS) scheduler.cpp

sema.o: sema.cpp
	$(CC) $(FLAGS) sema.cpp

clean:
	rm -f $(OBJS) $(OUT)
