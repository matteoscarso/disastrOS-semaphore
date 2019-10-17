#include <stdio.h>
#include <unistd.h>
#include <poll.h>

#include "disastrOS.h"

#define BUF_LENGTH 50
#define COUNTER 10

int sem_empty, sem_filled, sem_write, sem_read;
int buf[BUF_LENGTH];
int buf_length=BUF_LENGTH;
int wr_idx=0;
int rd_idx=0

unsigned long int shared_variable;

int producer(){
	disastrOS_semWait(sem_empty);
	disastrOS_semPost(sem_write);
	
	int ret=shared_variable;
	buf[wr_idx]=shared_variable;
	wr_idx=(write_index+1) % buf_length;
	shared_variable++;
	
	disastrOS_semPost(sem_write);
	disastrOS_semPost(sem_filled);
	
	return ret;
}

int consumer(){
	int ret;
	
	disastrOS_semWait(sem_filled);
	disastrOS_semWait(sem_read);
	
	ret=buf[rd_idx];
	rd_idx=(read_index+1)%buf_length;
	
	disastrOS_semPost(sem_empty);
	disastrOS_semPost(sem_read);
	
	return ret;

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
  
  sem_filled=disastrOS_semOpen(1,0);
  sem_empty=disastrOS_semOpen(2, buf_length);
  sem_read=disastrOS_semOpen(3,1);
  sem_write=disastrOS_semOpen(4,1);
  
  
  
  printf("PID: %d, terminating\n", disastrOS_getpid());

  
  disastrOS_exit(disastrOS_getpid());
}


void initFunction(void* args) {
  disastrOS_printStatus();
  printf("hello, I am init and I just started\n");
  disastrOS_spawn(sleeperFunction, 0);
  

  printf("I feel like to spawn 10 nice threads\n");
  int alive_children=0;
  for (int i=0; i<10; ++i) {
    int type=0;
    int mode=DSOS_CREATE;
    printf("mode: %d\n", mode);
    printf("opening resource (and creating if necessary)\n");
    int fd=disastrOS_openResource(i,type,mode);
    printf("fd=%d\n", fd);
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
