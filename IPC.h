#ifndef IPC_H
#define IPC_H

#include <string>
#include <iostream>
#include "ezQueue.h"
#include <list>
#include <queue>
#include "MCB.h"
#include "linkedlist.h"

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

    bool operator==(const Message_Type& rhs)
    {
      return (   source_Task_Id == rhs.source_Task_Id
              && destination_Task_Id == rhs.destination_Task_Id
              && message_Arrival_Time == rhs.message_Arrival_Time
              && message_Size == rhs.message_Size);
    }

    // void operator=(const Message_Type& rhs)
    // {
    //   source_Task_Id = rhs.source_Task_Id;
    //   destination_Task_Id = rhs.destination_Task_Id;
    //   message_Arrival_Time = rhs.message_Arrival_Time;
    //   message_Size = rhs.message_Size;
    //   message_Text = rhs.message_Text;
    // }
  };

  // struct Mailbox {
  //   int threadID;
  //   ezQueue<Message_Type> threadMailBox;
  //
  //   bool operator==(const Mailbox& rhs)
  //   {
  //     return (threadID == rhs.threadID);
  //   }
  // };

  linkedList<ezQueue<Message_Type>> threadMailboxes;

  MCB* mcb;
  //------------------Start Public Members-------------------
public:
  IPC();
  int createMailbox(int task_Id);
  int deleteMailbox(int task_Id);
  int Message_Send(int sourceTask, int destinationTask, std::string content);
  int Message_Receive(int task_Id, Message_Type *message);
  int Message_Count(int task_Id);
  int Message_Count();
  void Message_Print(int task_Id);
  int Message_DeleteAll(int task_Id);
  void setMCB(MCB* mcb);
};

#endif
