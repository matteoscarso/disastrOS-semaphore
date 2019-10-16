#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){
  int id = running->syscall_args[0];
  
  SemDescriptor* sem_desc = SemDescriptorListbyId(&running->sem_descriptor, id);
  
  List_detach(&running->sem_descriptor, (ListItem*)sem_desc);
  
  Semaphore* sem = sem_desc->semaphore;
  
  SemDescriptorPtr desc_ptr = (SemDescriptorPtr*)List_detach(&sem->descriptors,(ListItem*)(sem_desc->ptr));  
  
  SemDescriptor_free(sem_desc);
  SemDescriptorPtr_free(desc_ptr);
  
  running->syscall_retvalue=0;
  return;
}
