/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis
File          : scheduler.h
Date          : Febuary 25, 2019
Purpose       : implementation of a scheduler for threads
============================================================================*/
#ifndef SCHEDULER_H
#define SCHEDULER_H

//=================================
// Forward declarations
class MCB;

//=================================
// necessary includes
#include <string>
#include <pthread.h>
#include "window.h"
#include "linkedlist.h"


class Scheduler {

  private:

    /*********************************START STRUCTS ******************************/
    struct thread_data
    {
        int thread_no;
        int mem_handle;
        int mem_size;
        Window* thread_win;
        Window* head_win;
        Window* console_win;
      public:
        Window* getThreadWin() {return this->thread_win;}
        int getMemHandle() {return this->mem_handle;}
        Window* getHeadWin() {return this->head_win;}
        Window* getConsoleWin() {return this->console_win;}
        int getThreadNo() { return thread_no; }
        bool operator==(const thread_data& rhs)
        {
          return (thread_no == rhs.thread_no);
        }
    };

    struct TCB
    {
        // state: 0 (running), 1 (ready), 2 (blocked)
      private:
        thread_data* threadData;
        int TthreadID, Tstate;
      public:
        void setThreadData(thread_data d){this->threadData = &d;}
        void setThreadID(int ID){this->TthreadID = ID;}
        void setState(int state){this->Tstate = state;}
        thread_data* getThreadData(){return threadData;}
        int getThreadID() { return this->TthreadID; }
        int getState() { return this->Tstate; }
        bool operator==(const TCB& rhs)
        {
          return (TthreadID == rhs.TthreadID);
        }
    };
    /********************************* END STRUCTS ****************************************/

    /********************************* START PRIVATE MEMBERS ******************************/
    linkedList <TCB> TCBList;
    int processCount;
    int tempCounter;
    linkedList <pthread_t> pthreads;
    linkedList <thread_data> threadInfo;
    bool SCHEDULER_COMPLETED_RUN;
    void* perform_simple_output(void*);
    MCB *mcb;
    /********************************* END PRIVATE MEMBERS *******************************/


  public:
    /******************************** START PUBLIC MEMBERS ******************************/
    Scheduler();
    linkedList<TCB> getTCBList() { return this->TCBList; }
    linkedList<thread_data> getThreadInfo() { return this->threadInfo; }
    void* running(void* ID);
    void create_task(Window* win, Window* headerWin, Window* consoleWin);   // create appropriate data structures and calls coroutine()
    void destroy_task();  // to kill a task (Set its status to DEAD)
    void yield(int);  // strict round robin process switch.
    void messageDump(Window* targetWin, int level);
    void dump(Window* targetWin, int level);
    void garbage_collect();
    void stop();
    void resume();
    void setMCB(MCB* mcb);
    bool SCHEDULER_SUSPENDED;
    bool THREAD_SUSPENDED;
    void forceWrite(int threadID);
    friend class Semaphore;
    /******************************** END PUBLIC MEMBERS ********************************/

};

#endif
