#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define DEBUG 1

#define ERROR 1
#define ERROR_PORT 2
#define ERROR_BAD_CHAR 1
#define MAX_READ 1023
#define SPACE ' '
#define CHARSET_NUM 27
#define ASCII_OFFSET 64

#define HOSTNAME "localhost"

int otp_dec_d(int port);
int decrypt(char * ciphertext, int ciphertext_length,  char * key, int key_length, char * message, int message_length);

void error(const char *msg, int err) { 
  perror(msg); exit(err); 
} // Error function used for reporting issues

#ifdef DEBUG
int main(int argc, char ** argv) {
  if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

  return otp_dec_d(atoi(argv[1]));
}
#endif

int otp_dec_d(int port) {
  int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
  socklen_t sizeOfClientInfo;
  char buffer[MAX_READ+1];
  char message[MAX_READ+1];
  struct sockaddr_in serverAddress, clientAddress;
  FILE * kp;
  //if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

  // Set up the address struct for this process (the server)
  memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
  portNumber = port; //atoi(argv[1]); // Get the port number, convert to an integer from a string
  serverAddress.sin_family = AF_INET; // Create a network-capable socket
  serverAddress.sin_port = htons(portNumber); // Store the port number
  serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

  // Set up the socket
  listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
  if (listenSocketFD < 0) error("ERROR opening socket", ERROR_PORT);

  // Enable the socket to begin listening
  if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
    error("ERROR on binding", ERROR_PORT);

  if(listen(listenSocketFD, 5) < 0) // Flip the socket on - it can now receive up to 5 connections
  {
    error("ERROR on listen", ERROR_PORT);
  }
  kp = fopen("keyfile2", "r");
  char * keybuf = calloc(MAX_READ+1, sizeof(char));
  int decrypted_chars = 0;
  ssize_t total = 0;

  if (fgets(keybuf, sizeof(keybuf)-1, kp) != NULL)
  {
    while(1) {
      // Accept a connection, blocking if one is not available until one connects
      sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
      establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
      if (establishedConnectionFD < 0) error("ERROR on accept", ERROR_PORT);
      printf("SERVER: Connected Client at port %d\n", ntohs(clientAddress.sin_port));
      decrypted_chars = 0;
      memset(buffer, '\0', MAX_READ+1);
      total = 0;
      (charsRead = recv(establishedConnectionFD, buffer, MAX_READ, 0) > 0); // Read the client's message from the socket
      if (charsRead < 0) error("ERROR reading from socket", ERROR_PORT);
      total += charsRead;
      //printf("charsread is %d, and total is now : %zd\n", charsRead, total);
      printf("SERVER: I received this from the client: \"%s\"\n", buffer);
      //now encrypt it. 
      decrypted_chars += decrypt(buffer, strlen(buffer), keybuf, strlen(keybuf), message, sizeof(message));
      printf("SERVER: Decrypted text is : %s :\n", message);
      // Send a Success message back to the client
      memset(buffer, '\0', sizeof(buffer));
      snprintf(buffer, sizeof(buffer)-1, "I am the server, and I got your WHOLE message of %zd chars", total);
      charsRead = send(establishedConnectionFD, buffer, strlen(buffer), 0); // Send success back
      if (charsRead < 0) error("ERROR writing to socket", ERROR_PORT);
      //memset(buffer, '\0', sizeof(buffer)); //ready buffer for next message
      memset(message, '\0', sizeof(message));
      //snprintf(buffer, sizeof(buffer)-1, "I am the server, and I got your WHOLE message of %zd chars", total);
      //charsRead = send(establishedConnectionFD, buffer, strlen(buffer), 0); // Send success back
      close(establishedConnectionFD); // Close the existing socket which is connected to the client
    }
  }

  close(listenSocketFD); // Close the listening socket
  fclose(kp);
  //}
  //else 
  //{
  //problem getting keyfile
  // error("problem with keyfile", ERROR_BAD_CHAR);
  //}
  return 0;
  }

int decrypt(char * ciphertext, int ciphertext_length,  char * key, int key_length, char * message, int message_length) {
  int i = 0;
  for (i = 0; i < ciphertext_length; i++)
  {
    //printf("%d, %d\n",(abs((key[i]-ciphertext[i])-ASCII_OFFSET))%CHARSET_NUM + ASCII_OFFSET-3, i);
    message[i] = (abs((key[i] - ciphertext[i])-ASCII_OFFSET) % CHARSET_NUM + ASCII_OFFSET-3);
    if (strncmp(&message[i], "@", 1) == 0) //is space do something
    {   
      message[i] = (char)SPACE; 
    }   
  }
  message[i] = '\0';

  return i;
}


