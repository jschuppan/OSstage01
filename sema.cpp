#ifndef SEMA_CPP
#define SEMA_CPP
// Authors: Jakob Schuppan, Robert Davis
#include <stdio.h>
#include <iostream>
#include <queue>
#include <fstream>
#include "ezQueue.h"
#include <string.h>
#include "sema.h"
#include "scheduler.h"

Semaphore::Semaphore(std::string resName)
{
  this->resName = resName;
  this->sema_value = 1;
  this->lastPop = -5;
}

void Semaphore::retrieveSchedulerObject(void* schedObj) {
  schedRef = schedObj;
}

void Semaphore::down(int threadID)
{
  // case 1: resource is available
  if (sema_value == 1)
  {
    resMutex.lock();
    // make resource unavailable
    sema_value = 0;

    // set out mutex lock to prevent resource from
    // being used by multiple threads
  }

  // case 2: resource is unavailable
  else if (sema_value == 0)
  {
    // first push new process request on queue
    processQueue.enQueue(threadID);
    Scheduler* sr = (Scheduler*)schedRef;

    // we'll suspend the process in the scheduler
    sr->TCBList.getDatumById(threadID)->getThreadData()->setState(2);

    // we need to deal with requests until the queue
    // is empty
    while(lastPop != threadID);
    // std::cout << "CLEAR" << std::endl;

    //callToScheduler();
    sr->TCBList.getDatumById(threadID)->getThreadData()->setState(1);
  }

  // otherwise there is an issue
  else
  {
    perror("Invalid Semaphore state. Quitting!");
    exit(0);
  }
}

void Semaphore::up()
{
  // if nothing is queued we can
  // simply release the lock and reset
  // our status to available
  if(processQueue.isEmpty())
  {
    resMutex.unlock();
    sema_value = 1;
  }
  else {
    // next in queue gets released
    // get threadID from pop
    lastPop = processQueue.deQueue();

    // check if queue is now empty. If so release and unlock
    if (processQueue.isEmpty()) {
      resMutex.unlock();
      sema_value = 1;
    }
  }

}

void Semaphore :: dump(Window* targetWin, int level)
{
  char buff[256];
  sprintf(buff,"/n");
  int *nextElement = NULL;
  Scheduler* sr = (Scheduler*)schedRef;

  if(processQueue.isEmpty())
  {
      targetWin->write_window(1,1, "Queue is empty");
  }
  else
  {
      sprintf(buff, "Queue : ");
      while ((nextElement = processQueue.getNextElement(nextElement)))
      {
          sprintf(buff + strlen(buff), "  %d, ", *nextElement);
      }
      targetWin->write_window(buff);
  }
}
#endif
