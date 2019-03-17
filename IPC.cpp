/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis
File          : scheduler.cpp
Date          : March 17, 2019
Purpose       : implementation of IPC.h
============================================================================*/
#include "IPC.h"
#include <ctime>
#include <iomanip>
#include <string>
#include <fstream>


/*-----------------------------------------------------------------
Function      : createMailbox
Parameters    : int task_Id for which we will create the mailbox
Returns       : 1 - if successful
Details       : creates a mailbox for a thread
------------------------------------------------------------------*/
int IPC::createMailbox(int task_Id)
{
  // create a mailbox queue for thread
  ezQueue<Message_Type> tMailBox;

  // push it on both the regular (unread messages) and
  // archive(unread messages)  mailbox
  threadMailboxes.addToEnd(tMailBox, task_Id);
  threadMailboxesArchive.addToEnd(tMailBox, task_Id);

  return 1;
}

/*-----------------------------------------------------------------
Function      : deleteMailbox
Parameters    : int task_Id for which we will delete the mailbox
Returns       : 1 - if successful
Details       : deletes a mailbox for a thread
------------------------------------------------------------------*/
int IPC::deleteMailbox(int task_Id)
{
  threadMailboxes.removeNodeByElement(task_Id);
  return 0;
}

/*-----------------------------------------------------------------
Function      : Message_Send
Parameters    : sourceTask - the thread that is sending the message
                destinationTask - the thread the message is supposed to go to
                content - the actual body of the message
Returns       : 1 - if successful
Details       : allows threads to send messages to other threads
------------------------------------------------------------------*/
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

std::string IPC::Message_Print(int task_Id)
{
  std::string msgPrntBuf;
  // wirte in reference buffer?
  std::ofstream mPrint;
  std::string msgContent;
  // struct tm * timeDetail;
  // timeinfo = localtime (&rawtime);

  ezQueue<IPC::Message_Type>* stdMessages = threadMailboxes.getDatumById(task_Id);
  IPC::Message_Type* iter = NULL;

  mPrint.open("messageDebug.txt", std::ofstream::out | std::ofstream::app);
  // Message_Receive(task_Id, msgContent);

  mPrint << "--------------------------------------------------" << std::endl
         << "Messages for thread " << task_Id << ":" << std::endl;
  for (int i = 0; i < stdMessages->getSize(); i++) {
    iter = stdMessages->getNextElement(iter);
    mPrint << "  Message " << i+1 << ": " << std::endl;
    mPrint << "    Arrival time | size | text                | dest. Thread | source Thread"
    << std::endl;
    mPrint << "    " << std::setw(12) << iter->message_Arrival_Time //<< std::endl
           << " " << std::setw(7) << iter->message_Size //<< std::endl
           << " " << std::setw(20) << iter->message_Text //<< std::endl
           << " " << std::setw(13) << iter->destination_Task_Id //<< std::endl
           << " " << std::setw(12) << iter->source_Task_Id //<< std::endl
           << std::endl;
  }
  mPrint << "--------------------------------------------------" << std::endl
         << std::endl<< std::endl;

  mPrint.close();

  return msgPrntBuf;
}

int IPC::Message_DeleteAll(int task_Id)
{
  ezQueue<IPC::Message_Type>* stdMessages = threadMailboxes.getDatumById(task_Id);
  ezQueue<IPC::Message_Type>* stdMessagesArchive = threadMailboxesArchive.getDatumById(task_Id);
  while(stdMessages->getSize() > 0) {
    stdMessages->deQueue();
  }

  while(stdMessagesArchive->getSize() > 0) {
    stdMessagesArchive->deQueue();
  }

  return 0;
}

void IPC::setMCB(MCB* mcb)
{
  this->mcb = mcb;
}
