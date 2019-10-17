#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){
  int fd = running->syscall_args[0];
  
  SemDescriptor* desc = SemDescriptorList_byFd(&running->sem_descriptors, fd);
  
  Semaphore* sem = desc->semaphore;		//prendo il semaforo del descrittore	
  sem->count++;
  
  if(s->count <= 0){
  	List_insert(&ready_list, ready_list.last, (ListItem*) running);
  	SemDescriptorPtr* desc_ptr= (SemDescriptorPtr*)List_detach(&sem->waiting_descriptors, (ListItem*) sem->waiting_descriptors.first);
  	List_insert(&sem->descriptors, s->descriptors.last, (ListItem*) desc_ptr);
  	List_detach(&waiting_list, (ListItem*)desc_ptr->descriptor->pcb);
  }
  running->syscall_retvalue=0;
  return;
}
