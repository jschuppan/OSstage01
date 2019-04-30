#include "mem_mgr.h"
#include <iostream>
#include "window.h"
#include <assert.h>
#include <string.h>
#include "scheduler.h"
#include "IPC.h"
#include "sema.h"
#include "UI.h"
#include <unistd.h> //sleep

//Defualt Constructor
Mem_Mgr::Mem_Mgr()
{
  unsigned int size = 1024;
  unsigned int default_init_val = '.';
  char buff[255];

  default_mem_fill = default_init_val;
  freed_mem_fill = '#';
  capacity = available = size;
  used = 0;
  next_handle = 0;

  //Create new mem_seg to push to List
  mem_seg ms;
  ms.handle = next_handle++;
  ms.owner_tid = -1;
  ms.start = 0;
  ms.end = capacity - 1;
  ms.size = capacity;
  ms.read_cursor = 0;
  ms.write_cursor = 0;
  ms.free = true;

  //add ms to list
  segments.addToFront(ms, ms.handle);

  //Initalize MEMORY
  memory = new unsigned char[capacity];
  for (unsigned int i = 0; i < capacity; i++)
    memory[i] = default_mem_fill;
}

//Constructor
Mem_Mgr::Mem_Mgr(unsigned int size, unsigned char default_init_val) {
  char buff[255];

  default_mem_fill = default_init_val;
  freed_mem_fill = '#';
  capacity = available = size;
  used = 0;
  next_handle = 0;

  //Create mem_seg to push to list
  mem_seg ms;
  ms.handle = next_handle++;
  ms.owner_tid = -1;
  ms.start = 0;
  ms.end = capacity - 1;
  ms.size = capacity;
  ms.read_cursor = 0;
  ms.write_cursor = 0;
  ms.free = true;

  //push ms to list
  segments.addToFront(ms, ms.handle);

  //Initalize memory
  memory = new unsigned char[capacity];
  for (unsigned int i = 0; i < capacity; i++)
    memory[i] = default_mem_fill;
}

//destructer
Mem_Mgr::~Mem_Mgr() {
  delete memory;
  segments.deleteList();
}

/*-----------------------------------------------------------------
Function      : mem_alloc();
Parameters    : int handle, int size, int tid
Returns       : -1 if not enough memory, returns handle
Details       : allocates memory space and returns the handle
------------------------------------------------------------------*/
int Mem_Mgr::mem_alloc(unsigned int size, int tid) {
  char buff[255];
  //Not enough Memory
  if (size > available) {
    //mcb->writeSema->down(tid);
    sprintf(buff, "\n  mem_alloc() : not enough memory\n  available size: %d \n  Requested Size %d\n",available,size);
    mcb->s->getThreadInfo().getDatumById(tid)->getThreadWin()->write_window(buff);
    //mcb->writeSema->up();
    return -1;  //error: not enough memory
  }

  // allocate new memory at end
  // traverse segments looking where the last element ends before the
  // free space starts
  mem_seg* sPtr;
  sPtr = segments.getNextElementUntilEnd(NULL);
  int countPos = 0;
  while(sPtr->size < size || !sPtr->free)
  {
    //exexutes if node is null
    if(!(sPtr = segments.getNextElementUntilEnd(sPtr)))
    {
      //Node not found but available is > size
      if(available >= size)
      {
          burp();
          sPtr = segments.getNextElementUntilEnd(NULL);
          countPos = 0;
      }
      else
      {
        //mcb->writeSema->down(tid);
        sprintf(buff, "\n  Not Enough Consecutive Memory\n");
        mcb->s->getThreadInfo().getDatumById(tid)->getThreadWin()->write_window(buff);
        //mcb->writeSema->up();
        return -1;
      }
    }
    countPos++;
  }

  //Create and initalize mem_seg
  mem_seg tmpDatum;
  tmpDatum.handle = next_handle++;
  tmpDatum.owner_tid = tid;
  tmpDatum.size = size;
  tmpDatum.write_cursor = tmpDatum.read_cursor = sPtr->start;
  tmpDatum.free = false;

  tmpDatum.start = sPtr->start;
  tmpDatum.end = sPtr->start + size -1;
  //Insert mem_seg
  segments.insertNode(tmpDatum, tmpDatum.handle, countPos);

  sPtr->start = tmpDatum.end + 1;
  sPtr->size = sPtr->size - size;
  tmpDatum.write_cursor = tmpDatum.read_cursor = sPtr->start;

  //Write to window on success
  //mcb->writeSema->down(tid);
  sprintf(buff, "\n  Memory Allocated \n");
  mcb->s->getThreadInfo().getDatumById(tid)->getThreadWin()->write_window(buff);
  //mcb->writeSema->up();



  // adjust variables
  available -= size;
  used += size;

  // return our handle
  return tmpDatum.handle;
}

/*-----------------------------------------------------------------
Function      : mem_free();
Parameters    : int handle,int tid
Returns       : -1 if no access, 1 if success
Details       : Sets free variable to true, So memory can be used by another
------------------------------------------------------------------*/
int Mem_Mgr::mem_free(int handle, int tid) {
  //Get mem_seg at give handle
  mem_seg *ms_ptr = segments.getDatumById(handle);
  char buff[255];

  //If mem_seg doesnt exist
  if (ms_ptr == NULL) {
    //mcb->writeSema->down(tid);
    sprintf(buff,"\n   mem_free() : Item segment doesn't exist\n");
    mcb->s->getThreadInfo().getDatumById(tid)->getThreadWin()->write_window(buff);
    //mcb->writeSema->up();
    return -1;  //error: not found
  }

  //If tid Does not have permission
  if (tid != ms_ptr->owner_tid) {
    //mcb->writeSema->down(tid);
    sprintf(buff,"\n   mem_free() : access denied\n");
    mcb->s->getThreadInfo().getDatumById(tid)->getThreadWin()->write_window(buff);
    //mcb->writeSema->up();
    return -1;  //error: access denied
  }

  //Else Free up the memory
  unsigned int start = ms_ptr->start;
  unsigned int end = ms_ptr->end;
  for (unsigned int i = start; i <= end; i++)
    memory[i] = freed_mem_fill;

  ms_ptr->write_cursor = ms_ptr->read_cursor = start;

  ms_ptr->free = true;
  ms_ptr->owner_tid = -1;

  available += ms_ptr->size;
  used -= ms_ptr->size;

  mem_coalesce();

  return 1;  //success
}


/*-----------------------------------------------------------------
Function      : mem_read();
Parameters    : int handle, char c, int tid
Returns       : -1 if no access, 1 if success
Details       : Reads from a given memory space
------------------------------------------------------------------*/
int Mem_Mgr::mem_read(int handle, unsigned char *c, int tid)
{
  //get mem_seg by handle
  mem_seg *ms_ptr = segments.getDatumById(handle);

  //mem_seg does not exist
  if (ms_ptr == NULL)
  {
    writeToThreadWindow(tid, "  mem_read() : Item segment doesn't exist\n");
    return -1;  //error: not found
  }

  //tid does not have permission
  if (tid != ms_ptr->owner_tid)
  {
    writeToThreadWindow(tid, "  mem_read() : access denied\n");
    return -1;  //error: access denied
  }

  //Else read the Data
  *c = memory[ ms_ptr->read_cursor ];

  ms_ptr->read_cursor ++;
  //if read_cursor goes past write_cursor, wrap around to start
  if (ms_ptr->read_cursor >= ms_ptr->write_cursor)
  {
    ms_ptr->read_cursor = ms_ptr->start;
  }

  return 1;  //success
}

/*-----------------------------------------------------------------
Function      : mem_read();
Parameters    : int handle, int offset, int text_size, char* text, in tid
Returns       : -1 if no access, 1 if success
Details       : reads from a given memory space
------------------------------------------------------------------*/
/*********************  needs attention  ***********************/
int Mem_Mgr::mem_read(int handle, unsigned int offset, unsigned int text_size, unsigned char *text, int tid) {
  //get mem_seg by handle
  mem_seg *ms_ptr = segments.getDatumById(handle);

  //mem_seg does not exist
  if (ms_ptr == NULL) {
    writeToThreadWindow(tid, "  mem_read() : Item segment doesn't exist\n");
    return -1;  //error: not found
  }

  //tid does not have permission
  if (tid != ms_ptr->owner_tid || offset > ms_ptr->end) {
    writeToThreadWindow(tid, "  mem_read() : access denied\n");
    return -1;  //error: access denied
  }

  //else read from memory
  // set read cursor
  ms_ptr->read_cursor = ms_ptr->start;

  // return all characters of what is in the memory space
  int i = 0;
  while (ms_ptr->read_cursor < ms_ptr->end)
  {
    text[i] = memory[ms_ptr->read_cursor];
    ms_ptr->read_cursor++;
    i++;
  }
  // null terminate
  text[ms_ptr->end] = '\0';
  // reset cursor
  ms_ptr->read_cursor = ms_ptr->start;


  return 1;  //success
}

/*-----------------------------------------------------------------
Function      : mem_write();
Parameters    : int handle, char c, int tid
Returns       : -1 if no access, 1 if success
Details       : Writes to a given memory space
------------------------------------------------------------------*/
int Mem_Mgr::mem_write(int handle, unsigned char c, int tid) {
  //get mem_seg by handle
  mem_seg *ms_ptr = segments.getDatumById(handle);

  //mem_seg does not exist
  if (ms_ptr == NULL)
  {
    writeToThreadWindow(tid, "  mem_write() : Item segment doesn't exist\n");
    return -1;  //error: not found
  }

  //tid does not perssion
  if (tid != ms_ptr->owner_tid)
  {
    writeToThreadWindow(tid, "  mem_write() : access denied\n");
    return -1;  //error: access denied
  }

  //if writecursor is at end of memory
  if (ms_ptr->write_cursor > ms_ptr->end)
  {
    writeToThreadWindow(tid, "  mem_write() : segment is full\n");
    return -1;  //error: segment is full
  }

//just read
  memory[ ms_ptr->write_cursor ] = c;

  ms_ptr->write_cursor++;

  return 1;  //success
}

/*-----------------------------------------------------------------
Function      : mem_write();
Parameters    : int handle, int offset, int text_size, char* text, in tid
Returns       : -1 if access denied, 1 with success
Details       : Writes to a gien memory space
------------------------------------------------------------------*/
/*********************  needs attention  ***********************/
int Mem_Mgr::mem_write(int handle, unsigned int offset, unsigned int text_size, char *text, int tid)
{
  //get mem_seg
  mem_seg *ms_ptr = segments.getDatumById(handle);

  //mem_seg does not exist
  if (ms_ptr == NULL)
  {
    writeToThreadWindow(tid, "  mem_write() : Item segment doesn't exist\n");
    return -1;  //error: not found
  }

  char buff[255];
  //tid does not have permission
  if (tid != ms_ptr->owner_tid)
  {
    //mcb->writeSema->down(tid);
    sprintf(buff, "\n  Owner permission denied\n");
    mcb->s->getThreadInfo().getDatumById(tid)->getThreadWin()->write_window(buff);
    //mcb->writeSema->up();    return -1;  //error: access denied
  }

  //if not enough space to write string
  if (ms_ptr->write_cursor+text_size+offset >ms_ptr->end)
  {
    //mcb->writeSema->down(tid);
    sprintf(buff,"\n  Segmentation Fault\n");
    mcb->s->getThreadInfo().getDatumById(tid)->getThreadWin()->write_window(buff);
    //mcb->writeSema->up();
    return -1;  //error: segment is full
  }

  //just write
  int i = 0;
  while (i <= text_size && ms_ptr->write_cursor < ms_ptr->end)
  {
    memory[ms_ptr->write_cursor+offset] =text[i];
    ms_ptr->write_cursor++;
    i++;
  }

  ms_ptr->write_cursor = ms_ptr->start;


  return 1;  //success
}

/*-----------------------------------------------------------------
Function      : setMCB();
Parameters    :
Returns       : void
Details       : sets MCB pointer
------------------------------------------------------------------*/
void Mem_Mgr::setMCB(MCB *mcb)
{
  this->mcb = mcb;
}

/*-----------------------------------------------------------------
Function      : left();
Parameters    :
Returns       : returns available memory
Details       :  "
------------------------------------------------------------------*/
int Mem_Mgr::mem_left()
{
  return available;
}

/*-----------------------------------------------------------------
Function      : mem_largest();
Parameters    :
Returns       : returns largest  memory block
Details       :
------------------------------------------------------------------*/
int Mem_Mgr::mem_largest() {
  unsigned int largest = 0;

  mem_seg *ms_ptr = segments.getNextElementUntilEnd(NULL);  //get first element
  while (ms_ptr = segments.getNextElementUntilEnd(ms_ptr)) {
    if (ms_ptr->free && ms_ptr->size > largest)
      largest = ms_ptr->size;
    ms_ptr = segments.getNextElementUntilEnd(ms_ptr);
  }

  return largest;
}

/*-----------------------------------------------------------------
Function      : mem_smallest();
Parameters    :
Returns       : smallest capacity
Details       : finds the smallest capacity and returns it
------------------------------------------------------------------*/
int Mem_Mgr::mem_smallest() {
  unsigned int smallest = capacity;

  mem_seg *ms_ptr = segments.getNextElementUntilEnd(NULL);  //get first element
  while (ms_ptr = segments.getNextElementUntilEnd(ms_ptr))
  {
    if (ms_ptr->free && ms_ptr->size < smallest)
      smallest = ms_ptr->size;

    ms_ptr = segments.getNextElementUntilEnd(ms_ptr);
  }

  return smallest;
}

/*-----------------------------------------------------------------
Function      : mem_coalesce();
Parameters    :
Returns       : returns 1 on success
Details       : combines holes in the memory
------------------------------------------------------------------*/
int Mem_Mgr::mem_coalesce()
{
  //get first element
  mem_seg *prev_ms_ptr = segments.getNextElementUntilEnd(NULL);
  mem_seg *ms_ptr = segments.getNextElementUntilEnd(prev_ms_ptr);
  char buff[255];

  while (ms_ptr) {
    if (prev_ms_ptr->free && ms_ptr->free) {  //adjacent free segments

      //fill prev_mem_ptr with '.'
      if (memory[ prev_ms_ptr->start ] != default_mem_fill)
      {
        int start = prev_ms_ptr->start;
        int end = prev_ms_ptr->end;
        for (int i = start; i <= end; i++)
          memory[i] = default_mem_fill;
      }

      //fill mem_ptr with '.'
      if (memory[ ms_ptr->start ] != default_mem_fill)
      {
        int start = ms_ptr->start;
        int end = ms_ptr->end;
        for (int i = start; i <= end; i++)
          memory[i] = default_mem_fill;
      }


      //combine
      ms_ptr->start = prev_ms_ptr->start;
      ms_ptr->size += prev_ms_ptr->size;

      //remove prev node
      segments.removeNodeByElement(prev_ms_ptr->handle);
      ms_ptr->write_cursor = ms_ptr->read_cursor = ms_ptr->start;

    }

    //Iterate through list
    prev_ms_ptr = ms_ptr;
    ms_ptr = segments.getNextElementUntilEnd(ms_ptr);
  }

  return 1;  //success
}


/*-----------------------------------------------------------------
Function      : mem_dump();
Parameters    :
Returns       : void
Details       : Outputs the memory to a Window
------------------------------------------------------------------*/
void Mem_Mgr::mem_dump(Window* Win)
{
   mem_seg* myT = NULL;
   char mBuff[16384];
   std::string tempString;
   char* chr;

   //Suspend scheduler
   mcb->s->SCHEDULER_SUSPENDED = true;

   tempString = "\n\n  Status | Memory Handle | Starting Location | Ending Location | Size | task-ID\n";
   chr = strdup(tempString.c_str());
   sprintf(mBuff, "%s",chr);
   Win->write_window(mBuff);
   int count = 0;
   char cBuff[1025];

   //Display table for memory
   while ((myT = segments.getNextElementUntilEnd(myT)))
   {
     if(myT->free)
     {
       sprintf(mBuff, "  Free");
     }
     else
       sprintf(mBuff, "  Used");

     sprintf(mBuff + strlen(mBuff),"\t   %d\t\t", myT->handle);
     sprintf(mBuff + strlen(mBuff),"   %d\t\t\t", myT->start);
     sprintf(mBuff + strlen(mBuff),"%d\t\t", myT->end);
     sprintf(mBuff + strlen(mBuff)," %d\t", myT->size);
     sprintf(mBuff + strlen(mBuff),"%d\n", myT->owner_tid);

     Win->write_window(mBuff);
   }

   sprintf(cBuff, "");

   //Output memory segment
   while(count < capacity)
   {
     if(count % 32 == 0)
          sprintf(cBuff+strlen(cBuff), "\n");
     else if(count == 0)
     {
          // WHYYYYYYYY???????????????????????
          // fix me
          continue;
          count++;
      }
     sprintf(cBuff+strlen(cBuff), "  %c",memory[count]);
     count++;
   }
   sprintf(cBuff+strlen(cBuff), "\n");

   //write to window
   Win->write_window(cBuff);

   // deallocate memory for chr
   free(chr);

   mcb->s->SCHEDULER_SUSPENDED = false;
}


/*-----------------------------------------------------------------
Function      : burp();
Parameters    :
Returns       : int
Details       : Moves the the holes to the end of the memory
------------------------------------------------------------------*/
int Mem_Mgr::burp() {

  int shift_start = 0;
  mem_seg *shift_ptr = new mem_seg;
  //get first element
  mem_seg *prev_ms_ptr = segments.getNextElementUntilEnd(NULL);
  mem_seg *ms_ptr = segments.getNextElementUntilEnd(prev_ms_ptr);
  char buff[255];

  while (ms_ptr) {
     //free followed by not free
    if (prev_ms_ptr->free && !ms_ptr->free)
    {

      //Swap data in memory
      shift_start = prev_ms_ptr->start;
      int start = ms_ptr->start;
      int end = ms_ptr->end;
      for (int i = start; i <= end; i++) {
        memory[ shift_start++ ] = memory[i];
      }

      //Swap nodes
      ms_ptr->start = prev_ms_ptr->start;
      ms_ptr->end = ms_ptr->start+ms_ptr->size -1;
      prev_ms_ptr->start = ms_ptr->end +1;
      prev_ms_ptr->end = prev_ms_ptr->start+ prev_ms_ptr->size -1;

      segments.swapNodes(prev_ms_ptr->handle);

	}

    //iterate through list
    prev_ms_ptr = ms_ptr;
    ms_ptr = segments.getNextElementUntilEnd(ms_ptr);

  }//end while

  mem_coalesce();
  return 1;
}


void Mem_Mgr::writeToThreadWindow(int threadID, char* text)
{
    //mcb->writeSema->down(threadID);
    mcb->s->getThreadInfo().getDatumById(threadID)->getThreadWin()->write_window(text);
    //mcb->writeSema->up();

}
