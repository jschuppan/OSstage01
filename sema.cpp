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
    std::cout << "Setting lock" << std::endl;
    resMutex.lock();
  }
  // case 2: resource is unavailable
  else if (sema_value == 0)
  {
    // first push new process request on queue
    std::cout << "Pushing thread " << threadID
              << " on queue!" << std::endl;
    processQueue.push(threadID);
    // we need to deal with requests until the queue
    // is empty
    while(!processQueue.empty() || (lastPop != pthread_self()))
      {
        // std::cout << "stuck" << std::endl;
      }
    std::cout << "cleared while!" << std::endl;
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
    std::cout << "Queue empty. Unlocking sema!" << std::endl;
    resMutex.unlock();
    sema_value = 1;
  }
  else {
    // next in queue gets released

    // get threadID from pop
    lastPop = processQueue.front();
    processQueue.pop();
    std::cout << "last pop ID: " << lastPop << std::endl;
  }

  // if (queue.isEmpty()) {
  // sema_value = 1;
//}
}
