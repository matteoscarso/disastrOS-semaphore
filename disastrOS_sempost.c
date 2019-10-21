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
  
  if(desc==0){		//controllo esistenza descrittore semaforo
  	printf("Sempost su semaforo %d fallita!\n",fd);
  	running->syscall_retvalue = DSOS_ESEMPOST;
  	return;  	
  }
  
  Semaphore* sem = desc->semaphore;		//prendo il semaforo del descrittore	
  sem->count++;		//incremento contatore di sem
  
  if(sem->count <= 0){
  	List_insert(&ready_list, ready_list.last, (ListItem*) running);	//inserisco processo running nella ready_list
  	SemDescriptorPtr* desc_ptr= (SemDescriptorPtr*)List_detach(&sem->waiting_descriptors, (ListItem*) sem->waiting_descriptors.first);		//rimuovo il first descriptor del semaforo dalla lista dei descrittori in attesa
  	List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*) desc_ptr);
  	List_detach(&waiting_list, (ListItem*)desc_ptr->descriptor->pcb);	//rimuovo il processo dalla waiting_list tramite il suo desc_ptr
  	running->status = Ready;
  	running = desc_ptr->descriptor->pcb;	//metto in esecuzione il processo rimosso dalla waiting_list
  }
  running->syscall_retvalue=0;
  return;
}
