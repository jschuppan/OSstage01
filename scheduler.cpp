// Authors: Jakob Schuppan, Robert Davis
// lastUpdated: 02-21-2019
#include <pthread.h>
#include <list>
#include <assert.h>
#include "scheduler.h"
#include <fstream>
#include<unistd.h> //sleep
void* perform_simple_output(void* arguments);
Scheduler::Scheduler()
{
  processCount = 0;
  tempCounter = 0;
}

//void Scheduler::create_task(functionPtr, threadArg, threadName)
int Scheduler::create_task(Window* Win) {
  int createResult;
  threadInfo[processCount].thread_win = Win;
  std::ofstream debugFile2;
  //debugFile2.open("debug.txt");
//  debugFile2 << "thread# = " << &temp.sleep_time <<"\n";
  //debugFile2.close();
  // create a thread
  createResult = pthread_create(&pthreads[processCount], NULL, perform_simple_output, &threadInfo[processCount]);
  //threadInfo[0].thread_win->display_help();
  // wait for termination and check if we ran into issues
  //createResult = pthread_join(pthreads[processCount], NULL);
  //assert(!createResult);

  processCount++;
  return threadInfo[processCount].thread_no;

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

void* perform_simple_output(void* arguments)
{
  int tempCounter =0;
  for (int i=0; i < 10000; i++) {
    tempCounter++;
    //std::ofstream debugFile2;
    char buff[256];
    Scheduler :: thread_data* td = (Scheduler::thread_data*) arguments;
    sprintf(buff, "Task-%d running #%d\n",td->thread_no,tempCounter);
    td->thread_win->write_window(buff);

    //debugFile2.open("debug_thread.txt");
    //debugFile2 << "thread# = " << &td->sleep_time<<"\n";
    //debugFile2.close();


    //// the issue with the core dump is related to how the window
    //// ptr is passed
    //twindow->display_help();
    sleep(1);
  }
}
