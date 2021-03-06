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
#include <bits/stdc++.h> // stringstream
#include "IPC.h"
#include "sema.h"
#include "scheduler.h"


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
  mcb->messageSema->down(task_Id);
  threadMailboxes.addToEnd(tMailBox, task_Id);
  threadMailboxesArchive.addToEnd(tMailBox, task_Id);
  mcb->messageSema->up();

  return 1;
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

  mcb->messageSema->down(sourceTask);
  // make sure destionation task is alive before we write
  if (mcb->s->getTCBList().getDatumById(destinationTask)->getState() < 3) {
    threadMailboxes.getDatumById(destinationTask)->enQueue(newMessage);
  }
  mcb->messageSema->up();

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
  // receive a message and deQueue
  Message_Type message;
  mcb->messageSema->down(task_Id);
  message = threadMailboxes.getDatumById(task_Id)->deQueue();
  mcb->messageSema->up();

  // store deQueued message in archive (we are not using this feature yet!)
  content = message.message_Text;
  mcb->messageSema->down(task_Id);
  threadMailboxesArchive.getDatumById(task_Id)->enQueue(message);
  mcb->messageSema->up();

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
Function      : Message_Print(int task_Id)
Parameters    : task_Id - the thread which messages we will print
Returns       : msgPrntBuf - a string containing all infos containing a
                             threads info
Details       : this function allows us to print detailed information
                about the messages of a thread (including its content)
------------------------------------------------------------------*/
std::string IPC::Message_Print(int task_Id)
{
  std::stringstream msgSS;
  std::string msgPrntBuf;
  mcb->messageSema->down(task_Id);
  ezQueue<IPC::Message_Type>* stdMessages = threadMailboxes.getDatumById(task_Id);
  mcb->messageSema->up();
  IPC::Message_Type* iter = NULL;


  msgSS << "  --------------------------------------------------" << std::endl
         << "  Messages for thread " << task_Id << ":" << std::endl;
  for (int i = 0; i < stdMessages->getSize(); i++) {
    iter = stdMessages->getNextElement(iter);

    msgSS << "    Message " << i+1 << ": " << std::endl;
    msgSS << "      Arrival time | size | text                | dest. Thread | source Thread"
    << std::endl;
    msgSS << "    " << std::setw(12) << iter->message_Arrival_Time
           << " " << std::setw(7) << iter->message_Size
           << " " << std::setw(20) << iter->message_Text
           << " " << std::setw(13) << iter->destination_Task_Id
           << " " << std::setw(12) << iter->source_Task_Id
           << std::endl;
  }
  msgSS << "--------------------------------------------------" << std::endl
         << std::endl<< std::endl;

  // dump contents from stringstream into string
  msgPrntBuf = msgSS.str();
  return msgPrntBuf;
}

/*-----------------------------------------------------------------
Function      : Message_Print()
Parameters    : none
Returns       : msgPrntBuf - a string containing all infos containing a
                             threads info
Details       : this function allows us to print detailed information
                about the messages of a all threads (including its content)
------------------------------------------------------------------*/
std::string IPC::Message_Print()
{
  std::stringstream msgSS;
  std::string msgPrntBuf;

  for (int i = 0; i < threadMailboxes.getSize(); i++) {
    mcb->messageSema->down(i);
    ezQueue<IPC::Message_Type>* stdMessages = threadMailboxes.getDatumById(i);
    mcb->messageSema->up();

    IPC::Message_Type* iter = NULL;

    msgSS << " \n    --------------------------------------------------" << std::endl
           << "    Messages for thread " << i << ":" << std::endl;
    for (int i = 0; i < stdMessages->getSize(); i++) {
      iter = stdMessages->getNextElement(iter);
      msgSS << "    Message " << i+1 << ": " << std::endl;
      msgSS << "      Arrival time | size |               text                | dest. Thread | source Thread"
      << std::endl;
      msgSS << "    " << std::setw(12) << iter->message_Arrival_Time //<< std::endl
             << " " << std::setw(7) << iter->message_Size //<< std::endl
             << " " << std::setw(20) << iter->message_Text //<< std::endl
             << " " << std::setw(13) << iter->destination_Task_Id //<< std::endl
             << " " << std::setw(12) << iter->source_Task_Id //<< std::endl
             << std::endl;
    }
    msgSS << "    --------------------------------------------------" << std::endl
           << std::endl<< std::endl;

    // dump contents from stringstream into string
    msgPrntBuf = msgSS.str();
  }

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
  mcb->messageSema->down(task_Id);
  ezQueue<IPC::Message_Type>* stdMessages = threadMailboxes.getDatumById(task_Id);
  ezQueue<IPC::Message_Type>* stdMessagesArchive = threadMailboxesArchive.getDatumById(task_Id);
  while(stdMessages->getSize() > 0) {
    stdMessages->deQueue();
  }

  while(stdMessagesArchive->getSize() > 0) {
    stdMessagesArchive->deQueue();
  }
  mcb->messageSema->up();
  return 1;
}

/*-----------------------------------------------------------------
Function      : stMCB
Parameters    : MCB*
Returns       :
Details       : Sets the MCB pointer within the class
------------------------------------------------------------------*/
void IPC::setMCB(MCB* mcb)
{
  this->mcb = mcb;
}


