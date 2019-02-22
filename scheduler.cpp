//
#include <iostream>
#include <pthread.h>
#include <assert.h>
#include <list>
#include "scheduler.h"

Scheduler::Scheduler() {
  processCount = 0;
  TCB threadTCB;
}

//void Scheduler::create_task(functionPtr, threadArg, threadName)
void Scheduler::create_task() {
  int tmpResult;
  tmpResult = pthread_create(pthread_t[processCount], NULL, perform_simple_output, &thread_args_3);
  assert(!result_code);

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
