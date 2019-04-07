//=================================
// necessary includes
#include "MCB.h"
#include "scheduler.h"
#include "IPC.h"
#include "UI.h"
#include "mem_mgr.h"
#include "sema.h"

//Constructor
MCB::MCB()
{
  s = new Scheduler();
  ipc = new IPC();
  writeSema = new Semaphore("write_window",1);
  messageSema = new Semaphore("message_access",1);
  userInf = new UI();
  mem_mgr = new Mem_Mgr();
}
