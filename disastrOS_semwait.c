#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){
  int fd = running->syscall_args[0];
  
  SemDescriptor* desc = SemDescriptorList_byId(&running->sem_descriptors, fd);
  
  Semaphore* sem = desc->semaphore;		//prendo il semaforo del descrittore	
  sem->count--;
}
