//THIS IS THE CLIENT FOR OTP_ENC_D.C
//otp_enc.c
//Usage:
//> otp_end <plaintext> <key> <port>
//: <plaintest> is a file name for 
//: <key> is a file name for key
//: <port> is the port that otp_enc should attempt to connect to otp_enc_d on
//
//This program connects to otp_enc_d, and asks it to perform a one-time pad
//style encryption as detailed above. By itself, otp_enc doesn't do the
//encryption - otp_end_d does. The syntax of otp_enc is as follows:
//> otp_end <plaintext> <key> <port>
//: <plaintest> is a file name for 
//: <key> is a file name for key
//: <port> is the port that otp_enc should attempt to connect to otp_enc_d on
//In this syntax, plaintext is the name of a file in the current directory that
//contains the plaintext you wish to encrypt. Similarly, key contains the
//encryption key you wish to use to encrypt the text. Finally, port is the port
//that otp_enc should attempt to connect to otp_enc_d on.
//
//When otp_enc receives the ciphertext back from otp_enc_d, it should output it
//to stdout. Thus, otp_enc can be launched in any of the following methods, and
//should send its output appropriately:
//$ otp_enc myplaintext mykey 57171
//$ otp_enc myplaintext mykey 57171 > myciphertext
//$ otp_enc myplaintext mykey 57171 > myciphertext &
//
//If otp_enc receives key or plaintext files with bad characters in them, or the
//key file is shorter than the plaintext, it should exit with an error, and set
//the exit value to 1. If otp_enc cannot find the port given, it should report
//this error to stderr (not into the plaintext or ciphertext files) with the bad
//port, and set the exit value to 2. Otherwise, on successfully running, otp_enc
//should set the exit value to 0.
//
//otp_enc should NOT be able to connect to otp_dec_d, even if it tries to
//connect on the correct port - you'll need to have the programs reject each
//other. If this happens, otp_enc should report the rejection to stderr and then
//terminate itself.
//
//Again, any and all error text must be output to stderr.

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

int otp_enc(char * input_filename, char * key_filename, int port);
int encrypt(char * message, int message_length, char * key, int key_length, char * ciphertext, int cipertext_length);

void error(const char *msg, int err) { perror(msg); exit(err); } // Error function used for reporting issues

#ifdef DEBUG
int main(int argc, char ** argv) {
  if (argc < 3) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args

  return otp_enc(argv[1], "keyfile", atoi(argv[2]));
}
#endif

int otp_enc(char * input_filename, char * key_filename, int port) {
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

  printf("The file sizes for input are : %d : and for keyfile : %d :\n",input_file_size, key_file_size);
  //input_filename is plaintext. 

  //key_filename is the key file

  //port is the port number

  int socketFD, portNumber, charsWritten, charsRead;
  struct sockaddr_in serverAddress;
  struct hostent* serverHostInfo;
  //char * buffer = calloc(input_file_size+10, sizeof(char));
  char buffer[MAX_READ+1];
  char ciphertext[MAX_READ+1];
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

  //now transfer

  // Get input message from user
  //printf("CLIENT: Enter text to send to the server, and then hit enter: ");

  //read all of keyfile into memory for now. 
  char * keybuf = calloc(key_file_size+1, sizeof(char));
  int encrypted_chars = 0;
  if (fgets(keybuf, sizeof(keybuf)-1, kp) != NULL)
  {
    while (!feof(fp))
    {
      memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
      memset(ciphertext, '\0', sizeof(ciphertext));
      if (fgets(buffer, sizeof(buffer) - 1, fp) != NULL)  //stdin); // Get input from the user, trunc to buffer - 1 chars, leaving \0
      {
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds
        //encrypt line using keyfile. 

        encrypted_chars += encrypt(buffer, strlen(buffer), keybuf, strlen(keybuf), ciphertext, sizeof(ciphertext));
        printf("Encrypted message is : %s :\n", ciphertext);
        // Send message to server
        //charsWritten = send(socketFD, ciphertext, strlen(ciphertext), 0); // Write to the server
        charsWritten = send(socketFD, buffer, strlen(buffer), 0);
        if (charsWritten < 0) error("CLIENT: ERROR writing to socket", ERROR_PORT);
        if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

        // Get return message from server
        memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
        //memset(ciphertext, '\0', sizeof(ciphertext));
        //free(buffer);
        //buffer = calloc(MAX_READ+1, sizeof(char));
        charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
        //charsRead = recv(socketFD, ciphertext, sizeof(ciphertext) - 1, 0); // Read data from the socket, leaving \0 at end
        if (charsRead < 0) error("CLIENT: ERROR reading from socket", ERROR_PORT);
        printf("CLIENT: I received this from the server: \"%s\"\n", buffer);
        //printf("CLIENT: I received this from the server: \"%s\"\n", ciphertext);
      }
      else
      {
        //error with fgets?
        //printf("End..\n");
        //error("fgets bad char?", ERROR_BAD_CHAR);
      }
      //printf("Encrypted chars is : %d\n", encrypted_chars);
    }
    close(socketFD); // Close the socket
  }
  else
  {
    error("keyfile buffer allocation and/or fgets error", ERROR_BAD_CHAR);
  }

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
