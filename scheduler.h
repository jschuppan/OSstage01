#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <string>
#include <pthread.h>
#include <queue>
#include "window.h"
#include "linkedlist.h"
class Scheduler {
  //Change to private
public:
    struct thread_data {
      int thread_no;
      Window* thread_win;
      Window* head_win;
      Window* console_win;
      bool kill_signal;
      int sleep_time;
      int thread_results;
      int state = 1;

    public:
      void setState(int state){this->state = state;}
      int getState() { return this->state; }
    };

    struct TCB
    {
      // state: 0 (running), 1 (ready), 2 (blocked)
    private:
      thread_data* threadData;
      int threadID, state;
    public:
      void setThreadData(thread_data* d){this->threadData = d;}
      void setThreadID(int ID){this->threadID = ID;}
      void setState(int state){this->state = state;}
      thread_data* getThreadData(){return threadData;}
      int getThreadID() { return this->threadID; }
      int getState() { return this->state; }
    };

    linkedList <TCB> TCBList;

    //TCB* process_table;
    int processCount;
    int tempCounter;
    pthread_t pthreads[6];
    thread_data threadInfo[6];
    //void* peintrform_simple_output(void* arguments);
    //typedef void * (*THREADFUNCPTR)(void *);


  public:
    Scheduler();
    int running(int ID);
    void create_task(Window* win, Window* headerWin, Window* consoleWin);   // create appropriate data structures and calls coroutine()
    void destroy_task();  // to kill a task (Set its status to DEAD)
    void yield();  // strict round robin process switch.
    void dump(int level);
    void garbage_collect();

};

#endif
