/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis
File          : sema.cpp
Date          : Febuary 25, 2019
Purpose       : implementation of sema.h
============================================================================*/
#ifndef SEMA_CPP
#define SEMA_CPP

//=================================
// necessary includes
#include <stdio.h>
#include <iostream>
#include <queue>
#include <string.h>
#include "sema.h"
#include "ezQueue.h"
#include "scheduler.h"

const int BLOCKED = 2;
const int READY = 1;

//Constructor
Semaphore::Semaphore(std::string resName, int semaValue)
{
  this->resName = resName;
  this->sema_value = semaValue;
  this->lastPop = -5;
  pthread_mutex_init(&resMutex, NULL);
}


//Destructor
Semaphore::~Semaphore()
{
  pthread_mutex_destroy(&resMutex);
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

  // we'll suspend the process in the scheduler
  // case 1: resource is available
  if (sema_value > 0)
  {
    // make resource unavailable
    sema_value--;
  }

  // case 2: resource is unavailable
  else if (sema_value <= 0)
  {
    //resMutex.lock();
    pthread_mutex_lock(&resMutex);
    // set out mutex lock to prevent resource from
    // being used by multiple threads
    // first push new process request on queue
    processQueue.enQueue(threadID);
    // we'll suspend the process in the scheduler
    mcb->s->TCBList.getDatumById(threadID)->setState(BLOCKED);

    // we need to deal with requests until the queue
    // is empty
    if(!mcb->s->THREAD_SUSPENDED)
    {
        while(lastPop != threadID);
        //callToScheduler();
        mcb->s->TCBList.getDatumById(threadID)->setState(READY);
    }
    //resMutex.unlock();
    pthread_mutex_unlock(&resMutex);
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
  //resMutex.lock();
  pthread_mutex_lock(&resMutex);
  // next in queue gets released
  // get threadID from pop
  if(!mcb->s->THREAD_SUSPENDED && !processQueue.isEmpty())
      lastPop = processQueue.deQueue();

  // check if queue is now empty. If so release and unlock
  sema_value++;
  //resMutex.unlock();
  pthread_mutex_unlock(&resMutex);
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
      //sprintf(buff + strlen(buff), "Queue : ");
      //loops until the end of the queue
      sprintf(buff + strlen(buff), "\t");
      while ((nextElement = processQueue.getNextElement(nextElement)))
      {
          //nextElement = processQueue.getNextElement(nextElement);
          sprintf(buff + strlen(buff), "%d-> ", *nextElement);
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

/*-----------------------------------------------------------------
Function      : stMCB
Parameters    : MCB*
Returns       :
Details       : Sets the MCB pointer within the class
------------------------------------------------------------------*/
void Semaphore::setMCB(MCB* mcb)
{
  this->mcb = mcb;
}
#endif
