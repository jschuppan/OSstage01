// Authors: Jakob Schuppan, Robert Davis
// lastUpdated: 02-21-2019
#include <pthread.h>
#include <list>
#include <assert.h>
#include "scheduler.h"
#include "sema.h"
#include <fstream>
#include <mutex>
#include <cmath>
#include <unistd.h> //sleep

std::mutex mute;


Semaphore ns("write_window");

void* perform_simple_output(void* arguments);

Scheduler::Scheduler()
{
  processCount = 0;
  tempCounter = 0;
}
//TCB State: 0 running, 1 ready, 2 blocked, 3 dead
//void Scheduler::create_task(functionPtr, threadArg, threadName)
void Scheduler::create_task(Window* threadWin, Window* headerWin, Window* consoleWin) {

  int createResult;

  threadInfo[processCount].thread_win = threadWin;
  threadInfo[processCount].head_win = headerWin;
  threadInfo[processCount].console_win = consoleWin;
  threadInfo[processCount].thread_no = processCount;
  TCB tcbTemp;
  tcbTemp.setThreadID(processCount);
  tcbTemp.setState(1);
  tcbTemp.setThreadData(&threadInfo[processCount]);
  TCBList.addToFront(tcbTemp, processCount);


  std::ofstream debugFile2;
  debugFile2.open("debug.txt", std::ofstream::app);
  debugFile2 << "thread# = " <<threadInfo[processCount].head_win <<"\n";
  debugFile2.close();
  // create a thread
  createResult = pthread_create(&pthreads[processCount], NULL, perform_simple_output, &threadInfo[processCount]);
  //threadInfo[0].thread_win->display_help();
  // wait for termination and check if we ran into issues
  //createResult = pthread_join(pthreads[processCount], NULL);
  //assert(!createResult);
  char buff[256];
  sprintf(buff, " Thread-%d created.\n",threadInfo[processCount].thread_no);
  threadInfo[processCount].head_win->write_window(processCount + 1, 1,buff);
  dump(5);
  processCount++;

}

void Scheduler::yield()
{
  int result;
  //result = callingThread.pthread_yield();

  if (result != 0) {
    perror("Fatal error: pThread failed to yield!");
    exit(0);
  }
}

void Scheduler::dump(int level)
{
  std::ofstream debugFile2;
  int bs = level;
  debugFile2.open("debug_thread.txt", std:: ofstream::app);
  debugFile2 << "thread# = " << &threadInfo[0].thread_no<<"\n";
  //debugFile2 << "State:  = " << TCBList.getDatumById(processCount)->getState()<<"\n";
  debugFile2.close();
}

void* perform_simple_output(void* arguments)
{
  int tempCounter =0;
  char buff[256];
  Scheduler :: thread_data* td = (Scheduler::thread_data*) arguments;

  while(true) {
    if(td->state == 0)
    {
        tempCounter = td->state;
        sprintf(buff, "Task-%d running #%d\n",td->thread_no,tempCounter);
        // mute.lock();
        //ns.down(threadID);
        td->thread_win->write_window(buff);
        //ns.up();
        //mute.unlock();

        sprintf(buff, " Thread-%d currently running.\n",td->thread_no);
        // mute.lock();
        //ns.down(threadID);
        td->console_win->write_window(buff);
        //ns.up();
        // mute.unlock();

        // setState()
        td->state = 1;
        sleep(1);
    }
    else
      pthread_yield();

  }
}

int Scheduler:: running(int ID)
{

  std::ofstream debugFile2;
  debugFile2.open("debug_thread.txt", std:: ofstream::app);
  debugFile2 << "running# = " << ID <<"\n";


    if(TCBList.getDatumById(ID)->getThreadData()->getState() != 0)
    {
      if(TCBList.getDatumById(ID+1))
      {
        debugFile2 << "State# = " << TCBList.getDatumById(ID)->getThreadData()->getState()<<std::endl;
        TCBList.getDatumById(ID+1)->getThreadData()->setState(0);
        return ID+1;
      }
      else
      {
        debugFile2 << "State1# = " << TCBList.getDatumById(ID)->getThreadData()->getState()<<std::endl;

        TCBList.getDatumById(0)->getThreadData()->setState(0);
        return 0;
      }
    }
    debugFile2.close();
    return ID;
}
