// Authors: Jakob Schuppan, Robert Davis
// lastUpdated: 02-21-2019
#include <pthread.h>
#include <list>
#include <assert.h>
#include "scheduler.h"

Scheduler::Scheduler() {
  processCount = 0;
}

//void Scheduler::create_task(functionPtr, threadArg, threadName)
void Scheduler::create_task() {
  int createResult;
  // create a thread
  createResult = pthread_create(pthreads[processCount], NULL, perform_simple_output, threadInfo[processCount]);
  // check if we ran into issues
  assert(!createResult);

  processCount++;

}

void Scheduler::yield() {
  int result;
  //result = callingThread.pthread_yield();

  if (result != 0) {
    perror("Fatal error: pThread failed to yield!");
    exit(0);
  }
}

void* Scheduler :: perform_simple_output(void* arguments) {
  // extract the thread arguments: (method 1)
  // cast arguments in to thread_data
  thread_data *td = (thread_data *) arguments;
  int thread_no = td->thread_no;
  int sleep_time = td->sleep_time;
  //WINDOW * Win = td->thread_win;
  // bool kill_signal = td->kill_signal;
  int CPU_Quantum =0;
  char buff[256];

  //while(!td->kill_signal) {
    //sprintf(buff, " Task-%d running #%d\n", thread_no, CPU_Quantum++);
    //write_window(Win, buff);
    //sleep(thread_no*2);
  //}
}
