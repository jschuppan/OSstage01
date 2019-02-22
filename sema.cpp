#include <cstdio>
#include <iostream>
#include <queue>
#include "sema.h"


Semaphore::Semaphore(std::string resName)
{
  this->resName = resName;
  this->sema_value = 1;
}

void Semaphore::down(int threadID)
{
  // case 1: resource is available
  if (sema_value == 1)
  {
    sema_value = 0;
    //mutexCall();
  }
  // case 2: resource is unavailable
  else if (sema_value == 0)
  {
    //processQueue.push(processID);
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
  // if (queue.isEmpty()) {
  // sema_value = 1;
//}
}
