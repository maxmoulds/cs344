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
#define OTP_ENC_INT 1
#define OTP_ENC_D_INT 2

#define HOSTNAME "localhost"

int otp_enc_d(int port);
int encrypt(char * message, int message_length,  char * key, int key_length, char * ciphertext, int ciphertext_length);

void error(const char *msg, int err) { 
  fprintf(stderr, "%d: %s\n", err, msg); exit(err); 
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

  char keybuf[MAX_READ+1];
  memset(keybuf, -1, sizeof(keybuf));
  keybuf[MAX_READ] = '\0';
  char ciphertext[MAX_READ+1];
  memset(ciphertext, -1, sizeof(ciphertext));
  ciphertext[MAX_READ] = '\0';
  char response[MAX_READ+1+(sizeof(int)/sizeof(char))+1];
  memset(response, -1, sizeof(response));
  response[sizeof(response)-1] = '\0';
  char * buffer = (response+sizeof(int)/sizeof(char));//&(response[MAX_READ+1]);
  memset(buffer, -1, MAX_READ);
  //buffer[sizeof(buffer)-1] = NULL; //redundant if all is good but doesnt hurt. 
  buffer[MAX_READ] = '\0';//oo ima bad boy
  //printf("buffer + sizeof(int)/sizeof(char) : %lu : \n", (sizeof(int)/sizeof(char)));
  int * id = response;

  //buffer += (sizeof(int)/sizeof(char));//u kno structs are cooler. but so are floating points...
  *id = 10; 
  printf("id is %d\n *buffer[0] = %p, *buffer[MAX_READ] = %p, message = %lu, keybuf = %lu, and ciphertext = %lu\n", *id, buffer, (&(buffer[MAX_READ-1])), sizeof(response), sizeof(keybuf), sizeof(ciphertext));
  printf("CHECK:  %x (hex), %lu, should be %d\n", ((&(buffer[MAX_READ-1]))-(&(buffer[0]))), ((&(buffer[MAX_READ-1]))-(((buffer)))), MAX_READ);
  printf("CHECK2:  %x (dhex), %lu, should be %d\n", ((&(response[MAX_READ+2+(sizeof(int)/sizeof(char))]))-(&(response[0]))), ((&(response[MAX_READ+2+(sizeof(int)/sizeof(char))]))-(((response)))), MAX_READ+2+(sizeof(int)/sizeof(char)+1));
  //char ciphertext[MAX_READ+1];
  printf("main = %p, id = %p, response = %p , response[max-read+dfs] = %p, sizeofid = %d,\n", main, id, response, &(response[MAX_READ+2+(sizeof(int)/sizeof(char))]), sizeof(*id)); 
  printf("strlent buffer = %d,sizeof buffer = %d,  buffer[0] = %d, buffer[sizeofbuffer-1] = %d, buffer[maxread[ = %d ,\n", strlen(buffer), sizeof(buffer), buffer[0], buffer[sizeof(buffer)-1], buffer[MAX_READ-1]);


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
  //char * keybuf = calloc(MAX_READ+1, sizeof(char));
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
      memset(ciphertext, '\0', sizeof(ciphertext));
      (charsRead = recv(establishedConnectionFD, response, sizeof(response), 0) > 0); // Read the client's message from the socket
      if (charsRead < 0) error("ERROR reading from socket", ERROR_PORT);
      printf("SERVER: I received this from the client(%d): \"%s\"\n", *id, buffer);
      if ((*id) != OTP_ENC_INT) 
      {
        //do i want to exit or just log and keep accepting. hmmm. lets do both
        fprintf(stderr, "ERROR: rejected connection from %d - not otp_enc trying to connect\n", *id);
        //close fd?
        snprintf(ciphertext, 61, "ERROR: rejected connection - not otp_enc trying to connect");
        *id = ERROR;
        //continue;
        //error("ERROR: rejected - not otp_enc trying to connect", ERROR_PORT);
      }
      else 
      {
        total += charsRead;
        *id = OTP_ENC_D_INT;
        encrypted_chars += encrypt(buffer, strlen(buffer), keybuf, strlen(keybuf), ciphertext, sizeof(ciphertext));
        printf("SERVER: Encrypted text is : %s :\n", ciphertext);
      }
      // Send a Success message back to the client
      memset(buffer, '\0', MAX_READ);
      memcpy(buffer, ciphertext, MAX_READ);

      printf("SERVER: sending :: %d - %s\n", *id, buffer);
      //snprintf(buffer, MAX_READ-1, "I am the server, and I got your WHOLE message of %zd chars", encrypted_chars);
      charsRead = send(establishedConnectionFD, response, sizeof(response), 0); // Send success back
      if (charsRead < 0) error("ERROR writing to socket", ERROR_PORT);
      //memset(buffer, '\0', sizeof(buffer)); //ready buffer for next message
      //memset(ciphertext, '\0', sizeof(ciphertext));
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
  ciphertext[i] = '\0';
  return i;
}


