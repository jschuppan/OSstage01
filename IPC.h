#ifndef IPC_H
#define IPC_H

#include <string>
#include <iostream>
#include "ezQueue.h"
#include <list>
#include <queue>
#include "MCB.h"

class IPC
{
//-------------------Start Private Members------------------
private:
  struct Message_Type
  {
    int source_Task_Id;
    int destination_Task_Id;
    time_t message_Arrival_Time;
    int message_Size;
    std::string message_Text;
  };

  struct Mailbox {
    int threadID;
    std::queue<Message_Type> threadMailBox;
  };

  std::list<Mailbox> threadMailboxes;

  MCB* mcb;
  //------------------Start Public Members-------------------
public:
  IPC();
  int createMailbox(int task_Id);
  int deleteMailbox(int task_Id);
  int Message_Send(std::string content, int destinationTask);
  int Message_Receive(int task_Id, Message_Type *message);
  int Message_Count(int task_Id);
  int Message_Count();
  void Message_Print(int task_Id);
  int Message_DeleteAll(int task_Id);
  void setMCB(MCB* mcb);
};

#endif
