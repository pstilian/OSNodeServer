// Peter Stilian, Donald Christensen, Jonathan Sellier, Emily Cardella

//Fixed issue with input being repeated
//Created Print function to clean up code


/* this program shows how to create sockets for a client.
it also shows how the client connects to a server socket.
and sends a message to it. the server must already be running
on a machine. The name of this machine must be entered in the function
gethostbyname in the code below. The port number where the server is listening is
specified in PORTNUM. This port number must also be specified in the server code. */
/* main program */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#define PORTNUM  10101 /* the port number that the server is listening to*/
#define DEFAULT_PROTOCOL 0  /*constant for default protocol*/

char gameBoard[4][4]=       {{'a','b','c','d'},
                             {'e','f','g','h'},
                             {'i','j','k','l'},
                             {'m','n','o','p'}};
//redeclered as a global
char buffer[256];

void endGame(int sock){
   printf("Game end");
}

//Print board function
void printBoard(){
   int i = 0;
   int j = 0;
   for(i = 0; i < 4; i++){
         for(j = 0; j < 4; j++){
            printf(" %c ", gameBoard[i][j]);
         }
         printf("\n");
      }
}

void main()
{
   int  port;
   int  socketid; //will hold the id of the socket created
   int  status; //error status holder
   struct sockaddr_in serv_addr;
   struct hostent *server;
   /* this creates the socket*/
   socketid = socket (AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);
   if (socketid < 0) {
      printf( "error in creating client socket\n");
      exit (-1);
    }
    printf("created client socket successfully\n");
   /* before connecting the socket we need to set up the right values in
   the different fields of the structure server_addr
   you can check the definition of this structure on your own*/
   char host[16];
   printf("Enter the server name to connect to: (eg osnode02)\n");
   scanf("%s", &host);
   server = gethostbyname(host);
   if (server == NULL){
      printf(" error trying to identify the machine where the server is running\n");
      exit(0);
   }
   port = PORTNUM;
   /*This function is used to initialize the socket structures with null values. */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
   serv_addr.sin_port = htons(port);
   /* connecting the client socket to the server socket */
   status = connect(socketid, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
   if (status < 0){
      printf( " error in connecting client socket with server    \n");
      exit(-1);
   }
    printf("connected client socket to the server socket \n");
   /* now let's send a message to the server. The message will be
   whatever the user wants to write to the server.*/

   /* Read server response */
   bzero(buffer,256);

   char restartGame[5];
   int go = 1;
   //check if user is ready
   while(go) {
      printf("press y if you are ready. ");
      while ((getchar()) != '\n');
      scanf("%c", &buffer);
      if(buffer[0] == 'y' || buffer[0] == 'Y'){
         buffer[0] = 'y';
         status = write(socketid, buffer, 256);
         status = read(socketid,buffer,256);
         go = 0;
      }
   }

   int score = 0; //debugging addition
   char checkBuff;
   status = read(socketid, buffer, 256);
   printBoard();
   go = 1;
   int currGame = 0;
   while(go){
      bzero(buffer,255);
      while ((getchar()) != '\n');
      printf("%s\nSelect a Letter: \n", buffer);
      scanf("%c", &buffer);
      status = write(socketid, buffer, 255);
      status = read(socketid, buffer, 256);

      if(buffer[0] == 1) {
         if(buffer[150] > currGame){ //begin new game if a player won
            go = 0;
            score = 0;
            printf("Game over! Player %d won\n", buffer[151]);
            currGame = buffer[150];
            printf("Enter 1 to keep playing or 0 to exit: ");
            scanf("%d", &go);
            if(go == 1){
              printf("-------------\nA B C D\nE F G H\nI J K L\nM N O P\n-------------\n");
              buffer[2] = 0;
          }
         }
         score+= buffer[1];
         printf("Score: %d\n", score);
         int idx;
         for(idx = 0; idx<16; idx++){
            gameBoard[idx/4][idx%4] = buffer[100+idx];
         }
         printBoard();

      } else if(buffer[0] == 'z'){
            score = 0;
            go = 0;
            currGame = buffer[150];
            //status = read(socketid, buffer, 256);
            printf("Game over! Player %d won\n", buffer[2]);
            printf("Enter 1 to keep playing or 0 to exit: ");
            scanf("%d", &go);
            if(go == 1){
              printf("-------------\nA B C D\nE F G H\nI J K L\nM N O P\n-------------\n");
              buffer[2] = 0;
            }
            if(go == 0){
              buffer[7] = 1;
              status = write(socketid, buffer, 255);
              close(socketid);
              exit(0);
            }
         }
   }
   /* this closes the socket*/
   close(socketid);
}
