#include "IPC.h"
#include <ctime>
#include <iomanip>
#include <string>
#include <fstream>
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

  return 1;
}

int IPC::Message_Receive(int task_Id, std::string& content)
{
  // think about implementing a safeguard to prevent one thread
  // from reading messages destined for other threads

  std::ofstream mRec;
  IPC::Message_Type* recvMessage;
  recvMessage = threadMailboxes.getDatumById(task_Id)->deQueue();
  mRec.open("messageReceiveDebug.txt", std::ofstream::out | std::ofstream::app);
  mRec << recvMessage << std::endl;
  content = recvMessage->message_Text;
  threadMailboxesArchive.getDatumById(task_Id)->enQueue(*recvMessage);
  mRec.close();

  if (threadMailboxes.getDatumById(task_Id)->getSize() > 0)
    return 1;
  else
    return 0;
}

int IPC::Message_Count(int task_Id)
{
  std::ofstream mPrint;
  int sizeUnread = threadMailboxes.getDatumById(task_Id)->getSize();
  int sizeRead = threadMailboxesArchive.getDatumById(task_Id)->getSize();
  mPrint.open("messageDebug.txt", std::ofstream::out | std::ofstream::app);
  mPrint << "There are " << sizeUnread << " unread and "
         << sizeRead << " read messages"
         << " for thread " << task_Id << "!" << std::endl;
  mPrint.close();
  return 0;
}

int IPC::Message_Count()
{
  return 0;
}

void IPC::Message_Print(int task_Id)
{
  // wirte in reference buffer?
  std::ofstream mPrint;
  std::string msgContent;
  ezQueue<IPC::Message_Type>* stdMessages = threadMailboxes.getDatumById(task_Id);
  IPC::Message_Type* iter = NULL;

  mPrint.open("messageDebug.txt", std::ofstream::out | std::ofstream::app);
  // Message_Receive(task_Id, msgContent);

  mPrint << "--------------------------------------------------" << std::endl
         << "Messages for thread " << task_Id << ":" << std::endl;
  for (int i = 0; i < stdMessages->getSize(); i++) {
    iter = stdMessages->getNextElement(iter);
    mPrint << "  Message " << i+1 << ": " << std::endl;
    mPrint << "    Arrival time | Message size | Message text | dest ThreadID | source ThreadID"
    << std::endl;
    mPrint << "    " << std::setw(10) << iter->message_Arrival_Time //<< std::endl
           << " " << std::setw(12) << iter->message_Size //<< std::endl
           << " " << std::setw(15) << iter->message_Text //<< std::endl
           << " " << std::setw(10) << iter->destination_Task_Id //<< std::endl
           << " " << std::setw(10) << iter->source_Task_Id //<< std::endl
           << std::endl;
  }
  mPrint << "--------------------------------------------------" << std::endl
         << std::endl<< std::endl;

  mPrint.close();
}

int IPC::Message_DeleteAll(int task_Id)
{
  return 0;
}

void IPC::setMCB(MCB* mcb)
{
  this->mcb = mcb;
}
