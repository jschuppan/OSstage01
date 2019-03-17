#include "IPC.h"
#include <string>
//#include "Ultima.cpp"

// read and unread queues

//default constructor
IPC::IPC()
{
  // create linkedList of mailbox queues
}

int IPC::createMailbox(int task_Id)
{
  Mailbox tempNewMailbox;
  tempNewMailbox.threadID = task_Id;
  std::queue<Message_Type> tMailBox;
  tempNewMailbox.threadMailBox = tMailBox;
  threadMailboxes.push_back(tempNewMailbox);

  return 0;
}

int IPC::deleteMailbox(int task_Id)
{
  return 0;
}

int IPC::Message_Send(std::string content, int destinationTask)
{

  return 0;
}

int IPC::Message_Receive(int task_Id, Message_Type *message)
{
  return 0;
}

int IPC::Message_Count(int task_Id)
{
  return 0;
}

int IPC::Message_Count()
{
  return 0;
}

void IPC::Message_Print(int task_Id)
{

}

int IPC::Message_DeleteAll(int task_Id)
{
  return 0;
}

void IPC::setMCB(void* mcb)
{
  this->mcb = mcb;
}
