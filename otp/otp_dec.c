//THIS IS THE CLIENT FOR OTP_DEC_D.C
//otp_dec.c
//Usage:
//> otp_dec <ciphertext> <key> <port>
//: <ciphertext> is a file name for the ciphertext
//: <key> is a file name for key
//: <port> is the port that otp_enc should attempt to connect to otp_enc_d on

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h> 

#define DEBUG 1
#define ERROR_PORT 2
#define ERROR_BAD_CHAR 1
#define MAX_READ 255
#define SPACE " "
#define CHARSET_NUM 27
#define ASCII_OFFSET 64

#define HOSTNAME "localhost"

int otp_dec(char * input_filename, char * key_filename, int port);

void error(const char *msg, int err) { perror(msg); exit(err); } // Error function used for reporting issues

#ifdef DEBUG
int main(int argc, char ** argv) {
  if (argc < 3) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args

  return otp_dec(argv[1], "keyfile", atoi(argv[2]));
}
#endif

int otp_dec(char * input_filename, char * key_filename, int port) {
  //lets use client.c and server.c given to us. 
  //check file lengths
  FILE * fp;
  FILE * kp;
  int input_file_size = 0;
  int key_file_size = 0;
  if (!(fp = fopen(input_filename, "r"))) 
  {
    //open failed
    error("input filename open error", ERROR_BAD_CHAR);
  }
  //get length
  fseek(fp, 0L, SEEK_END);
  input_file_size = ftell(fp);
  rewind(fp); 
  if (!(kp = fopen(key_filename, "r")))
  {
    //open failed
    error("keyfile open error", ERROR_BAD_CHAR);
  }
  fseek(kp, 0L, SEEK_END);
  key_file_size = ftell(kp);
  rewind(kp);
  if(key_file_size-1 < input_file_size) 
  {
    //error
    error("key file size is smaller than input file size", ERROR_BAD_CHAR);
  }
  fclose(kp);

  int socketFD, portNumber, charsWritten, charsRead;
  struct sockaddr_in serverAddress;
  struct hostent* serverHostInfo;
  //char * buffer = calloc(input_file_size+10, sizeof(char));
  char buffer[MAX_READ+1];
  //if (argc < 3) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args

  // Set up the server address struct
  memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
  portNumber = port; //atoi(argv[2]); // Get the port number, convert to an integer from a string
  serverAddress.sin_family = AF_INET; // Create a network-capable socket
  serverAddress.sin_port = htons(portNumber); // Store the port number
  serverHostInfo = gethostbyname(HOSTNAME); //argv[1]); // Convert the machine name into a special form of address
  if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
  memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

  // Set up the socket
  socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
  if (socketFD < 0) error("CLIENT: ERROR opening socket", ERROR_PORT);

  // Connect to server
  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
    error("CLIENT: ERROR connecting", ERROR_PORT);
  //while (!feof(fp))
  //{
  memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
  if (fgets(buffer, sizeof(buffer) - 1, fp) != NULL)  //stdin); // Get input from the user, trunc to buffer - 1 chars, leaving \0
  {
    buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds
    charsWritten = send(socketFD, buffer, strlen(buffer), 0);
    if (charsWritten < 0) error("CLIENT: ERROR writing to socket", ERROR_PORT);
    if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");
    // Get return message from server
    memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
    charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
    if (charsRead < 0) error("CLIENT: ERROR reading from socket", ERROR_PORT);
    printf("CLIENT: I received this from the server: \"%s\"\n", buffer);
  }
  //}
  close(socketFD); // Close the socket
  fclose(fp);
  return 0;
}

