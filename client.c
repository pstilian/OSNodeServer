// Peter Stilian, Donald Christensen, Jonathan Sellier, Emily Cardella

//Fixed issue with input being repeated
//Created Print function to clean up code


/* this program shows how to create sockets for a client.
it also shows how the client connects to a server socket.
and sends a message to it. the server must already be running
on a machine. The name of this machine must be entered in the function
gethostbyname in the code below. The port number where the server is listening is
specified in PORTNUM. This port number must also be specified in the server code.
 * main program */
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

void endGame(int sock){
   printf("Game end");
}

//Print board function
void printBoard(char playerMove){
   int i = 0;
   int j = 0;
   for(i = 0; i < 4; i++){
         for(j = 0; j < 4; j++){
            if(gameBoard[i][j] == playerMove){
               gameBoard[i][j] = '-';
            }
            printf(" %c ", gameBoard[i][j]);
         }
         printf("\n");
      }
}
void main()
{
   int  port;
   int  socketid;      /*will hold the id of the socket created*/
   int  status;        /* error status holder*/
   char buffer[256];   /* the message buffer*/
   struct sockaddr_in serv_addr;
   struct hostent *server;
   /* this creates the socket*/
   socketid = socket (AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);
   if (socketid < 0) {
      printf( "error in creating client socket\n");
      exit (-1);
    }
    printf("created client socket successfully\n");
   /* before connecting the socket we need to set up the right         values in
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
   status = connect(socketid, (struct sockaddr *) &serv_addr,
                            sizeof(serv_addr));
   if (status < 0){
      printf( " error in connecting client socket with server    \n");
      exit(-1);
   }
    printf("connected client socket to the server socket \n");
   /* now lets send a message to the server. the message will be
   whatever the user wants to write to the server.*/

   /* Read server response */
   bzero(buffer,256);

   char m[5];
   int go = 1;
   //check if user is ready
   while(go) {
      printf("press y if you are ready. ");
      while ((getchar()) != '\n'); 
      scanf("%c", &buffer);
      if(buffer[0] == 'y' || buffer[0] == 'Y'){
         buffer[0] = 'y';
         status = write(socketid, buffer, 256);
         go = 0;
      }
   }

   //bzero(buffer,256);
   
   //prints buffer
   printf("-------------\n");
   printf("%s",buffer);
   printf("-------------\n");
   int score = 0; //idk why it jus tworks
   status = read(socketid, buffer, 256);
   go = 1;
   while(go){
      bzero(buffer,255);
      while ((getchar()) != '\n');
      printf("%s\nSelect a Letter: \n", buffer);
      scanf("%c", &buffer);
      printBoard(buffer[0]);
      status = write(socketid, buffer, 255);
      status = read(socketid, buffer, 256);
      if(buffer[0] == 1) {
         score+= buffer[1];
         printf("Score: %d\n", score);
         if(buffer[2]>=16){
            go = 0;
         }
      } else {
         printf("%s\n", buffer);
      }
   }
   
   endGame(socketid);
   close(socketid);
   /* this closes the socket*/
}



