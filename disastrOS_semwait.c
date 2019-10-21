#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){
  int fd = running->syscall_args[0];	//fd semaforo
  
  SemDescriptor* desc = SemDescriptorList_byFd(&running->sem_descriptors, fd);		//prendo il descrittore del semaforo tramite fd
  
  if(desc==0){		//controllo se trovo il descrittore
  	printf("Semwait su semaforo %d fallita!\n",fd);
  	running->syscall_retvalue = DSOS_ESEMWAIT;
  	return;  	
  }
  
  Semaphore* sem = desc->semaphore;		//prendo il semaforo del descrittore
  SemDescriptorPtr* desc_ptr=desc->ptr;		//salvo puntatore al descrittore
  sem->count--;		//decremento il contatore del semaforo
  
  if(sem->count<0){
  	List_detach(&sem->descriptors, (ListItem*)desc_ptr);	//rimuovo il descrittore del processo dalla lista dei descrittori
  	List_insert(&sem->waiting_descriptors, sem->waiting_descriptors.last, (ListItem*) desc->ptr);	//lo inserisco nella lista di waiting
  	running->status=Waiting;
  	
  	List_insert(&waiting_list, waiting_list.last, (ListItem*)running);	//inserisco il processo in esecuzione nella waiting_list
  	
  	PCB* pcb = (PCB*) List_detach(&ready_list, (ListItem*)ready_list.first);	//prendo il primo elemento dalla ready list
  	running=(PCB*)pcb;	//mando in esecuzione l'elemento appena preso
  }
  running->syscall_retvalue=0;
  return;
}
