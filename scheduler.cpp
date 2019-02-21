//
#include <pthread.h>
#include <list>
#include "scheduler.h"

void Scheduler::create_task(functionPtr, threadArg, threadName) {

}

void Scheduler::yield() {
  int result;
  //result = callingThread.pthread_yield();

  if (result != 0) {
    perror("Fatal error: pThread failed to yield!");
    exit(0);
  }
}
