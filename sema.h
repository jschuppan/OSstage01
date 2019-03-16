/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis
File          : sema.h
Date          : Febuary 25, 2019
Purpose       : implementation of a semaphore to regulate what is
                using a reasource
============================================================================*/
#ifndef SEMA_H
#define SEMA_H

#include <mutex>
#include <queue>
#include "ezQueue.h"
#include "window.h"

class Semaphore {
  public:
    /******************************* START PUBLIC MEMBERS *******************/
    Semaphore(std::string resName);
    void down(int threadID);
    void up();
    void dump(Window* targetWin,int level);
    void retrieveSchedulerObject(void* schedObj);
    void setMCB(void* mcb);
    /******************************** END PUBLIC MEMBERS **********************/

  private:
    /******************************* START PRIVATE MEMBERS ********************/
    std::string resName;
    int sema_value;
    ezQueue<int> processQueue;
    std::mutex resMutex;
    int lastPop;
    void* schedRef; //need to remove for mcb
    void* mcb;
    /******************************* END PRIVATE MEMBERS ********************/
};

#endif
