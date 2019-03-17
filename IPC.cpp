#include "IPC.h"
#include <ctime>
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
  // Mailbox tempNewMailbox;
  // tempNewMailbox.threadID = task_Id;
  ezQueue<Message_Type> tMailBox;
  // tempNewMailbox.threadMailBox = tMailBox;
  threadMailboxes.addToEnd(tMailBox, task_Id);
  threadMailboxesArchive.addToEnd(tMailBox, task_Id);

  return 0;
}

int IPC::deleteMailbox(int task_Id)
{
  return 0;
}

int IPC::Message_Send(int sourceTask, int destinationTask, std::string content)
{
  // get mailbox for destinationTask
  IPC::Message_Type newMessage;
  newMessage.source_Task_Id = sourceTask;
  newMessage.destination_Task_Id = destinationTask;
  newMessage.message_Arrival_Time = time(NULL);
  newMessage.message_Size = content.size();
  newMessage.message_Text = content;
  threadMailboxes.getDatumById(destinationTask)->enQueue(newMessage);

  return 0;
}

int IPC::Message_Receive(int task_Id, std::string& content)
{
  // think about implementing a safeguard to prevent one thread
  // from reading messages destined for other threads


  IPC::Message_Type recvMessage;
  recvMessage = *(threadMailboxes.getDatumById(task_Id)->deQueue());
  content = recvMessage.message_Text;
  threadMailboxesArchive.getDatumById(task_Id)->enQueue(recvMessage);

  return 0;
}

int IPC::Message_Count(int task_Id)
{
  int sizeUnread = threadMailboxes.getDatumById(task_Id)->getSize();
  int sizeRead = threadMailboxesArchive.getDatumById(task_Id)->getSize();

  return 0;
}

int IPC::Message_Count()
{
  return 0;
}

void IPC::Message_Print(int task_Id)
{
  //
}

int IPC::Message_DeleteAll(int task_Id)
{
  return 0;
}

void IPC::setMCB(MCB* mcb)
{
  this->mcb = mcb;
}
