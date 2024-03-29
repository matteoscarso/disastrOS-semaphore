#include <stdio.h>
#include <unistd.h>
#include <poll.h>

#include "disastrOS.h"

#define BUF_LENGTH 93


int sem_empty, sem_filled, sem_write, sem_read;
int buf[BUF_LENGTH];
int buf_length=BUF_LENGTH;
int wr_idx=0;
int rd_idx=0;
int deposit=0;
int iteration=BUF_LENGTH/5;
int shared_variable;

void producer(){
	//funzione produttore che salva nel buffer la variabile condivisa e la incrementa
	disastrOS_semWait(sem_empty);
	disastrOS_semWait(sem_write);
	
	buf[wr_idx]=shared_variable;
	wr_idx=(wr_idx+1) % buf_length;
	shared_variable++;
	
	disastrOS_semPost(sem_write);
	disastrOS_semPost(sem_filled);
	
}

void consumer(){
	//funzione consumatore che legge il valore dal buffer e lo somma nella variabile globale deposit
	disastrOS_semWait(sem_filled);
	disastrOS_semWait(sem_read);
	
	deposit+=buf[rd_idx];
	rd_idx=(rd_idx+1)%buf_length;
	
	disastrOS_semPost(sem_empty);
	disastrOS_semPost(sem_read);
	

}
// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Hello, I am the sleeper, and I sleep %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}

void childFunction(void* args){
  printf("Hello, I am the child function %d\n",disastrOS_getpid());
  printf("Apertura semafori\n");
  sem_filled=disastrOS_semOpen(1,0);
  sem_empty=disastrOS_semOpen(2, buf_length);
  sem_read=disastrOS_semOpen(3,1);
  sem_write=disastrOS_semOpen(4,1);
  
  
  for(int i=0; i < iteration; i++){
    if (disastrOS_getpid() % 2 == 0){
     consumer();
     }
    else {
     producer();
    }
  }
  
  
  printf("PID: %d, terminating\n", disastrOS_getpid());
  
  printf("Chiusura semafori\n");
  disastrOS_semClose(sem_filled);
  disastrOS_semClose(sem_empty);
  disastrOS_semClose(sem_write);
  disastrOS_semClose(sem_read);
  
  disastrOS_exit(disastrOS_getpid());
}


void initFunction(void* args) {
  disastrOS_printStatus();
  printf("hello, I am init and I just started\n");
  disastrOS_spawn(sleeperFunction, 0);
  
  shared_variable=1;	//inizializzo la variabile condivisa
  
   printf("STATO INIZIALE DEL BUFFER\n");
  for(int i=0; i < buf_length; i++){
    printf("%d ", buf[i]);
  }
  printf("\n");
  
  
  
  printf("I feel like to spawn 10 nice threads\n");
  int alive_children=0;
  for (int i=0; i<10; ++i) {
    disastrOS_spawn(childFunction, 0);
    alive_children++;
  }

  disastrOS_printStatus();
  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){ 
    disastrOS_printStatus();
    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n",
	   pid, retval, alive_children);
    --alive_children;
  }
  //ciclo "toppa"
  if(buf_length%5!=0){
	  for(int i=rd_idx;i<buf_length;i++){
	  	buf[i]=shared_variable;
		shared_variable++;
		deposit+=buf[i];
	  }
  }
  
  printf("STATO FINALE DEL BUFFER\n");
  for(int i=0; i < buf_length; i++){
    printf("%d ", buf[i]);
  }
  printf("\n");
  
  int val_exp=(buf_length*(buf_length+1))/2;
  
  printf("Valore ottenuto: %d\nValore atteso: %d\n",deposit,val_exp);
  
  printf("shutdown!");
  disastrOS_shutdown();
}

int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  // we create the init process processes
  // the first is in the running variable
  // the others are in the ready queue
  printf("the function pointer is: %p", childFunction);
  // spawn an init process
  printf("start\n");
  disastrOS_start(initFunction, 0, logfilename);
  return 0;
}
