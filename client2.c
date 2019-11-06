// Peter Stilian, Donald Christensen, Jonathan Sellier, Emily Cardella

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

void main()
{
   int go = 1;
   int  port;
   char host[16];
   int  socketid;      /*will hold the id of the socket created*/
   int  status;        /* error status holder*/
   char buffer[255];   /* the message buffer*/
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
   printf("Enter the server name to connect to (eg osnode02): \n");
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

    // This section takes a ready command from client in order to proceed when there are 2 players
    go = 1;
    printf("Select \"y\" if you are ready. Or \"n\" to exit.\n");
    //gets y or n for player ready
    fgets(buffer, 255, stdin);
    if(buffer[0] == 'n' || buffer[0] == 'N'){
      close(socketid);
    }
    else if(buffer[0] == 'y' || buffer[0] == 'Y'){
       buffer[0] = 'y';
       //writes waiting on other player from server
       status = write(socketid, buffer, 255);
       bzero(buffer,255);
    }

   // Prints the initial Gameboard
   bzero(buffer,255);
   status = read(socketid, buffer, 255);
   if (status < 0){
     printf("error while reading message from server");
     exit(1);
    }

   // Prints letterboard and ask for selection
   printf("%s\nSelect a Letter: \n", buffer);

   // This while loop recieves input and displays up to date letterboard
   while(go){
      bzero(buffer,255);
      fgets(buffer, 255, stdin);
      status = write(socketid, buffer, 255);
      status = read(socketid, buffer, 255);
      printf("%s\nSelect a Letter: \n", buffer);
   }

   /* this closes the socket*/
   close(socketid);
}
