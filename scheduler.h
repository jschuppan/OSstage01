#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <string>
#include <pthread.h>
#include "window.h"

class Scheduler {
public:
    struct thread_data {
      int thread_no;
      Window* thread_win;
      Window* head_win;
      Window* console_win;
      bool kill_signal;
      int sleep_time;
      int thread_results;
    };

    struct TCB {
      // state: 0 (running), 1 (ready), 2 (blocked)
      int threadID, state;
      std::string name;
    };

    TCB* process_table;
    int processCount;
    int tempCounter;
    pthread_t pthreads[5];
    thread_data threadInfo[5];
    //void* perform_simple_output(void* arguments);
    //typedef void * (*THREADFUNCPTR)(void *);


  public:
    Scheduler();
    int create_task(Window* win, Window* headerWin, Window* consoleWin);   // create appropriate data structures and calls coroutine()
    void destroy_task();  // to kill a task (Set its status to DEAD)
    void yield();  // strict round robin process switch.
    void dump(int level);
    void garbage_collect();

};

#endif
