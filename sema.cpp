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
  char resourceName[50];
  strcpy(resourceName, this->resName.c_str());
  sprintf(buff,"\n \n  Semaphore:\n");
  sprintf(buff+ strlen(buff),"    Resource Name: %s \n\n", resourceName);
  sprintf(buff+ strlen(buff),"    Queue Status: \n", this->resName);

  int *nextElement = NULL;
  if(processQueue.isEmpty())
  {
      sprintf(buff + strlen(buff), "       Queue is empty");
  }
  else
  {
      sprintf(buff, "Queue : ");
      while ((nextElement = processQueue.getNextElement(nextElement)))
      {
          sprintf(buff + strlen(buff), "  %d, ", *nextElement);
      }
  }

  targetWin->write_window(buff);
}


void Semaphore::retrieveSchedulerObject(void* schedObj) {
  schedRef = schedObj;
}
#endif
