#ifndef MCB_H
#define MCB_H

//=================================
// Forward declarations
class Scheduler;
class Semaphore;
class UI;
class IPC;


struct MCB
{
  public:
    Scheduler* s;
    IPC* ipc;
    Semaphore* writeSema;
    Semaphore* messageSema;
    UI* userInf;
    MCB();
};

#endif
