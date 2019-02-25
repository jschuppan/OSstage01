// Authors: Jakob Schuppan, Robert Davis
#include <stdio.h>
#include <iostream>
#include <queue>
#include <fstream>
#include "ezQueue.h"
#include "sema.h"

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

    // SUSPENDINSCHEDULER()
    // we need to deal with requests until the queue
    // is empty
    while(lastPop != threadID);
    // std::cout << "CLEAR" << std::endl;

    //callToScheduler();
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
  std :: ofstream debugDump;
  debugDump.open("semaDump.txt", std::ofstream::app);
  //TCB* myT = NULL;
  debugDump <<"Before Loop" << std::endl;

  while (!processQueue.isEmpty())
  {
    debugDump << processQueue.front();
  }

  debugDump <<"After loop" << std::endl;
}
