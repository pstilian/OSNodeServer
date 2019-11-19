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

int scoreMap[4][4] =        {{4,6,2,1},
                             {0,6,2,3},
                             {8,4,9,1},
                             {6,6,8,9}};


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

    // Create forks for each new client
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
    }
}

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

    // Print the board on server
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

     //buffer needs ot be zeroed
     bzero(buffer,256);

     // Read 
     while(1){
         status = read(sock, buffer, 256);
         if(status < 0) printf("READ error\n");
         letter = buffer[0];
         index = letter - 'a';
         i = index/4;
         j = index%4;
         //Trying here
         if(gameBoard[i][j] != '-'){
            printf("%c -> %d\n",buffer[0], scoreMap[i][j]);
            gameBoard[i][j] = '-';
            status = write(sock, buffer, 256);
            bzero(buffer,256);
         }
         else
         {
            printf("%c -> %d\n",buffer[0], scoreMap[i][j]);
            status = write(sock, buffer, 256);
            bzero(buffer,256);
         }
         
         
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
