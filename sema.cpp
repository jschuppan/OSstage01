#include <stdio.h>
#include <iostream>
#include <queue>
#include "sema.h"


Semaphore::Semaphore(std::string resName)
{
  this->resName = resName;
  this->sema_value = 1;
}

// void Semaphore::down(int threadID) {
//   std::cout << "before lock" << std::endl;
//   resMutex.lock();
//   std::cout << "after lock" << std::endl;
//   resMutex.unlock();
// }

void Semaphore::down(int threadID)
{
  // case 1: resource is available
  if (sema_value == 1)
  {
    // make resource unavailable
    sema_value = 0;
    // set out mutex lock to prevent resource from
    // being used by multiple threads
    resMutex.lock();
  }
  // case 2: resource is unavailable
  else if (sema_value == 0)
  {
    // first push new process request on queu
    processQueue.push();
    // we need to deal with requests until the queue
    // is empty
    while(!processQueue.empty())
    {

    }
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
  if(processQueue.empty())
  {
    resMutex.unlock();
    sema_value = 1;
  }
  else {
    // next in queue gets released
    processQueue.pop();
  }

  // if (queue.isEmpty()) {
  // sema_value = 1;
//}
}
