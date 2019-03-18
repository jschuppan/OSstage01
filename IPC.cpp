/*===========================================================================
Programmers   : Jakob Schuppan, Robert Davis
File          : scheduler.cpp
Date          : March 17, 2019
Purpose       : implementation of IPC.h
============================================================================*/

// necessary includes
#include <ctime>
#include <iomanip>
#include <string>
#include <fstream>
#include "IPC.h"
#include "sema.h"


//constructor
IPC::IPC()
{
  this->mcb = NULL;
}
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
  //mcb->messageSema->down(task_Id);
  threadMailboxes.addToEnd(tMailBox, task_Id);
  threadMailboxesArchive.addToEnd(tMailBox, task_Id);
  //mcb->messageSema->up();

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
  mcb->messageSema->down(task_Id);
  threadMailboxes.removeNodeByElement(task_Id);
  //mcb->messageSema->up();
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
  //mcb->messageSema->down(task_Id);
  threadMailboxes.getDatumById(destinationTask)->enQueue(newMessage);
  //mcb->messageSema->up();

  return 1;
}

/*-----------------------------------------------------------------
Function      : Message_Receive
Parameters    : task_Id - the thread that is wanting to read its messages
                content - the content of the message
Returns       : 1 - if successful
Details       : allows threads to read messages from their virtual mailbox
------------------------------------------------------------------*/
int IPC::Message_Receive(int task_Id, std::string& content)
{
  // think about implementing a safeguard to prevent one thread
  // from reading messages destined for other threads
  //mcb->messageSema->down(task_Id);
  Message_Type message;
  std::ofstream mRec;
  message = threadMailboxes.getDatumById(task_Id)->deQueue();
  mRec.open("messageReceiveDebug.txt", std::ofstream::out | std::ofstream::app);
  mRec << message.message_Text << std::endl;
  //message->message_Text ="  THIS IS BULLSHIT\n";
  content = message.message_Text;
  threadMailboxesArchive.getDatumById(task_Id)->enQueue(message);
  mRec.close();
  //mcb->messageSema->up();
  if (threadMailboxes.getDatumById(task_Id)->getSize() > 0)
    return 1;
  else
    return 0;
}

/*-----------------------------------------------------------------
Function      : Message_Count
Parameters    : task_Id - the thread which message_count we will display
Returns       : 1 - if successful
Details       : allows us to gain insight into how many messages a thread has
------------------------------------------------------------------*/
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

// TBI
int IPC::Message_Count()
{
  return 0;
}

/*-----------------------------------------------------------------
Function      : Message_Print
Parameters    : task_Id - the thread which messages we will print
Returns       : msgPrntBuf - a string containing all infos containing a
                             threads info
Details       : this function allows us to print detailed information
                about the messages of a thread (including its content)
------------------------------------------------------------------*/
std::string IPC::Message_Print(int task_Id)
{
  std::string msgPrntBuf;
  // wirte in reference buffer?
  std::ofstream mPrint;
  std::string msgContent;
  // struct tm * timeDetail;
  // timeinfo = localtime (&rawtime);
  //mcb->messageSema->down(task_Id);
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
  //mcb->messageSema->up();
  return msgPrntBuf;
}

/*-----------------------------------------------------------------
Function      : Message_DeleteAll
Parameters    : task_Id - the thread which messages we will delete
Returns       : 1 - if successful
Details       : this function allows us to delete all messages of a
                given thread
------------------------------------------------------------------*/
int IPC::Message_DeleteAll(int task_Id)
{
  //mcb->messageSema->down(task_Id);
  ezQueue<IPC::Message_Type>* stdMessages = threadMailboxes.getDatumById(task_Id);
  ezQueue<IPC::Message_Type>* stdMessagesArchive = threadMailboxesArchive.getDatumById(task_Id);
  while(stdMessages->getSize() > 0) {
    stdMessages->deQueue();
  }

  while(stdMessagesArchive->getSize() > 0) {
    stdMessagesArchive->deQueue();
  }
  //mcb->messageSema->up();
  return 1;
}


void IPC::setMCB(MCB* mcb)
{
  this->mcb = mcb;
}

// IPC::Message_Type IPC::getMessage()
// {
//   return message;
// }
