
//This program will run in the background as a daemon. Upon execution, otp_enc_d
//must output an error if it cannot be run due to a network error, such as the
//ports being unavailable. Its function is to perform the actual encoding, as
//described above in the Wikipedia quote. This program will listen on a
//particular port/socket, assigned when it is first ran (see syntax below). When
//a connection is made, otp_enc_d must call accept() to generate the socket used
//for actual communication, and then use a separate process to handle the rest
//of the transaction (see below), which will occur on the newly accepted socket.
//
//This child process of otp_enc_d must first check to make sure it is
//communicating with otp_enc (see otp_enc, below). After verifying that the
//connection to otp_enc_d is coming from otp_enc, then this child receives from
//otp_enc plaintext and a key via the communication socket (not the original
//listen socket). The otp_enc_d child will then write back the ciphertext to the
//otp_enc process that it is connected to via the same communication socket.
//Note that the key passed in must be at least as big as the plaintext.
//
//Your version of otp_enc_d must support up to five concurrent socket
//connections running at the same time; this is different than the number of
//processes that could queue up on your listening socket (which is specified in
//the second parameter of the listen() call). Again, only in the child process
//will the actual encryption take place, and the ciphertext be written back: the
//original server daemon process continues listening for new connections, not
//encrypting data.
//
//In terms of creating that child process as described above, you may either
//create with fork() a new process every time a connection is made, or set up a
//pool of five processes at the beginning of the program, before connections are
//allowed, to handle your encryption tasks. As above, your system must be able
//to do five separate encryptions at once, using either method you choose.
//
//Use this syntax for otp_enc_d:
//
//otp_enc_d listening_port
//listening_port is the port that otp_enc_d should listen on. You will always
//start otp_enc_d in the background, as follows (the port 57171 is just an
//example; yours should be able to use any port):
//
//$ otp_enc_d 57171 &
//In all error situations, this program must output errors to stderr as
//appropriate (see grading script below for details), but should not crash or
//otherwise exit, unless the errors happen when the program is starting up (i.e.
//are part of the networking start up protocols like bind()). If given bad
//input, once running, otp_enc_d should recognize the bad input, report an error
//to stderr, and continue to run. Generally speaking, though, this daemon
//shouldn't receive bad input, since that should be discovered and handled in
//the client first. All error text must be output to stderr.
//
//This program, and the other 3 network programs, should use "localhost" as the
//target IP address/host. This makes them use the actual computer they all share
//as the target for the networking connections.
//
//
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
#define SPACE " "
#define CHARSET_NUM 27
#define ASCII_OFFSET 64

#define HOSTNAME "localhost"

int otp_enc_d(int port);
int encrypt(char * message, int message_length,  char * key, int key_length, char * ciphertext, int ciphertext_length);

void error(const char *msg, int err) { 
  perror(msg); exit(err); 
} // Error function used for reporting issues

#ifdef DEBUG
int main(int argc, char ** argv) {
  if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

  return otp_enc_d(atoi(argv[1]));
}
#endif

int otp_enc_d(int port) {
  int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
  socklen_t sizeOfClientInfo;
  char buffer[MAX_READ+1];
  char ciphertext[MAX_READ+1];
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
  int encrypted_chars = 0;
  ssize_t total = 0;

  if (fgets(keybuf, sizeof(keybuf)-1, kp) != NULL)
  {
    while(1) {
      // Accept a connection, blocking if one is not available until one connects
      sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
      establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
      if (establishedConnectionFD < 0) error("ERROR on accept", ERROR_PORT);
      printf("SERVER: Connected Client at port %d\n", ntohs(clientAddress.sin_port));
      encrypted_chars = 0;
      memset(buffer, '\0', MAX_READ+1);
      total = 0;
      (charsRead = recv(establishedConnectionFD, buffer, MAX_READ, 0) > 0); // Read the client's message from the socket
      if (charsRead < 0) error("ERROR reading from socket", ERROR_PORT);
      total += charsRead;
      //printf("charsread is %d, and total is now : %zd\n", charsRead, total);
      printf("SERVER: I received this from the client: \"%s\"\n", buffer);
      //now encrypt it. 
      encrypted_chars += encrypt(buffer, strlen(buffer), keybuf, strlen(keybuf), ciphertext, sizeof(ciphertext));
      printf("SERVER: Encrypted text is : %s :\n", ciphertext);
      // Send a Success message back to the client
      memset(buffer, '\0', sizeof(buffer));
      snprintf(buffer, sizeof(buffer)-1, "I am the server, and I got your WHOLE message of %zd chars", total);
      charsRead = send(establishedConnectionFD, buffer, strlen(buffer), 0); // Send success back
      if (charsRead < 0) error("ERROR writing to socket", ERROR_PORT);
      //memset(buffer, '\0', sizeof(buffer)); //ready buffer for next message
      memset(ciphertext, '\0', sizeof(ciphertext));
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

int encrypt(char * message, int message_length,  char * key, int key_length, char * ciphertext, int ciphertext_length) {
  int i = 0;
  for (i = 0; i < message_length; i++)
  {
    if (strncmp(&message[i], SPACE, 1) == 0) //is space do something
    {   
      ciphertext[i] = ((char) ((((int)'@' + (int)key[i]) % CHARSET_NUM) + ASCII_OFFSET)); 
    }   
    else //add keyfile pos to it. then mod. 
    {   
      ciphertext[i] = ((char) ((((int)message[i] + (int)key[i]) % CHARSET_NUM) + ASCII_OFFSET));
    }   
  }
  return i;
}


