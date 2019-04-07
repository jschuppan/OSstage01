/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis
File          : sema.h
Date          : Febuary 25, 2019
Purpose       : implementation of a semaphore to regulate what is
                using a reasource
============================================================================*/
#ifndef SEMA_H
#define SEMA_H

//=================================
// Forward declarations
class Window;

//=================================
// necessary includes
//#include <mutex>
#include "ezQueue.h"
#include "MCB.h"

class Semaphore {
  public:
    /******************************* START PUBLIC MEMBERS *******************/
    Semaphore(std::string resName, int semaValue);
    ~Semaphore();
    void down(int threadID);
    void up();
    void dump(Window* targetWin,int level);
    void retrieveSchedulerObject(void* schedObj);
    void setMCB(MCB* mcb);
    /******************************** END PUBLIC MEMBERS **********************/

  private:
    /******************************* START PRIVATE MEMBERS ********************/
    std::string resName;
    int sema_value;
    ezQueue<int> processQueue;
    //std::mutex resMutex;
    pthread_mutex_t resMutex;
    
    int lastPop;
    void* schedRef; //need to remove for mcb
    MCB* mcb;
    /******************************* END PRIVATE MEMBERS ********************/
};

#endif
