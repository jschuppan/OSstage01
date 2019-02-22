OBJS	= ultima.o scheduler.o sema.p
SOURCE	= scheduler.cpp sema.cpp ultima.cpp
HEADER	= 
OUT	= hw2
CC	 = g++
FLAGS	 = -g -c -Wall
LFLAGS	 = lncurses lnpthread

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

tokenizer.o: hw2.cpp
	$(CC) $(FLAGS) hw2.cpp

ezQueue.o: ezStack.cpp
	$(CC) $(FLAGS) ezStack.cpp

clean:
	rm -f $(OBJS) $(OUT)
