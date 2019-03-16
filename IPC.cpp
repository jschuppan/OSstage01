#include "IPC.h"
//#include "Ultima.cpp"

//default constructor
IPC::IPC()
{

}


int IPC::Message_Send(Message_Type *message)
{
  return 0;
}
int IPC::Message_Recieve(int task_Id, Message_Type *message)
{
  return 0;
}
int IPC::Message_Count(int task_Id)
{
  return 0;
}
int IPC::Message_Count()
{
  return 0;
}
void IPC::Message_Print(int task_Id)
{

}
int IPC::Message_DeleteAll(int task_Id)
{
  return 0;
}
void IPC::setMCB(void* mcb)
{
  this->mcb = mcb;
}
