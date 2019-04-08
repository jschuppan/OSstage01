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

  mem_seg ms;
  // why are we increasing the count here?
  ms.handle = next_handle++;
  ms.owner_tid = -1;
  ms.start = 0;
  ms.end = capacity - 1;
  ms.size = capacity;
  ms.read_cursor = 0;
  ms.write_cursor = 0;
  ms.free = true;

  segments.addToFront(ms, ms.handle);

  memory = new unsigned char[capacity];
  for (unsigned int i = 0; i < capacity; i++)
    memory[i] = default_mem_fill;
}

//Constructor and default Constructor
Mem_Mgr::Mem_Mgr(unsigned int size, unsigned char default_init_val) {
  char buff[255];

  default_mem_fill = default_init_val;
  freed_mem_fill = '#';
  capacity = available = size;
  used = 0;
  next_handle = 0;

  mem_seg ms;
  // why are we increasing the count here?
  ms.handle = next_handle++;
  ms.owner_tid = -1;
  ms.start = 0;
  ms.end = capacity - 1;
  ms.size = capacity;
  ms.read_cursor = 0;
  ms.write_cursor = 0;
  ms.free = true;

  segments.addToFront(ms, ms.handle);

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
/*********************  needs attention  ***********************/
int Mem_Mgr::mem_alloc(unsigned int size, int tid) {
  char buff[255];
  if (size > available) {
    mcb->writeSema->down(tid);
    sprintf(buff, "\n  mem_alloc() : not enough memory\n  available size: %d \n  Requested Size %d\n",available,size);
    mcb->s->getThreadInfo().getDatumById(tid)->getThreadWin()->write_window(buff);
    mcb->writeSema->up();
    return -1;  //error: not enough memory
  }

  // allocate new memort at end
  // traverse segments looking where the last element ends before the
  // free space starts
  mem_seg* sPtr;
  sPtr = segments.getNextElementUntilEnd(NULL);
  int countPos = 0;
  while(sPtr->size < size || !sPtr->free)
  {
    if(!(sPtr = segments.getNextElementUntilEnd(sPtr)))
    {
      if(available >= size)
      {
          burp();
          continue;
      }
      mcb->writeSema->down(tid);
      sprintf(buff, "\n  Not Enough Consecutive Memory\n");
      mcb->s->getThreadInfo().getDatumById(tid)->getThreadWin()->write_window(buff);
      mcb->writeSema->up();
      return -1;
    }
    countPos++;
  }


  mem_seg tmpDatum;
  tmpDatum.handle = next_handle++;
  tmpDatum.owner_tid = tid;
  tmpDatum.size = size;
  tmpDatum.write_cursor = tmpDatum.read_cursor = sPtr->start;
  tmpDatum.free = false;

  tmpDatum.start = sPtr->start;
  tmpDatum.end = sPtr->start + size -1;
  segments.insertNode(tmpDatum, tmpDatum.handle, countPos);

  sPtr->start = tmpDatum.end + 1;
  sPtr->size = sPtr->size - size;
  tmpDatum.write_cursor = tmpDatum.read_cursor = sPtr->start;

  mcb->writeSema->down(tid);
  sprintf(buff, "\n  Memory Allocated \n");
  mcb->s->getThreadInfo().getDatumById(tid)->getThreadWin()->write_window(buff);
  mcb->writeSema->up();


  // otherwise we need to go to the end after burping to add element
  // else
  // {
  //   burp();
  //
  //   int maxEnd = -1;
  //   while ((sPtr = segments.getNextElementUntilEnd(sPtr)))
  //   {
  //     maxEnd = 0;
  //     if (sPtr->end > maxEnd)
  //     {
  //       maxEnd = sPtr->end;
  //     }
  //   }
  //
  //   // we start our node right after the last found element
  //   tmpDatum.start = maxEnd + 1;
  //   tmpDatum.end = tmpDatum.start + size;
  //
  //   // do some boundary checking as a safeguard
  //   assert(!(tmpDatum.end > (capacity - 1)));
  //
  //   // if we have no issues push to linked list
  //   segments.addToEnd(tmpDatum, tmpDatum.handle);
  //
  //
  // }


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
  mem_seg *ms_ptr = segments.getDatumById(handle);
  char buff[255];

  if (ms_ptr == NULL) {
    mcb->writeSema->down(tid);
    sprintf(buff,"\n   mem_free() : Item segment doesn't exist\n");
    mcb->s->getThreadInfo().getDatumById(tid)->getThreadWin()->write_window(buff);
    mcb->writeSema->up();
    return -1;  //error: not found
  }

  if (tid != ms_ptr->owner_tid) {
    mcb->writeSema->down(tid);
    sprintf(buff,"\n   mem_free() : access denied\n");
    mcb->s->getThreadInfo().getDatumById(tid)->getThreadWin()->write_window(buff);
    mcb->writeSema->up();
    return -1;  //error: access denied
  }

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
int Mem_Mgr::mem_read(int handle, unsigned char *c, int tid) {
  mem_seg *ms_ptr = segments.getDatumById(handle);

  if (ms_ptr == NULL)
  {
    std::cout << "\nmem_read() : Item segment doesn't exist\n";
    return -1;  //error: not found
  }

  if (tid != ms_ptr->owner_tid)
  {
    std::cout << "\nmem_read() : access denied\n";
    return -1;  //error: access denied
  }

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
  mem_seg *ms_ptr = segments.getDatumById(handle);

  if (ms_ptr == NULL) {
    std::cout << "\nmem_read() : Item segment doesn't exist\n";
    return -1;  //error: not found
  }

  if (tid != ms_ptr->owner_tid || offset > ms_ptr->end) {
    std::cout << "\nmem_read() : access denied\n";
    return -1;  //error: access denied
  }

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
  mem_seg *ms_ptr = segments.getDatumById(handle);

  if (ms_ptr == NULL)
  {
    std::cout << "\nmem_write() : Item segment doesn't exist\n";
    return -1;  //error: not found
  }

  if (tid != ms_ptr->owner_tid)
  {
    std::cout << "\nmem_write() : access denied\n";
    return -1;  //error: access denied
  }

  if (ms_ptr->write_cursor > ms_ptr->end)
  {
    std::cout << "\nmem_write() : segment is full\n";
    return -1;  //error: segment is full
  }

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
int Mem_Mgr::mem_write(int handle, unsigned int offset, unsigned int text_size, char *text, int tid) {
  mem_seg *ms_ptr = segments.getDatumById(handle);

  if (ms_ptr == NULL)
  {
    std::cout << "\nmem_write() : Item segment doesn't exist\n";
    return -1;  //error: not found
  }

  char buff[255];
  if (tid != ms_ptr->owner_tid)
  {
    mcb->writeSema->down(tid);
    sprintf(buff, "\n  Owner permission denied\n");
    mcb->s->getThreadInfo().getDatumById(tid)->getThreadWin()->write_window(buff);
    mcb->writeSema->up();    return -1;  //error: access denied
  }

  if (ms_ptr->write_cursor+text_size+offset >ms_ptr->end)
  {
    mcb->writeSema->down(tid);
    sprintf(buff,"\n  Segmentation Fault\n");
    mcb->s->getThreadInfo().getDatumById(tid)->getThreadWin()->write_window(buff);
    mcb->writeSema->up();
    return -1;  //error: segment is full
  }

  int i = 0;
  while (i <= text_size && ms_ptr->write_cursor < ms_ptr->end)
  {
    memory[ms_ptr->write_cursor+offset] =text[i];
    ms_ptr->write_cursor++;
    i++;
  }
  // make sure message was null terminated, if not return error
  // if (text[i] != '\0')
  // {
  //   return -1; // error
  // }
  // reset cursor
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
int Mem_Mgr::mem_coalesce() {
  mem_seg *prev_ms_ptr = segments.getNextElementUntilEnd(NULL);  //get first element
  mem_seg *ms_ptr = segments.getNextElementUntilEnd(prev_ms_ptr);
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
      segments.removeNodeByElement(prev_ms_ptr->handle);
      ms_ptr->write_cursor = ms_ptr->read_cursor = ms_ptr->start;
    }

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
   mcb->s->SCHEDULER_SUSPENDED = true;

   tempString = "\n\n   Status \t Memory Handle \t Starting Location \t Ending Location \t Size \t task-ID\n";
   chr = strdup(tempString.c_str());
   sprintf(mBuff, "%s",chr);
   Win->write_window(mBuff);
   int count = 0;
   char cBuff[1025];

   while ((myT = segments.getNextElementUntilEnd(myT)))
   {
     if(myT->free)
     {
       sprintf(mBuff, "   Free \t\t ");
     }
     else
       sprintf(mBuff, "   Used \t\t ");

     sprintf(mBuff + strlen(mBuff),"%d \t\t", myT->handle);
     sprintf(mBuff + strlen(mBuff),"%d \t\t\t", myT->start);
     sprintf(mBuff + strlen(mBuff),"%d \t\t", myT->end);
     sprintf(mBuff + strlen(mBuff),"%d \t\t", myT->size);
     sprintf(mBuff + strlen(mBuff),"%d \n", myT->owner_tid);

     Win->write_window(mBuff);
   }

   sprintf(cBuff, "   %c",memory[count]);
   while(count < 1024)
   {
     if(count % 32 == 0)
          sprintf(cBuff+strlen(cBuff), "\n");
     else if(count == 0)
     {
          continue;
          count++;
      }
     sprintf(cBuff+strlen(cBuff), "   %c",memory[count]);
     count++;
   }
   sprintf(cBuff+strlen(cBuff), "\n");

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
  //int total_burped_size = 0;
  int shift_start = 0;
  //mem_seg hole;
  mem_seg *shift_ptr = new mem_seg;
  mem_seg *prev_ms_ptr = segments.getNextElementUntilEnd(NULL);  //get first element
  mem_seg *ms_ptr = segments.getNextElementUntilEnd(prev_ms_ptr);
  //std::cout << "here";
  while (ms_ptr) {

    //free followed by not free
    if (prev_ms_ptr->free && !ms_ptr->free)
    {
      //set temp values to be swaped
      shift_ptr-> owner_tid = ms_ptr->owner_tid;
      shift_ptr-> handle = ms_ptr->handle;
      shift_ptr-> size = ms_ptr->size;

      //set next node to previous
      ms_ptr->free = true;
      ms_ptr->size = prev_ms_ptr->size;
      ms_ptr->start = prev_ms_ptr->end +1;
      ms_ptr->end = ms_ptr-> start + ms_ptr->size;
      ms_ptr->owner_tid = -1;
      ms_ptr->read_cursor= ms_ptr->write_cursor = ms_ptr->start;

      //set previous to next
      prev_ms_ptr->free = false;
      prev_ms_ptr->owner_tid = shift_ptr->owner_tid;
      prev_ms_ptr->handle = shift_ptr->handle;
      prev_ms_ptr->size = shift_ptr->size;
      prev_ms_ptr->end = prev_ms_ptr->start + prev_ms_ptr->size;
      prev_ms_ptr->write_cursor = prev_ms_ptr->read_cursor = prev_ms_ptr->start;


      // //total_burped_size += prev_ms_ptr->size;
      //
      // //shift all following nodes and memory back by the size of the hole
      // shift_start = prev_ms_ptr->start;
      // shift_ptr = ms_ptr;
      // prev_ms_ptr = shift_ptr;
      // //  while (shift_ptr) {
      //
    	// //if a node is free, don't shift the contents
    	// //if (!shift_ptr->free) {
      int start = prev_ms_ptr->start;
      int end = prev_ms_ptr->end;
      for (int i = start; i <= end; i++) {
        memory[ shift_start++ ] = memory[i];
      }
	}
  //free followed by free
  // else if(prev_ms_ptr->free && ms_ptr->free)
  // {
  //   mem_coalesce();
  //   //prev_ms_ptr = segments.getNextElementUntilEnd(NULL);
  //   ms_ptr = segments.getNextElementUntilEnd(prev_ms_ptr);
  //
  //   continue;
  // }
  //not free followed by free
  else
  {
    prev_ms_ptr = ms_ptr;
    ms_ptr = segments.getNextElementUntilEnd(ms_ptr);
  }
	//shift_ptr->start -= prev_ms_ptr->size;
	//shift_ptr->end -= prev_ms_ptr->size;

	// shift_ptr = segments.getNextElementUntilEnd(shift_ptr);
  //     }

      //remove the hole
      //segments.removeNodeByElement(prev_ms_ptr->handle);


    //prev_ms_ptr = ms_ptr;
  // ms_ptr = segments.getNextElementUntilEnd(ms_ptr);

}
mem_coalesce();
  //hole.handle = prev_ms_ptr->handle;
  //hole.owner_tid = -1;
  //hole.start = capacity - 1 - prev_ms_ptr->size;
  //prev_ms_ptr->end = prev_ms_ptr->start + available - 1;
  //prev_ms_ptr->size = available;
  //prev_ms_ptr->read_cursor = 0;
  //prev_ms_ptr->write_cursor = 0;
  //hole.free = true;

  //mem_coalesce();

  return 1;
}
