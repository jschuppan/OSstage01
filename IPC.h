#ifndef IPC_H
#define IPC_H

#include<string.h>
#include<iostream>

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
  void* mcb;
  //------------------Start Public Members-------------------
public:
  IPC();
  int Message_Send(Message_Type *message);
  int Message_Recieve(int task_Id, Message_Type *message);
  int Message_Count(int task_Id);
  int Message_Count();
  void Message_Print(int task_Id);
  int Message_DeleteAll(int task_Id);
  void setMCB(void* mcb);
};

#endif
