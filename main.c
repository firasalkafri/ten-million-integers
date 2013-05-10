#include<stdio.h>
#include<stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include<time.h>
#include <errno.h>
#include <string.h>

#define FIFO_NAME "myFiFo"

void forkChildren (int);       //a simple function to handle multi children
void sendData(int [],int);     //a simple fuction to send data through FIFO, pass an array and it's size
void readData(int);            //a simple fuction to read data through FIFO, onle pass the 
int acquireLock (char *);
void releaseLock (int );

void findDub(int, int);        //each child will use this function to process it's 2M integers to find dublicates
void initDublicatesCounter();  //the global hashed array, all children will call this function to init their counters
void initArray();              //fill "array" with 10000000 random integer

static int array[10000000];    //parent will initialize this array with 10M random integers between -5M and 5M
static int parts[2000000];     //each child will process it's 2000000 integers from this array
static int pos[5000000];       //between 0 and 5000000
static int neg[5000000];       //between -5000000 and -1

int dublicates=0;

int me=0;                      //each child will save it's number here, 0 is parent
int fd;                        //Global file handler

int main()
{

long int i;
initArray();
initDublicatesCounter();
forkChildren(5);
if(me==0){
	printf("Parent starting\n");
	sendData(array,9999999);
}

int pid;
while (pid = waitpid(-1, NULL, 0)) {
   if (errno == ECHILD) {
      break;
   }
}
}



void forkChildren (int nChildren) {
    int i;
    pid_t pid;
    for (i = 1; i <= nChildren; i++) {
        pid = fork();
        if (pid == -1) {
            printf("Can't fork child number %d\n",i);
            return;
        }
        if (pid == 0 && i==1) {
	    me = 1;
            printf("I am child: %d PID: %d\n",i, getpid());
	    initDublicatesCounter();
	    readData(1999999);
	    	 findDub(0,1999999);
            return;
        }
        
        if (pid == 0 && i==2) {
	    me = 2;
            printf("I am child: %d PID: %d\n",i, getpid());
	    initDublicatesCounter();
	    readData(1999999);
	          findDub(0,1999999);
            return;
        }
        
        if (pid == 0 && i==3) {
	    me = 3;
            printf("I am child: %d PID: %d\n",i, getpid());
	    initDublicatesCounter();
	    readData(1999999);
	      	  findDub(0,1999999);
            return;
        }       

        if (pid == 0 && i==4) {
	    me = 4;
            printf("I am child: %d PID: %d\n",i, getpid());
	    initDublicatesCounter();
	    readData(1999999);
	          findDub(0,1999999);
	    return;
        } 

        if (pid == 0 && i==5) {
	    me = 5;
            //printf("I am child: %d PID: %d\n",i, getpid());
	    readData(1999999);
                findDub(0,1999999);
	    return;
        }  
    }
}

void sendData(int data[],int lim){
  int num,dataCounter,lock;
  
  mknod(FIFO_NAME, S_IFIFO | 0666, 0);  
  fd = open(FIFO_NAME, O_WRONLY); //here the program will stop excution 
                                  //and keep waiting for a process to open the 
				  //file and start reading
     printf("Sending all 10 million integers\n");
  for(dataCounter=0;dataCounter<=lim;dataCounter++){
        if ((num = write(fd, &data[dataCounter], sizeof(int))) == -1)
            perror("Can't communicate");
        else{
            //data was sent successfuly
	}
    }
     printf("sent all data\n");
}

void readData(int lim){
    int data;
    int i,num,lock;
    
    mknod(FIFO_NAME, S_IFIFO | 0666, 0);
    
    if((lock = acquireLock (FIFO_NAME)) < 0){
      printf("Child %d waiting lock\n",me);
      while((lock = acquireLock (FIFO_NAME)) < 0) {
      }    
    }

      printf("Child %d got lock\n",me);
      printf("Child %d started reading data\n",me);

    fd = open(FIFO_NAME, O_RDONLY,O_NONBLOCK);

    for(i=0;i<=lim;i++){
        if ((num = read(fd, &data, sizeof(int))) == -1){
            perror("Can't read data");
	}
        else {
	    parts[i]=data;
            //printf("child %d reading: \"%d\"\n",me, data);
        }
}
    releaseLock(lock);  
    printf("Child %d released lock and read it's data\n",me);
}

int acquireLock (char *fileSpec) {
    int lockFd;

    if ((lockFd = open (fileSpec, O_RDWR, 0666))  < 0)
        return -1;

    if (flock (lockFd, LOCK_EX | LOCK_NB) < 0) {
        close (lockFd);
        return -1;
    }

    return lockFd;
}

void releaseLock (int lockFd) {
    flock (lockFd, LOCK_UN);
    close(fd);
}

void findDub(int from, int to){
int i,counter;
  for(i=from;i<=to;i++){
    if(parts[i]>=0){
    pos[parts[i]]++;
    }
  
    if(parts[i]<0){
      long int conv = parts[i];
      conv=abs(conv);
      neg[conv]++;
    }
  }


  for(counter=0;counter<=5000000;counter++){
    if(pos[counter]>0 || neg[counter]>0){
      dublicates++;
    }
  }
    printf("\nDublicates = %d\n",dublicates);
}

void initDublicatesCounter(){ 
  int counter=0;
  for(counter=0;counter<=5000000;counter++){
  pos[counter]=0; 
  neg[counter]=0;
}
}

void initArray(){
  int i;
  srand (time(NULL));
for(i=0;i<=9999999;i++){
  if(i==5000000 || i==2500000 || i==7500000){
    srand (time(NULL));
  }
    array[i] = (rand() % (5000000-(-5000000)+1))+(-5000000);
}
}
