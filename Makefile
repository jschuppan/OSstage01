OBJS	= Ultima.o scheduler.o sema.o UI.o window.o linkedlist.o ezQueue.o IPC.o MCB.o mem_mgr.o
SOURCE	= scheduler.cpp sema.cpp ultima.cpp UI.cpp window.cpp linkedlist.cpp ezQueue.cpp IPC.cpp mem_mgr.cpp
OUT	= phase03
CC	 = g++
FLAGS	 = -g -c -Wall
LFLAGS	 = -lncurses -lpthread

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

Ultima.o: Ultima.cpp
	$(CC) $(FLAGS) Ultima.cpp

scheduler.o: scheduler.cpp
	$(CC) $(FLAGS) scheduler.cpp -std=c++11

MCB.o: MCB.cpp
	$(CC) $(FLAGS) MCB.cpp

sema.o: sema.cpp
	$(CC) $(FLAGS) sema.cpp

ezQueue.o: ezQueue.cpp
	$(CC) $(FLAGS) ezQueue.cpp

IPC.o: IPC.cpp
	$(CC) $(FLAGS) IPC.cpp

mem_mgr.o: mem_mgr.cpp
	$(CC) $(FLAGS) mem_mgr.cpp

UFS.o: UFS.cpp
	$(CC) $(FLAGS) UFS.cpp
	
clean:
	rm -f $(OBJS) $(OUT) *.txt *.gch *~
