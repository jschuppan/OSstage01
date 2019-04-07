#include "mem_mgr.h"
#include <iostream>
#include "window.h"
#include <assert.h> 

//Constructor and default Constructor
Mem_Mgr::Mem_Mgr(unsigned int size, unsigned char default_init_val) {
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
  if (size > available) {
    std::cout << "\nmem_alloc() : not enough memory\n";
    return -1;  //error: not enough memory
  }

  // allocate new memort at end
  // traverse segments looking where the last element ends before the
  // free space starts
  mem_seg* sPtr = segments.getNextElementUntilEnd(NULL);


  mem_seg tmpDatum;
  tmpDatum.handle = next_handle++;
  tmpDatum.owner_tid = tid;
  tmpDatum.size = size;
  tmpDatum.write_cursor = tmpDatum.read_cursor = tmpDatum.start;
  tmpDatum.free = false;

  // if we have nothing in RAM 
  if (sPtr == NULL) 
  {
    // allocate first node
    tmpDatum.start = 0;
    tmpDatum.end = size - 1;
    segments.addToEnd(tmpDatum, tmpDatum.handle);
  }

  // otherwise we need to go to the end after burping to add element
  else 
  {
    // burp()

    int maxEnd = -1;
    while ((sPtr = segments.getNextElementUntilEnd(sPtr)))
    {
      maxEnd = 0;
      if (sPtr->end > maxEnd) 
      {
        maxEnd = sPtr->end;
      }
    }

    // we start our node right after the last found element
    tmpDatum.start = maxEnd + 1;
    tmpDatum.end = tmpDatum.start + size;

    // do some boundary checking as a safeguard
    assert(!(tmpDatum.end > (capacity - 1));

    // if we have no issues push to linked list
    segments.addToEnd(tmpDatum, tmpDatum.handle);


  }


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

  if (ms_ptr == NULL) {
    std::cout << "\nmem_free() : Item segment doesn't exist\n";
    return -1;  //error: not found
  }

  if (tid != ms_ptr->owner_tid) {
    std::cout << "\nmem_free() : access denied\n";
    return -1;  //error: access denied
  }

  unsigned int start = ms_ptr->start;  
  return smallest;
}


/*-----------------------------------------------------------------
Function      : mem_coalesce();
Parameters    :
Returns       : returns 1 on success
Details       : combines holes in the memory
------------------------------------------------------------------*/
int Mem_Mgr::mem_coalesce() {
  mem_seg *prev_ms_ptr = getNextElementUntilEnd(NULL);  //get first element
  mem_seg *ms_ptr = getNextElementUntilEnd(prev_ms_ptr);
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
    text[i] = message[ms_ptr->read_cursor];
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
int Mem_Mgr::mem_write(int handle, unsigned int offset, unsigned int text_size, unsigned char *text, int tid) {
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

  int i = 0;
  while (ms_ptr->write_cursor < ms_ptr->end)
  {

    message[ms_ptr->write_cursor] = text[i];
    ms_ptr->write_cursor++;
    i++;
  }
  // make sure message was null terminated, if not return error
  if (text[i] != '\0')
  {
    return -1; // error
  }
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

  mem_seg *ms_ptr = getNextElementUntilEnd(NULL);  //get first element
  while (ms_ptr = getNextElementUntilEnd(ms_ptr)) {
    if (ms_ptr->free && ms_ptr->size > largest)
      largest = ms_ptr->size;

    ms_ptr = getNextElementUntilEnd(ms_ptr);
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

  mem_seg *ms_ptr = getNextElementUntilEnd(NULL);  //get first element
  while (ms_ptr = getNextElementUntilEnd(ms_ptr))
  {
    if (ms_ptr->free && ms_ptr->size < smallest)
      smallest = ms_ptr->size;

    ms_ptr = getNextElementUntilEnd(ms_ptr);
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
  mem_seg *prev_ms_ptr = getNextElementUntilEnd(NULL);  //get first element
  mem_seg *ms_ptr = getNextElementUntilEnd(prev_ms_ptr);
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
      removeNodeByElement(prev_ms_ptr->handle);
      ms_ptr->write_cursor = ms_ptr->read_cursor = ms_ptr->start;
    }

    prev_ms_ptr = ms_ptr;
    ms_ptr = getNextElementUntilEnd(ms_ptr);
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

  char mBuff[16384];
  std::string tempString;
  char* chr;
  mcb->s->SCHEDULER_SUSPENDED = true;

  // get threadID of current element
  tempString = mcb->ipc->Message_Print();

  // store returned string into buffer
  chr = strdup(tempString.c_str());
  sprintf(mBuff, "%s",chr);

  // write buffer to window
  usleep(5000);
  targetWin->write_window(mBuff);
  // deallocate memory for chr
  free(chr);

  mcb->s->SCHEDULER_SUSPENDED = false;
}