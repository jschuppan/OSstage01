#include "mem_mgr.h"
#include <iostream>

using namespace std;

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


Mem_Mgr::~Mem_Mgr() {
  delete memory;
  segments.deleteList();
}


/*********************  needs attention  ***********************/
int Mem_Mgr::mem_alloc(unsigned int size, int tid) {
  if (size > available) {
    cout << "\nmem_alloc() : not enough memory\n";
    return -1;  //error: not enough memory
  }

  //look for a free node (code not finished!!!!)
  // traverse segments looking for smallest gap that fulfills size
  // requirement
  mem_seg* sPtr = NULL;
  while ((sPtr = segments.getNextElementUntilEnd(sPtr)))
  {
    // we will have to sort the node in a way where we can determine
    // the smallest amount of space between them left to right
  }

  // RAM IS empty
  if (sPtr == NULL)
    // code here


  mem_seg ms;
  ms.handle = next_handle++;
  ms.owner_tid = tid;
  //ms.start = ;
  //ms.end = ;
  //ms.size = ;
  ms.write_cursor = ms.read_cursor = ms.start;
  ms.free = false;

  //add new node
  //code here

  available -= size;
  used += size;

  return ms.handle;
}


int Mem_Mgr::mem_free(int handle, int tid) {
  mem_seg *ms_ptr = segments.getDatumById(handle);

  if (ms_ptr == NULL) {
    cout << "\nmem_free() : Item segment doesn't exist\n";
    return -1;  //error: not found
  }

  if (tid != ms_ptr->owner_tid) {
    cout << "\nmem_free() : access denied\n";
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


int Mem_Mgr::mem_read(int handle, unsigned char *c, int tid) {
  mem_seg *ms_ptr = segments.getDatumById(handle);

  if (ms_ptr == NULL)
  {
    cout << "\nmem_read() : Item segment doesn't exist\n";
    return -1;  //error: not found
  }

  if (tid != ms_ptr->owner_tid)
  {
    cout << "\nmem_read() : access denied\n";
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


/*********************  needs attention  ***********************/
int Mem_Mgr::mem_read(int handle, unsigned int offset, unsigned int text_size, unsigned char *text, int tid) {
  mem_seg *ms_ptr = segments.getDatumById(handle);

  if (ms_ptr == NULL) {
    cout << "\nmem_read() : Item segment doesn't exist\n";
    return -1;  //error: not found
  }

  if (tid != ms_ptr->owner_tid || offset > ms_ptr->end) {
    cout << "\nmem_read() : access denied\n";
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


int Mem_Mgr::mem_write(int handle, unsigned char c, int tid) {
  mem_seg *ms_ptr = segments.getDatumById(handle);

  if (ms_ptr == NULL)
  {
    cout << "\nmem_write() : Item segment doesn't exist\n";
    return -1;  //error: not found
  }

  if (tid != ms_ptr->owner_tid)
  {
    cout << "\nmem_write() : access denied\n";
    return -1;  //error: access denied
  }

  if (ms_ptr->write_cursor > ms_ptr->end)
  {
    cout << "\nmem_write() : segment is full\n";
    return -1;  //error: segment is full
  }

  memory[ ms_ptr->write_cursor ] = c;

  ms_ptr->write_cursor++;

  return 1;  //success
}


/*********************  needs attention  ***********************/
int Mem_Mgr::mem_write(int handle, unsigned int offset, unsigned int text_size, unsigned char *text, int tid) {
  mem_seg *ms_ptr = segments.getDatumById(handle);

  if (ms_ptr == NULL)
  {
    cout << "\nmem_write() : Item segment doesn't exist\n";
    return -1;  //error: not found
  }

  if (tid != ms_ptr->owner_tid)
  {
    cout << "\nmem_write() : access denied\n";
    return -1;  //error: access denied
  }

  if (ms_ptr->write_cursor > ms_ptr->end)
  {
    cout << "\nmem_write() : segment is full\n";
    return -1;  //error: segment is full
  }

  int i = 0;
  while (ms_ptr->write_cursor < ms_ptr->end)
  {
    // this is not setting any actual text since write cursor is an index
    // its just a place holder for now
    ms_ptr->write_cursor = text[i];
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


void Mem_Mgr::set_mcb(MCB *mcb)
{
  this->mcb = mcb;
}


int Mem_Mgr::mem_left()
{
  return available;
}


int Mem_Mgr::mem_largest() {
  unsigned int largest = 0;

  mem_seg *ms_ptr = getNextElementUntilEnd(NULL);  //get first element
  while (ms_ptr) {
    if (ms_ptr->free && ms_ptr->size > largest)
      largest = ms_ptr->size;

    ms_ptr = getNextElementUntilEnd(ms_ptr);
  }

  return largest;
}


int Mem_Mgr::mem_smallest() {
  unsigned int smallest = capacity;

  mem_seg *ms_ptr = getNextElementUntilEnd(NULL);  //get first element
  while (ms_ptr)
  {
    if (ms_ptr->free && ms_ptr->size < smallest)
      smallest = ms_ptr->size;

    ms_ptr = getNextElementUntilEnd(ms_ptr);
  }

  return smallest;
}


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


//
int Mem_Mgr::mem_dump()
{

}
