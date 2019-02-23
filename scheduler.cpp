// Authors: Jakob Schuppan, Robert Davis
// lastUpdated: 02-21-2019
#include <pthread.h>
#include <list>
#include <assert.h>
#include "scheduler.h"
#include "sema.h"
#include <fstream>
#include<unistd.h> //sleep

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
  int threadID = td->thread_no;

  for (int i=0; i < 10000; i++) {
    tempCounter++;
    //std::ofstream debugFile2;

    sprintf(buff, "Task-%d running #%d\n",threadID,tempCounter);
    //ns.down(td->thread_no);
    //ns.down(td->thread_win);
    td->thread_win->write_window(buff);

    ns.down();
    sprintf(buff, " Thread-%d currently running.\n",threadID);
    td->console_win->write_window(buff);


    //debugFile2.open("debug_thread.txt");
    //debugFile2 << "thread# = " << &td->sleep_time<<"\n";
    //debugFile2.close();

    // dump(5);
    sleep(1);
    //// the issue with the core dump is related to how the window
    //// ptr is passed
    //twindow->display_help();
  }
}
