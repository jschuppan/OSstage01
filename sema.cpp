// Authors: Jakob Schuppan, Robert Davis
#include <stdio.h>
#include <iostream>
#include <queue>
#include "sema.h"
#include <fstream>

const bool DEBUG = false;


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
    processQueue.push(threadID);

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
  if(processQueue.empty())
  {
    if(DEBUG)
      std::cout << "Queue empty. Unlocking sema!" << std::endl;
    resMutex.unlock();
    sema_value = 1;
  }
  else {
    // next in queue gets released
    // get threadID from pop
    lastPop = processQueue.front();
    processQueue.pop();

    if (DEBUG)
      std::cout << "last pop ID: " << lastPop << std::endl;

    // check if queue is now empty. If so release and unlock
    if (processQueue.empty()) {
      if(DEBUG)
        std::cout << "Queue NOW empty. Unlocking sema!" << std::endl;
      resMutex.unlock();
      sema_value = 1;
    }
  }

}

void Semaphore :: dump(Window* targetWin, int level)
{
  char buff[256];
  sprintf(buff,"/n");
  if(processQueue.empty())
  {
      targetWin->write_window(1,1, "Queue is empty");
  }
  else
  {
      sprintf(buff, "Queue : ");
      while (!processQueue.empty())
      {
          sprintf(buff + strlen(buff), "  %d, ", processQueue.);
      }
      targetWin->write_window();
  }
}
