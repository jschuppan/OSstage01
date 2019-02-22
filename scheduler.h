#ifndef SCHEDULER_H
#define SCHEDULER_H

class Scheduler {
  private:
    struct TCB {
      int threadID, state;
      std::string name;
    };
    TCB* process_table;
    int processCount;
    pthread_t[MAX_WINDOWS_THREADS];

  public:
    void create_task(functionPtr, threadArg, threadName);   // create appropriate data structures and calls coroutine()
    void destroy_task();  // to kill a task (Set its status to DEAD)
    void yield();  // strict round robin process switch.
    void dump(int level);
    void garbage_collect();

};

#endif
