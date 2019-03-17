#include "MCB.h"
#include "scheduler.h"
#include "IPC.h"
#include "UI.h"

MCB::MCB()
{
  s = new Scheduler();
  ipc = new IPC();
  writeSema = new Semaphore("write_window");
  messageSema = new Semaphore("message_access");
  userInf = new UI();
}
