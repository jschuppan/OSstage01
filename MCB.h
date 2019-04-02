/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis
File          : MCB.h
Date          : March 18, 2019
Purpose       : Help prevent circular dependancy
============================================================================*/

#ifndef MCB_H
#define MCB_H

//=================================
// Forward declarations
class Scheduler;
class Semaphore;
class UI;
class IPC;
class Mem_Mgr;


struct MCB
{
  public:
    Scheduler* s;
    IPC* ipc;
    Semaphore* writeSema;
    Semaphore* messageSema;
    UI* userInf;
  //  Mem_Mgr* mem_mgr;
    MCB();
};

#endif
