// Peter Stilian, Donald Christensen, Jonathan Sellier, Emily Cardella

#include <string.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#define PORTNUM  10101 /* the port number the server will listen to*/
#define DEFAULT_PROTOCOL 0  /*constant for default protocol*/
#define SHMKEY ((key_t) 7890)

typedef struct
{
    int scores[5];
    int count;
}shared_mem;
shared_mem *playerInfo;




//Initialized a matrix and testMap that needs ints

char gameBoard[4][4]=       {{'A','B','C','D'},
                             {'E','F','G','H'},
                             {'I','J','K','L'},
                             {'M','N','O','P'}};
                             
char gameBoardMap[4][4] =   {{'4','6','2','1'},
                             {'0','6','2','3'},
                             {'8','4','9','1'},
                             {'6','6','8','9'}};

int scoreMap[4][4] =        {{4,6,2,1},
                             {0,6,2,3},
                             {8,4,9,1},
                             {6,6,8,9}};


void printBoard(int i, int j);
void rungame(int sock);
void doprocessing (int sock, int pid);
char buffer[256];

int main( int argc, char *argv[] ) {

    //int client_size[6];
    
    int sockfd, newsockfd, portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int status, pid;
    /////////Added working on SharedMemory /////////
    int shmid;
    key_t key = 123; /*shared memory key*/
    char* shmadd;
    shmadd = (char*)0;


    if((shmid = shmget (SHMKEY, sizeof(int), IPC_CREAT | 0666)) < 0){
        perror ("shmget");
        exit (1);
    }

    if ((playerInfo = (shared_mem *) shmat (shmid, shmadd, 0)) == (shared_mem *) -1){
        perror ("shmat");
        exit (0);
    }
    ////////////////End of SharedMemory setup /////////////////////
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
    /////////////////old////////////////
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
            
            close(sockfd);
            doprocessing(newsockfd, pid);
            exit(0);
        }
        else {
            close(newsockfd);
        }
    } /* end of while */
    ////////////////Old///////////
}

void doprocessing (int sock, int pid) {
    int i = 0;
    int j = 0;
    int k = 0;
    int status;
    bzero(buffer,256);
    //Loop to keep socket running until correct input is recieved
    int go = 1;
    while(go){
        status = read(sock,buffer,256);
        if(buffer[0] == 'y'){
            bzero(buffer,256);
            printf("Im Ready! \n");
            playerInfo->count++;
            go = 0;
        }
    }

    //Fills buffer with board.
    //create sendBoard function;

    for(i = 0; i < 4; i++){
        for(j = 0; j < 4; j++){
            buffer[k] = gameBoard[i][j];
            k++;
            if(j == 3){
                buffer[k] = '\n';
                k++;
            }
        }
    }
    k = 0;
    printf("-------------\n");
    printf("%s",buffer);
    printf("-------------\n");
    //Sends board in buffer to player
    status = write(sock,buffer,256);
    //waits for two players.
    while(playerInfo->count < 1){};
    //Needs to wait for input
     printf("Running game\n");


     bzero(buffer,256);
     
     while(1){
         pid = wait(&status);
         rungame(sock);
         pid = signal(&status);
     }

}

void rungame(int sock){
        int status = read(sock, buffer, 256);
        if(status < 0) printf("READ error\n");
        char letter = buffer[0];
        int index = letter - 'a';
        int i = index/4;
        int j = index%4;
        printf("%c -> %d\n",buffer[0], scoreMap[i][j]);
        gameBoard[i][j] = gameBoardMap[i][j];
        //changes the board to the players choice successfully for one player so far 
        //have not checked out multiplayer yet.
        printBoard(i,j);
        status = write(sock, buffer, 256);
        bzero(buffer,256);
}

void printBoard(int i, int j){
    printf("----------\n");
    for(i = 0; i < 4; i++){
        for(j = 0; j < 4; j++){
            printf("%c ", gameBoard[i][j]);
        }
        printf("\n");
    }
    printf("----------\n");
}

