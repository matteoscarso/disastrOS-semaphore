#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){
  int fd = running->syscall_args[0];
  
  SemDescriptor* desc = SemDescriptorList_byFd(&running->sem_descriptors, fd);
  
  Semaphore* sem = desc->semaphore;		//prendo il semaforo del descrittore
  SemDescriptorPtr* desc_ptr=desc->ptr;	
  sem->count--;
  
  if(sem->count<0){
  	List_detach(&sem->descriptors, (ListItem*)desc_ptr);
  	List_insert(&sem->waiting_descriptors, sem->waiting_descriptors.last, (ListItem*) desc->ptr);
  	running->status=Waiting;
  	
  	List_insert(&waiting_list, waiting_list.last, (LastItem*)running);
  	
  	PCB* pcb = (PCB*) List_detach(&ready_list, (ListItem*)ready_list.first);
  	running=(PCB*)pcb;
  }
  running->syscall_retvalue=0;
  return;
}
