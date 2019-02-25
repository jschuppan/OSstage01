/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis
File          : sema.cpp
Date          : Febuary 25, 2019
Purpose       : implementation of sema.h
============================================================================*/
#ifndef SEMA_CPP
#define SEMA_CPP
#include <stdio.h>
#include <iostream>
#include <queue>
#include "ezQueue.h"
#include <string.h>
#include "sema.h"
#include "scheduler.h"

//Constructor
Semaphore::Semaphore(std::string resName)
{
  this->resName = resName;
  this->sema_value = 1;
  this->lastPop = -5;
}

/*-----------------------------------------------------------------
Function      : down(int id);
Parameters    : int threadID
Returns       : void
Details       : checks if resource is available and allows the thread
                to use it.
                else the thread is put into a Queue until the resource
                is available
------------------------------------------------------------------*/
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
    sr->TCBList.getDatumById(threadID)->setState(2);

    // we need to deal with requests until the queue
    // is empty
    while(lastPop != threadID);
    // std::cout << "CLEAR" << std::endl;

    //callToScheduler();
    sr->TCBList.getDatumById(threadID)->setState(1);
  }

  // otherwise there is an issue
  else
  {
    perror("Invalid Semaphore state. Quitting!");
    exit(0);
  }
}

/*-----------------------------------------------------------------
Function      : up();
Parameters    :
Returns       : void
Details       : unlocks the mutex and allows the resource to be used
                by another thread
------------------------------------------------------------------*/
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

/*-----------------------------------------------------------------
Function      : dump(Window* targetWin, int level);
Parameters    : Window* targetWin , int level to dump
Returns       : void
Details       : outputs resource name and Queue contents into targetWin
------------------------------------------------------------------*/
void Semaphore :: dump(Window* targetWin, int level)
{
  char buff[256];
  char resourceName[50];
  // convert our std::string to a c-string for sprintf
  strcpy(resourceName, this->resName.c_str());
  sprintf(buff,"\n \n  Semaphore:\n");
  sprintf(buff+ strlen(buff),"    Resource Name: %s \n\n", resourceName);
  sprintf(buff+ strlen(buff),"    Queue Status: \n");

  int *nextElement = NULL;
  if(processQueue.isEmpty())
  {
      sprintf(buff + strlen(buff), "       Queue is empty");
  }
  else
  {
      sprintf(buff + strlen(buff), "Queue : ");
      //loops until the end of the queue
      while ((nextElement = processQueue.getNextElement(nextElement)))
      {
          sprintf(buff + strlen(buff), "  %d, ", *nextElement);
      }
  }

  targetWin->write_window(buff);
}

/*-----------------------------------------------------------------
Function      : retrieveSchedulerObject(void* schedObj);
Parameters    : void* scheduler object
Returns       : void
Details       : sets schedRef to the paramenter object
------------------------------------------------------------------*/
void Semaphore::retrieveSchedulerObject(void* schedObj)
{
  schedRef = schedObj;
}
#endif
