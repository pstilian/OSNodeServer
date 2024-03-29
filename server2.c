// Peter Stilian, Donald Christensen, Jonathan Sellier, Emily Cardella

#include <string.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<sys/shm.h>
#include<sys/sem.h>
#define PORTNUM  10101 /* the port number the server will listen to*/
#define DEFAULT_PROTOCOL 0  /*constant for default protocol*/
#define SHMKEY ( (key_t) 7890)
#define SEMKEY ( (key_t) 400L)
#define NSEMS 1

// Defining Shared Memory
typedef struct{
  int numClients;
  char letterBoard[36];
  int waitVal;
  int numSelected;
  int sockID[5];
}shared_mem;
shared_mem *sharedData;

// Defining Semaphore union
union{
  int val;
  struct semid_ds *buf;
  ushort *array;
} semctl_arg;

static struct sembuf Wait   = {0,-1,0};
static struct sembuf Signal = {0,1,0};

// Declared Functions
void doprocessing (int sock);
void rungame (int sock);

// Global Variables
int numReady, status, semStatus, semid, semnum, semval;


//--------------------------------Mainline Code---------------------------------
int main( int argc, char *argv[] ) {
    key_t key = 123; /* shared memory key */
    int shmid;
    char *shmadd;
    char letters[36] = "a b c d\ne f g h\ni j k l\nm n o p\n";
    int sockfd, newsockfd, portno, clilen;
    char buffer[1024];
    struct sockaddr_in serv_addr, cli_addr;
    int status, pid, temp;
    shmadd = (char *) 0;

    // Generating Shared Memory
    if ((shmid = shmget (key, sizeof(int), IPC_CREAT | 0666)) < 0){
   		perror ("shmget");
   		exit (1);
  	}
  	if ((sharedData = (shared_mem *) shmat (shmid, shmadd, 0)) == (shared_mem *) -1){
   		perror ("shmat");
   		exit (0);
  	}

    // This line gets the semaphore
	  semid = semget (SEMKEY, NSEMS, IPC_CREAT | 0666);
    if (semid < 0){
	     printf( "error in creating semaphore");
	      exit (1);
    }

    /* this sets the value of the semaphore semnum to semctl_arg.val*/
    semnum = 0;
    semctl_arg.val = 1;
    status = semctl(semid, semnum, SETVAL, semctl_arg);

    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM,DEFAULT_PROTOCOL );
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = PORTNUM;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    /* Now bind the host address using bind() call.*/
    status =  bind(sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr));
    if (status < 0) {
      perror("ERROR on binding");
      exit(1);
    }

    /* Now Server starts listening clients wanting to connect. No more than 5
    clients allowed */
    listen(sockfd,5);
    int i = 0;
    clilen = sizeof(cli_addr);

    // Manage Shared Memory Here
    sharedData->numClients = 0;
    strcpy(sharedData->letterBoard, letters);
    sharedData->waitVal = 0;
    sharedData->numSelected = 0;

    while (1) {
       newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,  &clilen);
       if (newsockfd < 0) {
         perror("ERROR on accept");
         exit(1);
        }
      /* Create child process */
        pid = fork();
        if (pid < 0) {
            perror("ERROR on fork");
            exit(1);
        }
        if (pid == 0) {
         /* This is the client process */
            printf("New Client\n");
            close(sockfd);
            doprocessing(newsockfd);
            exit(0);
        }
        else {
            close(newsockfd);
        }
    } /* end of while */

    /* the next set of statements releases the shared memory */
    if ((shmctl (shmid, IPC_RMID, (struct shmid_ds *) 0)) == -1){
      perror ("shmctl");
      exit (-1);
    }
    printf("Shared memory has been released.\n");

    /* this releases the semaphore numbered "0" the first in the set*/
	  semctl_arg.val = 0;
    semnum = 0;
    status = semctl(semid, semnum, IPC_RMID, semctl_arg);
    if (status < 0){
	     printf( "error in releasing semaphore");
		   exit(1);
	  }
    else printf("Semaphore has been released");
}

/*
   Increased buffer size for processing client info using forloops
   taking response using forloops and passing within buffer.
   need to add Mutex or Semaphores and figure out second client.
   Additionaly need to recieve updated matrix from server before mutex or
   semiphre swap to maintain correct state when entering critical section.
*/
void doprocessing (int sock) {
    int status;
    char buffer[1024];
    //Loop to keep socket running until correct input is recieved
    int go = 1;
    printf("Waiting on ready...\n");
    while(go){
        status = read(sock,buffer,255);
        // check to make sure status is good
        if(status < 0){
          perror("ERROR reading from socket");
          exit(1);
        }

        bzero(buffer,256);

        // Checking to see if all clients are ready
        if(buffer[0] == 'y'){
          // Increase the number of clients for each call
          sharedData->numClients++;
          printf("%d Clients Connected", sharedData->numClients);
           // Check to see if at least 2 players are ready
           // first case if not enough players
           if(sharedData->numClients < 2) status = write(sock, "Waiting on other players.\n", 28);
          // second case 2 players ready
          //waits for both players
          if(sharedData->numClients == 2){
              status = semop(semid, &Signal, 1);
          }

          // Prints letterboard to server
          status = semop(semid, &Wait, 1);
          //printf("-------------\n");
          //printf("%s",sharedData->letterBoard);
          //printf("-------------\n");
          status = semop(semid, &Signal, 1);

          // Sends letterboard to clients
          status = semop(semid, &Wait, 1);
          status = write(sock, sharedData->letterBoard, strlen(sharedData->letterBoard));
          status = semop(semid, &Signal, 1);

          // run the game
          rungame(sock);
      }
  }
}

void rungame(sock){
  char buffer[1024];
  while(sharedData->numSelected < 16){
    bzero(buffer,256);
    status = read(sock, buffer, 255); //recieving letter from client
    if (status < 0){
      perror("ERROR reading from socket");
      exit(1);
    }
  }
}
