#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semOpen(){
  int id = running->syscall_args[0];		//id del semaforo	
  int count = running->syscall_args[1];		//contatore processi
  
  Semaphore* sem = SemaphoreList_byId(&semaphores_list,id);		//controllo se esiste il semaforo tramite id
  
  if(!sem) {								//allocazione semaforo nel caso non esista
  	sem = Semaphore_alloc(id,count);
  	if (!sem)		//controllo allocazione 
  	   {
  	   	printf("Il semaforo con id=%d non è stato allocato correttamente!\n", id);
  		running->syscall_retvalue = DSOS_ESEMOPEN;
  		return;
  	   }
  	}
  List_insert(&semaphores_list, semaphores_list.last, (ListItem*) sem);		//inserisco il semaforo nella lista dei semafori
  
  printf("Il semaforo con id=%d è stato allocato correttamente!\n", id);
  
  SemDescriptor* desc = SemDescriptor_alloc(running->last_sem_fd, sem, running);	//allocazione descrittore semaforoù
  if (!desc) 
     {
  	printf("Allocazione descrittore semaforo fallita!");
  	running->syscall_retvalue = DSOS_ESEMOPEN;
  	return;
     }

  running->last_sem_fd++;		//incremento numero di semafori aperti

  List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem*) desc);		//inserimento nella lista descrittori

  SemDescriptorPtr* desc_ptr = SemDescriptorPtr_alloc(desc);		//allocazione puntatore a descrittore

  desc->ptr = desc_ptr;			
  List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*) desc_ptr);		//puntatore aggiunto alla lista dei puntatori

  running->syscall_retvalue = desc->fd;			//restituisco valore file descriptor
  return;
}
