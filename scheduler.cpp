// Authors: Jakob Schuppan, Robert Davis
// lastUpdated: 02-21-2019
#include <pthread.h>
#include <list>
#include <assert.h>
#include "scheduler.h"
#include "sema.h"
#include <fstream>
#include <mutex>
#include <cmath>
#include <unistd.h> //sleep
#include <random>

std::mutex testMtx;
bool THREAD_SUSPENDED = false;
std::mutex suspend_mtx;

void* perform_simple_output(void* arguments);

int randomGenerator(int min, int max) {
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);

    return dist(rng);
}

Scheduler::Scheduler()
{
  processCount = 0;
  tempCounter = 0;
}
//TCB State: 0 running, 1 ready, 2 blocked, 3 dead
//void Scheduler::create_task(functionPtr, threadArg, threadName)
void Scheduler::create_task(Window* threadWin, Window* headerWin, Window* consoleWin) {

  if(processCount > 5 )
    return;
  int createResult;

  threadInfo[processCount].thread_win = threadWin;
  threadInfo[processCount].head_win = headerWin;
  threadInfo[processCount].console_win = consoleWin;
  threadInfo[processCount].thread_no = processCount;
  TCB tcbTemp;
  tcbTemp.setThreadID(processCount);
  tcbTemp.setState(1);
  tcbTemp.setThreadData(&threadInfo[processCount]);
  TCBList.addToFront(tcbTemp, processCount);


  // create a thread
  createResult = pthread_create(&pthreads[processCount], NULL, perform_simple_output, &threadInfo[processCount]);
  //threadInfo[0].thread_win->display_help();
  // wait for termination and check if we ran into issues
  // createResult = pthread_join(pthreads[processCount], NULL);
  //assert(!createResult);
  char buff[256];
  sprintf(buff, " Thread-%d created.\n",threadInfo[processCount].thread_no);
  threadInfo[processCount].thread_win->write_window("\n");
  threadInfo[processCount].head_win->write_window(processCount + 1, 1,buff);
  //dump(5);
  processCount++;

}
void Scheduler::yield()
{
  int result;
  //result = callingThread.pthread_yield();

  if (result != 0) {
    perror("Fatal error: pThread failed to yield!");
    exit(0);
  }
}

void Scheduler::dump(WINDOW* targetWin, int level)
{
  // suspend threads and wait to make sure
  // everything is synced
  THREAD_SUSPENDED = true;
  sleep(1);


  int bs = 0;
  debugDump.open("sched_dump.txt", std:: ofstream::app);
  debugDump << "\n DUMP START \n";
  // threadStop
  while((TCBList.getDatumById(bs)) != NULL) {
    debugDump << "---- \n ThreadID: " << bs << std::endl
               << "   " << "Status: "
               << TCBList.getDatumById(bs)->getThreadData()->getState()
               << std::endl << "---- \n";
    bs++;
  }
  debugDump  << "\n DUMP END";

  THREAD_SUSPENDED = false;
  // debugFile2 << "thread# = " << &threadInfo[0].thread_no<<"\n";
  //debugFile2 << "State:  = " << TCBList.getDatumById(processCount)->getState()<<"\n";
  debugDump.close();
}

void* perform_simple_output(void* arguments)
{
  int tempCounter = 0;
  char buff[256];
  Scheduler :: thread_data* td = (Scheduler::thread_data*) arguments;
  std::ofstream threadDebug;
  threadDebug.open("threadStatus.txt");

  while((1) && (td->state != 4))
  {
    // check for suspend called by dump
    if (THREAD_SUSPENDED) {
      // we use try_lock to prevent deadlock
      if(!suspend_mtx.try_lock()) {
        suspend_mtx.lock();
      };
    }
    // if thread is no longer suspended keep going
    else {
      suspend_mtx.unlock();
    }

    //sleep(1);
    // testMtx.lock();
      int runID = randomGenerator(10,100);
      // threadDebug << "Thread: " << td->thread_no << "[" << runID << "]"<< std::endl;
      // threadDebug << "(" << td->thread_no << ":" << runID << ")"
      //             << "current status: " << td->state << std::endl;
    // testMtx.unlock();
    if(td->state == 0)
    {
        // testMtx.lock();
        threadDebug << "   (" << td->thread_no << ":" << runID  << ")"<< "RUNNING" << std::endl;
        // testMtx.unlock();
        tempCounter++;
        sprintf(buff, "  Task-%d running #%d\n",td->thread_no,tempCounter);
        // mute.lock();
        //ns.down(threadID);
        td->thread_win->write_window(buff);
        //ns.up();
        //mute.unlock();

        sprintf(buff, "  Thread-%d currently running.\n",td->thread_no);
        // mute.lock();
        //ns.down(threadID);
        td->console_win->write_window(buff);
        //ns.up();
        // mute.unlock();

        // setState() --> replace with YIELD()
        td->state = 1;
    }
    else {
      // testMtx.lock();
      threadDebug << "   (" << td->thread_no << ":" << runID  << ")"<< "YIELDING" << std::endl;
      // testMtx.unlock();
      // pthread_yield();
    }
  }
  threadDebug.close();
}

// int Scheduler :: running(int ID) {
//
// }

int Scheduler:: running(int ID)
{

  std::ofstream runDebug;
  runDebug.open("debug_thread.txt", std:: ofstream::app);
  // debugFile2 << "running# = " << ID <<"\n";
  runDebug << ID << ": " << TCBList.getDatumById(ID)->getThreadData()->getState() << std::endl;

    // if thread not running
    if(TCBList.getDatumById(ID)->getThreadData()->getState() != 0)
    {
      // if the next element is not null
      // set it to running and return following element
      if((TCBList.getDatumById(ID+1)) != NULL)
      {
        TCBList.getDatumById(ID+1)->getThreadData()->setState(0);
        return ID+1;
      }
      // otherwise set first thread to running
      else
      {
        TCBList.getDatumById(0)->getThreadData()->setState(0);
        return 0;
      }
    }
    runDebug.close();
    return ID;
}
