/*===========================================================================
  Programmers   : Jakob Schuppan, Robert Davis, Yazan Faham
  File          : Ultima.cpp
  Date          : Febuary 25, 2019
  Purpose       : Driver for project ULTIMA
  ============================================================================*/

//k

//=================================
// Forward declarations
class Window;
class MCB;

//=================================
// necessary includes
#include <iostream>
#include <curses.h>     // needed for nCurses
#include <stdarg.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <ctime>
#include "UI.h"
#include "scheduler.h"
#include "sema.h"
#include "IPC.h"
#include "mem_mgr.h"
#include "string.h"
#include "UFS.h"

static const char alphabet[26] = {'a','b','c','d','e','f','g','h','i',
				  'j','k','l','m','n','o','p','q','r',
				  's','t','u','v','w','x','y','z'};
const int DUMP_SLEEP = 8;
const int MAX_WINDOWS_THREADS = 6;
const int HEADER_WIN = 0;
const int RUNNING_WINDOW = 1;
const int CONSOLE_WINDOW = 2;
const char READ = 0b10;
const char WRITE = 0b01;
const char READ_WRITE = 0b11;
void* ID = NULL;
int fileOpen1 = 0;
int fileSize1 = 200;
int fileSize2 = 500;
int tempFileHandle;
double timeElapsed;
MCB* mcb;
std::string menu = "";
std::string fileName1 = "tf";
std::string fileName2 = "tfw";
std::string fileName3 = "tfe";
std::string fileName4 = "tfa";
std::string fileName5 = "tfy";
char c;

void wrapperDump(int level,std::string menu,int threadID);
void setMCB();
void ULTIMA_menu();
void ipc_subMenu();
void scheduler_subMenu();
void FS_subMenu();
void memMgr_subMenu();
void sema_subMenu();
void schedule();
void mem_mgr_write(int offset,char *text, int tid);
void mem_mgr_alloc(int size, int tid);
void mem_mgr_free(int tid);
void ufs_createFile(int tid,char permission, std::string filename, int size);
void createTask();
void display_help();
void writeTitle();


//main
int main()
{
  initscr(); //strart curses
  refresh(); //refreshes virtual window
  // disable line buffering
  cbreak();
  // disable automatic echo of characters read by getch(), wgetch()
  noecho();
  // nodelay causes getch to be a non-blocking call.
  nodelay(stdscr, true);

  mcb = new MCB;

  setMCB();
  //Create starter threads and windows
  createTask();
  createTask();
  createTask();

  ULTIMA_menu();

  //end curses
  endwin();
}

/*-----------------------------------------------------------------
  Function      : wrapperDump(Scheduler &s, UI &userInf, int level);
  Parameters    : Scheduler,UI, integer level
  Returns       : void
  Details       : A wrapper function to encapsulate the call to dump
  ------------------------------------------------------------------*/

void wrapperDump(int level,std::string menu,int threadID)
{

  Window* writeWin = mcb->userInf->getWindowByID(CONSOLE_WINDOW);
  //mcb->writeSema->down(-1);
  writeWin->clearScreen();

  if(menu == "Scheduler")
    mcb->s->dump(writeWin, level);
  else if(menu == "Sema")
    mcb->s->dump(writeWin, level);
  else if(menu == "IPC")
    mcb->s->messageDump(writeWin, level);
  else if(menu == "MemMgr")
    mcb->mem_mgr->mem_dump(writeWin);
  else if(menu == "UFS")
  {
    if(level == 0)
      mcb->ufs->dump(writeWin);
    else if(level == 1)
      mcb->ufs->dir(writeWin);
    else
      mcb->ufs->dir(writeWin,threadID);
  }

  //mcb->writeSema->up();
}


/*-----------------------------------------------------------------
  Function      : endlessLoop()
  Parameters    :
  Returns       : void
  Details       : A wrapper function to encapsulate an endless loop
  ------------------------------------------------------------------*/
void ULTIMA_menu()
{
  menu = "ULTIMA";
  char ch;
  int tempFileHandle;
  writeTitle();
  //loop until q is pressed
  while(ch != 'q')
    {
      menu = "ULTIMA";
      schedule();

      //Get user input
      ch = getch();
      switch (ch)
		  {
		  	case 's':
		    {
		      //Enter Scheduler Sub menu
		      scheduler_subMenu();
		      menu = "ULTIMA";
		      writeTitle();
		      break;
		    }
		    case 'S':
		    {
		      //Enter Scheduler Sub menu
		      sema_subMenu();
		      menu = "ULTIMA";
		      writeTitle();
		      break;
		    }
		  	case 'i':
		    {
		      //Enter IPC Sub menu
		      ipc_subMenu();
		      menu = "ULTIMA";
		      writeTitle();
		      break;
		    }
		    case 'f':
		    {
		      //Enter Scheduler Sub menu
		      FS_subMenu();
		      menu = "ULTIMA";
		      writeTitle();
		      break;
		    }
		  	case 'm':
		    {
		      memMgr_subMenu();
		      menu = "ULTIMA";
		      writeTitle();
		      break;
		    }
		    //clear console screen
		    case  'c':
		    {
		      writeTitle();
		      break;
		    }
		    //Display help
		    case 'h':
		    {
          display_help();
          break;
		    }
		  }
    }
}


/*-----------------------------------------------------------------
  Function      :
  Parameters    :
  Returns       :
  Details       :
  ------------------------------------------------------------------*/
void scheduler_subMenu()
{
  menu = "Scheduler";
  writeTitle();
  char ch;
  //loop until q is pressed
  while(ch != 'q')
    {

      schedule();

      //Get user input
      ch = getch();

      switch (ch)
      {
      	//Add new window
		  	case 'a':
		    {
		      usleep(1000);;
		      createTask();
		      break;
		    }
				case'd':
				{
			      wrapperDump(1,menu,0);
				    break;
				}
				case 'D':
				{
				   wrapperDump(2,menu,0);
				   break;
				}
        case 'r':
				{
				    mcb->s->resume();
				    break;
				}
        case 's':
				{
				    mcb->s->stop();
				    break;
				}
				//clear console screen
        case  'c':
				{
				  writeTitle();
				  break;
				}
				//Display help
        case 'h':
				{
			    display_help();
				  break;
				}
				//Kill thread
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
				{

				  if(mcb->s->getTCBList().getDatumById((int)ch-'0') == NULL)
				    {
				      usleep(1000);
				      mcb->userInf->getWindowByID((int)ch-'0')->write_window(1,1,"ERROR DELETING THREAD");
				    }

				  else
				    mcb->s->getTCBList().getDatumById((int)ch-'0')->setState(3);
				  break;
				}
      }
    }
}

/*-----------------------------------------------------------------
  Function      :
  Parameters    :
  Returns       :
  Details       :
  ------------------------------------------------------------------*/
 void sema_subMenu()
 {
  menu = "Sema";
  writeTitle();
  char ch;

  //loop until q is pressed
  while(ch != 'q')
    {
      schedule();
      //Get user input
      ch = getch();

      switch (ch)
      {
				case 'd':
				{
				    wrapperDump(3,menu,0);
				    break;
				}
	  		//Dump(level4)
        case 'D':
				{
			      wrapperDump(4,menu,0);
			      break;
				}
        case 'r':
				{
				    mcb->s->resume();
				    break;
				}
        case 's':
				{
				    mcb->s->stop();
				    break;
				}
				//clear console screen
        case  'c':
				{
				  writeTitle();
				  break;
				}
				//Display help
        case 'h':
				{
				  display_help();
				  break;
				}
        case 'f':
				{
				  mcb->writeSema->down(0);
					mcb->writeSema->down(1);
					mcb->writeSema->down(2);
				  break;
				}
				case 'F':
				{
					mcb->writeSema->up();
					mcb->writeSema->up();
					mcb->writeSema->up();
 				  break;
				}
      }
    }
 }

/*-----------------------------------------------------------------
  Function      :
  Parameters    :
  Returns       :
  Details       :
  ------------------------------------------------------------------*/
 void ipc_subMenu()
 {
  menu = "IPC";
  writeTitle();
  char ch;
	std::string message;
	char buff[255];
	char* msg;
  //loop until q is pressed
  while(ch != 'q')
  {
      schedule();
      //Get user input
      ch = getch();

      switch (ch)
      {
				case'd':
				{
				    wrapperDump(1,menu,0);
				    break;
				}
         case 'r':
				{
				    mcb->s->resume();
				    break;
				}
			   case 's':
				{
				    mcb->s->stop();
				    break;
				}
				//clear console screen
        case  'c':
				{
				  writeTitle();
				  break;
				}
				//Display help
        case 'h':
				{
				  display_help();
				  break;
				}
				case 'm':
				{
					int task1 = rand()%3;
					int task2 = rand()%3;
				  mcb->ipc->Message_Send(task1, task2, "Can you hear me now?");
					sprintf(buff,  "  Message sent to task %d\n",task2);
					mcb->s->getThreadInfo().getDatumById(task1)->getThreadWin()->write_window(buff);
				  break;
				}
				case 'M':
				{
					int task = rand()%3;
					mcb->ipc->Message_Receive(task, message);
					msg = strdup(message.c_str());
					sprintf(buff,  "  %s\n",msg);
					mcb->s->getThreadInfo().getDatumById(task)->getThreadWin()->write_window(buff);
				  break;
				}
				case 'R':
				{
					int task = rand()%3;
					mcb->ipc->Message_DeleteAll(task);
					mcb->s->getThreadInfo().getDatumById(task)->getThreadWin()->write_window("  All Messages deleted\n");
				  break;
				}


      }
    }
 }

/*-----------------------------------------------------------------
  Function      :
  Parameters    :
  Returns       :
  Details       :
  ------------------------------------------------------------------*/
 void memMgr_subMenu()
 {
   menu = "MemMgr";
   writeTitle();
  char ch;

  //loop until q is pressed
  while(ch != 'q')
    {
      schedule();
      //Get user input
      ch = getch();

      switch (ch)
      {
				case'd':
				{
				    wrapperDump(0,menu,0);
				    break;
				}
        case 'r':
				{
				    mcb->s->resume();
				    break;
				}
        case 's':
				{
				    mcb->s->stop();
				    break;
				}
				//clear console screen
        case  'c':
				{
				  writeTitle();
				  break;
				}
				//Display help
        case 'h':
				{
				  display_help();
				  break;
				}

        case 'w':
				{
				  char* c = "This space is not mine";
				  // mem_mgr_write(int offset,char *text, int tid)
				  mem_mgr_write(90,c,rand()%3);
				}
        case 'W':
				{
				  char* c = "This space is mine";
				  // mem_mgr_write(int offset,char *text, int tid)
				  mem_mgr_write(1,c,rand()%3);

				  break;
				}
        case 'a':
				{
				  // void mem_mgr_alloc(int size, int tid)
				  mem_mgr_alloc(300, rand()%3);
				  break;
				}
        case 'f':
				{
				  mem_mgr_free(rand()%3);
				  break;
				}
      }
    }
 }

/*-----------------------------------------------------------------
  Function      :
  Parameters    :
  Returns       :
  Details       :
  ------------------------------------------------------------------*/
 void FS_subMenu()
 {
   menu = "UFS";
   writeTitle();
  char ch;

  //loop until q is pressed
  while(ch != 'q')
    {
      schedule();
      //Get user input
      ch = getch();

      switch (ch)
      {
        case 's':
	{
	    mcb->s->stop();
	    break;
	}
	//clear console screen
        case  'c':
	{
	  writeTitle();
	  break;
	}
	//Display help
        case 'h':
	{
	  display_help();
	  break;
	}
        case 'a':
	{
	  // void ufs_createFile(int tid,char permission, string filename, int size)
	  char status = 0b1111;
	  ufs_createFile(rand()%3 ,status,fileName1,fileSize1);
	  break;
	}
        case 'A':
	{
	  //Create multiple files to fill memory
	  //void ufs_createFile(int tid,char permission, string filename, int size)
	  char status = 0b1100;
	  ufs_createFile(0 ,status,fileName2,fileSize2);
	  ufs_createFile(1 ,status,fileName3,fileSize2);
	  ufs_createFile(2 ,status,fileName4,fileSize2);
	  ufs_createFile(0 ,status,fileName5,fileSize2);
	  break;
	}
        case 'd':
	{
	  //Test UFS Dump
	  wrapperDump(0,menu,0);
	  break;
	}
        case 'D':
	{
	  //Test UFS Dir dump
	  wrapperDump(1,menu,0);
	  break;
	}
        case '0':
        case '1':
        case '2':
	{
	  //Test UFS  Dir dump
	  wrapperDump(2,menu,0);
	  break;
	}
        case 'w':
	{
	  //Test UFS Write out of bounds
	  mcb->ufs->writeChar(0,fileOpen1, alphabet[rand()%26],700);
	  break;
	}
        case 'W':
	{
	  //Test UFS Write in bounds
	  mcb->ufs->writeChar(0,fileOpen1, alphabet[rand()%26],rand() % fileSize1 );
	  break;
	}
        case 'o':
	{
	  //Test UFS OpenFile for Write
	  fileOpen1 =mcb->ufs->openFile(0,tempFileHandle,fileName1 , WRITE);
	  break;
	}
        case 'O':
	{
	  //Test UFS OpenFile for Read
	  fileOpen1 =mcb->ufs->openFile(0, tempFileHandle, fileName1 , READ);
	  break;
	}
        case 'v':
	{
	  //Test UFS Read out of bounds
	  mcb->ufs->readChar(0,fileOpen1, c,700);
	  break;
	}
        case 'V':
	{
	  //Test UFS Read in bounds
	  mcb->ufs->readChar(0,fileOpen1,c,rand() % fileSize1 );
	  break;
	}
        case 'g':
	{
	  //Test UFS Delete File
	  mcb->ufs->deleteFile(rand()%3,fileName1);
	  break;
	}
        case 'G':
	{
	  //Test UFS Multiple file create
	  mcb->ufs->deleteFile(0,fileName2);
	  mcb->ufs->deleteFile(1,fileName3);
	  mcb->ufs->deleteFile(2,fileName4);
	  mcb->ufs->deleteFile(0,fileName5);
	  break;
	}
      }
    }
 }

/*-----------------------------------------------------------------
  Function      :
  Parameters    :
  Returns       :
  Details       :
  ------------------------------------------------------------------*/
void display_help()
{
  Window* writeWin = mcb->userInf->getWindowByID(CONSOLE_WINDOW);

  //Protect write window
  mcb->writeSema->down(-1);
  writeWin->clearScreen();
  if(menu == "ULTIMA")
  {
    writeWin->write_window("\n   WELCOME TO THE ULTIMA HELP MENU\n\n");
    writeWin->write_window("   s=...Scheduler Menu...\n");
    writeWin->write_window("   S=...Semaphore Menu...\n");
    writeWin->write_window("   i=...IPC Menu...\n");
    writeWin->write_window("   f=...UFS Menu...\n");
    writeWin->write_window("   m=...mem_mgr Menu...\n");
  }
  else if(menu == "Scheduler")
  {
    writeWin->write_window("\n   WELCOME TO THE SCHEDULER HELP MENU\n\n");
    writeWin->write_window("   c=...clear screen...\n");
    writeWin->write_window("   q=...Quit...\n");
    writeWin->write_window("   s=...stop...\n");
    writeWin->write_window("   r=...Restart...\n");
    writeWin->write_window("   a=...Add a thread...\n");
    writeWin->write_window("   0=...Kill task 0...\n");
    writeWin->write_window("   1=...Kill task 1...\n");
    writeWin->write_window("   2=...Kill task 2...\n");
    writeWin->write_window("   3=...Kill task 3...\n");
    writeWin->write_window("   4=...Kill task 4...\n");
    writeWin->write_window("   5=...Kill task 5...\n");
    writeWin->write_window("   d=...Dump level 1..\n");
    writeWin->write_window("   d=...Dump level 2...\n");
  }
  else if(menu == "Sema")
  {
    writeWin->write_window("\n   WELCOME TO THE SEMAPHORE HELP MENU\n\n");
    writeWin->write_window("   c=...clear screen...\n");
    writeWin->write_window("   q=...Quit...\n");
    writeWin->write_window("   s=...stop...\n");
    writeWin->write_window("   r=...Restart...\n");
    writeWin->write_window("   d=...Dump Level 1...\n");
    writeWin->write_window("   D=...Dump Level 2...\n");
    //This still needs implemented
    writeWin->write_window("   f=...Force DeadLock...\n");
    writeWin->write_window("   F=...Free DeadLock...\n");
  }
  else if(menu == "IPC")
  {
    writeWin->write_window("\n   WELCOME TO THE IPC HELP MENU\n\n");
    writeWin->write_window("   c=...clear screen...\n");
    writeWin->write_window("   q=...Quit...\n");
    writeWin->write_window("   s=...stop...\n");
    writeWin->write_window("   r=...Restart...\n");
    writeWin->write_window("   d=...Dump...\n");
    writeWin->write_window("   m=...Send message to random target...\n");
    writeWin->write_window("   M=...Receive message random target...\n");
    writeWin->write_window("   R=...Delete message random target...\n");
  }
  else if(menu == "MemMgr")
  {
    writeWin->write_window("\n   WELCOME TO THE MEM_MGR HELP MENU\n\n");
    writeWin->write_window("   c=...clear screen...\n");
    writeWin->write_window("   q=...Quit...\n");
    writeWin->write_window("   s=...stop...\n");
    writeWin->write_window("   r=...Restart...\n");
    writeWin->write_window("   d=...Dump...\n");
    writeWin->write_window("   w=...Write out of bounds...\n");
    writeWin->write_window("   W=...Write in bound...\n");
    writeWin->write_window("   a=...Allocate memory...\n");
    writeWin->write_window("   f=...Free memory...\n");
  }
  else if(menu == "UFS")
  {
    writeWin->write_window("\n   WELCOME TO THE UFS HELP MENU\n\n");
    writeWin->write_window("   c=...clear screen...\n");
    writeWin->write_window("   q=...Quit...\n");
    writeWin->write_window("   s=...stop...\n");
    writeWin->write_window("   r=...Restart...\n");
    writeWin->write_window("   d=...Dump...\n");
    writeWin->write_window("   D=...Directory...\n");
    writeWin->write_window("   0=...Thread 0 Directory...\n");
    writeWin->write_window("   1=...Thread 1 Directory...\n");
    writeWin->write_window("   2=...Thread 2 Directory...\n");
    writeWin->write_window("   a=...Create File size 200...\n");
    writeWin->write_window("   A=...Create 4 Files size 400...\n");
    writeWin->write_window("   w=...Write out of bounds...\n");
    writeWin->write_window("   W=...Write in bound...\n");
    writeWin->write_window("   o=...Open file for read...\n");
    writeWin->write_window("   O=...Open file for Write...\n");
    writeWin->write_window("   v=...Read out of bounds...\n");
    writeWin->write_window("   V=...Read in bound...\n");
    writeWin->write_window("   g=...Delete file...\n");
    writeWin->write_window("   g=...Delete Multiple files...\n");

  }

//Free Window write
mcb->writeSema->up();
}
 /*==================================================


                OTHER HELPER FUNCTIONS
		USED TO CLEAN UP CODE


=====================================================*/
void schedule()
{
  std::clock_t garbageTimerStart = std::clock();

  ID = mcb->s->running(ID);

  // run garbage_collect() every 30 seconds
  timeElapsed = ((std::clock() - garbageTimerStart) / (double)CLOCKS_PER_SEC);

  if (timeElapsed > 30) {
    mcb->s->garbage_collect();
    garbageTimerStart = std::clock();
  }
}


void mem_mgr_write(int offset,char *text, int tid)
{
  mcb->mem_mgr->mem_write(mcb->s->getThreadInfo().getDatumById(tid)->getMemHandle(), offset, strlen(text), text, tid);
}

void mem_mgr_alloc(int size, int tid)
{
  mcb->s->getThreadInfo().getDatumById(tid)->mem_size = size;
  mcb->s->getThreadInfo().getDatumById(tid)-> mem_handle = mcb->mem_mgr->mem_alloc(mcb->s->getThreadInfo().getDatumById(tid)->mem_size, tid);
}

void mem_mgr_free(int tid)
{
  mcb->mem_mgr->mem_free(mcb->s->getThreadInfo().getDatumById(tid)->getMemHandle(),tid);
}

void ufs_createFile(int tid, char permission, std::string filename, int size)
{

  tempFileHandle = mcb->ufs->createFile(tid,filename,size,permission);
  mcb->s->getThreadInfo().getDatumById(tid)->fileHandle.addToFront(tempFileHandle,tempFileHandle);
}


void setMCB()
{
  mcb->s->setMCB(mcb);
  mcb->ipc->setMCB(mcb);
  mcb->writeSema->setMCB(mcb);
  mcb->messageSema->setMCB(mcb);
  mcb->metaFileSema->setMCB(mcb);
  mcb->dataFileSema->setMCB(mcb);
  mcb->UFSLinkSema->setMCB(mcb);
  mcb->mem_mgr->setMCB(mcb);
  mcb->ufs->setMCB(mcb);
}

void createTask()
{
  //First create window
  mcb->userInf->addNewWindow();
  //Create Thread
  mcb->s->create_task(mcb->userInf->getWindowCreated(),mcb->userInf->getWindowByID(HEADER_WIN),mcb->userInf->getWindowByID(RUNNING_WINDOW));
}

void writeTitle()
{
  Window* writeWin = mcb->userInf->getWindowByID(CONSOLE_WINDOW);
  mcb->writeSema->down(-1);
  writeWin->clearScreen();

  if(menu == "ULTIMA")
    writeWin->write_window("\n   WELCOME TO THE ULTIMA SUB MENU\n   press h for help\n");
  else if(menu == "Scheduler")
    writeWin->write_window("\n   WELCOME TO THE SCHEDULER SUB MENU\n   press h for help\n");
  else if(menu == "Sema")
    writeWin->write_window("\n   WELCOME TO THE SEMAPHORE SUB MENU\n   press h for help\n");
  else if(menu == "IPC")
    writeWin->write_window("\n   WELCOME TO THE IPC SUB MENU\n   press h for help\n");
  else if(menu == "MemMgr")
    writeWin->write_window("\n   WELCOME TO THE MEM_MGR SUB MENU\n   press h for help\n");
  else if(menu == "UFS")
    writeWin->write_window("\n   WELCOME TO THE UFS SUB MENU\n   press h for help\n");

  mcb->writeSema->up();
}
