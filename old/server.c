// Peter Stilian, Donald Christensen, Jonathan Sellier, Emily Cardella

#include <string.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

#define PORTNUM  10101 /* the port number the server will listen to*/
#define DEFAULT_PROTOCOL 0  /*constant for default protocol*/
//Initialized a matrix and testMap that needs ints
char gameBoard[4][4]=       {{'A','B','C','D'},
                             {'E','F','G','H'},
                             {'I','J','K','L'},
                             {'M','N','O','P'}};
char gameBoardMap[4][4] =   {{'-','-','-','-'},
                             {'-','-','-','-'},
                             {'-','-','-','-'},
                             {'-','-','-','-'}};
void rungame(int sock);
void doprocessing (int sock);
char buffer[256];

int main( int argc, char *argv[] ) {

    //int client_size[6];
    int sockfd, newsockfd, portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int status, pid;
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
            doprocessing(newsockfd);
            exit(0);
        }
        else {
            close(newsockfd);
        }
    } /* end of while */
    ////////////////Old///////////
}

/*
   Increased buffer size for processing client info using forloops
   taking response using forloops and passing within buffer.
   need to add Mutex or Semaphores and figure out second client.
   Additionaly need to recieve updated matrix from server before mutex or 
   semiphre swap to maintain correct state when entering critical section.
*/
void doprocessing (int sock) {
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
            go = 0;
        }
    }
    //bzero(buffer,1024);
    //status = write(sock, buffer, 1024);

    //Fills buffer with board.
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

    //Needs to wait for input
     printf("Running game\n");
     char letter;
     int index;
     while(1){
         //bzero(buffer,256);
         status = read(sock, buffer, 256);
         if(status < 0) printf("READ error\n");
         printf("Buffer is:%s:", buffer);
         letter = buffer[0];
         index = letter - 'a';
         i = index/4;
         j = index%4;
         printf("%c -> %c\n",buffer[0], gameBoard[i][j]);
     }

    for(i = 0; i < 4; i++){
        for(j = 0; j < 4; j++){
            if(gameBoard[i][j] == buffer[0]){
                gameBoard[i][j] = gameBoardMap[i][j];
            }
        }
    }
 //  Resset Buffer to send back
    buffer[0] = '\0';

    for(i = 0; i < 4; i++){
        for(j = 0; j < 4; j++){
            if( j == 3 ){
                buffer[k] = gameBoard[i][j];
                k++;
                buffer[k] = '\n';
                k++;
                continue;
            }
            buffer[k] = gameBoard[i][j];
            k++;
        }
    }

    if (status < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }

    printf("Here is the Board: \n%s\n",buffer);
    status= write(sock,buffer,256);

    if (status < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }

    rungame(sock);
}

void rungame(int sock){
}