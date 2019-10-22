#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){
  int fd = running->syscall_args[0];	//prendo id semaforo
  
  SemDescriptor* sem_desc = SemDescriptorList_byFd(&running->sem_descriptors, fd);
  //controllo se trovo il semaforo
  if(sem_desc==0){
        printf("Chiusura semaforo fallita!\n");
        running->syscall_retvalue = DSOS_ESEMCLOSE;
        return;
  }
  
  //rimuovo il semaforo dalla lista dei descrittori
  List_detach(&running->sem_descriptors, (ListItem*)sem_desc);
  
  //prendo il semaforo
  Semaphore* sem = sem_desc->semaphore;
  
  if(sem==0){	//controllo se trovo il semaforo
  	printf("Chiusura semaforo fallita!\n");
  	running->syscall_retvalue=DSOS_ESEMCLOSE;
  	return;
  }
  
  //rimuovo il puntatore al descrittore dalla lista
  SemDescriptorPtr* desc_ptr = (SemDescriptorPtr*)List_detach(&sem->descriptors,(ListItem*)(sem_desc->ptr));  
  
 
  //libero la memoria
  SemDescriptor_free(sem_desc);
  SemDescriptorPtr_free(desc_ptr);
  
  //controllo di non avere descrittori in waiting e quindi rimuovo il semaforo dalla lista
  if(sem->descriptors.size == 0 && sem->waiting_descriptors.size==0){
        List_detach(&semaphores_list, (ListItem*)sem);
        Semaphore_free(sem);
        printf("Chiusura semaforo con id=%d\n",fd+1);
    }
  
  running->syscall_retvalue=0;
  return;
}
