#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <string>
#include <pthread.h>
#include "Ultima.cpp"

class Scheduler {
  private:
    struct thread_data {
      int thread_no;
      //WINDOW *thread_win;
      bool kill_signal;
      int sleep_time;
      int thread_results;
    };
    struct TCB {
      int threadID, state;
      std::string name;
    };
    TCB* process_table;
    int processCount;
    pthread_t pthreads[MAX_WINDOWS_THREADS];
    thread_data threadInfo[MAX_WINDOWS_THREADS];
    static void* perform_simple_output(void* arguments);
    Scheduler();

    typedef void * (*THREADFUNCPTR)(void *);


  public:
    void create_task();   // create appropriate data structures and calls coroutine()
    void destroy_task();  // to kill a task (Set its status to DEAD)
    void yield();  // strict round robin process switch.
    void dump(int level);
    void garbage_collect();

};

#endif
