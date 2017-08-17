//THIS IS THE CLIENT FOR OTP_ENC_D.C
//otp_enc.c
//Usage:
//> otp_end <plaintext> <key> <port>
//: <plaintest> is a file name for 
//: <key> is a file name for key
//: <port> is the port that otp_enc should attempt to connect to otp_enc_d on
//

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
#define MAX_READ 1023
#define SPACE " "
#define CHARSET_NUM 27
#define ASCII_OFFSET 64
#define OTP_ENC_INT 1 //testing should be 1
#define OTP_ENC_KEYFILE_INT 11
#define OTP_ENC_D_INT 2
#define OTP_ENC_D_KEYFILE_RESPONSE_INT 21
#define OTP_DEC_INT 3
#define OTP_DEC_D_INT 4

#define HOSTNAME "localhost"

int otp_enc(char * input_filename, char * key_filename, int port);

void * mymalloc(size_t num, size_t size, int * id, char * buffer);
void myfree(int * id, char * buffer);
void error(const char *msg, int err) { fprintf(stderr, "%s", msg); exit(err); } // Error function used for reporting issues

#ifdef DEBUG
int main(int argc, char ** argv) {
  if (argc < 3) { fprintf(stderr,"USAGE: %s plaintext key port\n", argv[0]); exit(0); } // Check usage & args

  return otp_enc(argv[1], argv[2], atoi(argv[3]));
}
#endif

int otp_enc(char * input_filename, char * key_filename, int port) {
  //lets use client.c and server.c given to us. 
  //check file lengths
  FILE * fp;
  FILE * kp;
  FILE * cp; 
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
    //error("Error: key file size is smaller than file size", ERROR_BAD_CHAR);
    fprintf(stderr, "Error: key file size %d is smaller than input file \'%s\' size %d", key_file_size, input_filename, input_file_size);
    exit(ERROR_BAD_CHAR);
  }
  //fclose(kp);
  int socketFD, portNumber, charsWritten, charsRead;
  struct sockaddr_in serverAddress;
  struct hostent* serverHostInfo;
  //char * buffer = calloc(input_file_size+10, sizeof(char));
  fd_set set;
  int result;
  struct timeval tv;
  tv.tv_sec = 5;
  tv.tv_usec = 100 * 1000;

  char message[MAX_READ+1+(sizeof(int)/sizeof(char))+1];
  memset(message, -1, sizeof(message));
  message[MAX_READ] = '\0';
  char * buffer = (message+sizeof(int)/sizeof(char));//&(response[MAX_READ+1]);
  memset(buffer, -1, MAX_READ);
  buffer[MAX_READ] = '\0';
  //buffer = (((int)buffer)+1);
  //printf("buffer + sizeof(int)/sizeof(char) : %lu : \n", (sizeof(int)/sizeof(char)));
  int * id = (void*)message;
  *id = OTP_ENC_INT;
  int keyfile_sent = 0;
  //buffer += (sizeof(int)/sizeof(char));//u kno structs are cooler. but so are floating points...
  //*id = 10; 
  //printf("id is %d\n *buffer[0] = %p, *buffer[MAX_READ] = %p, message = %lu\n", *id, buffer, (&(buffer[MAX_READ-1])), sizeof(message)); 
  //printf("CHECK:  %x (hex), %lu, should be %d\n", ((&(buffer[MAX_READ]))-(&(buffer[0]))), ((&(buffer[MAX_READ]))-(((buffer)))), MAX_READ);
  //printf("main = %p, id = %p, message = %p , message[max-read] = %p, sizeofid = %d,\n", main, id, message, &(message[MAX_READ-1]), sizeof(*id)); 


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
  //set cp to fp first... idk why. 
  cp = fp;
  result = 1;
SEND:
  if (keyfile_sent == 0)
  {
    cp = kp;
  }
  while (!feof(cp))
  {
//    do {
//      FD_ZERO (&set);
//      FD_SET (socketFD, &set);
//      result = select(FD_SETSIZE, &set, NULL, NULL, &tv);
//      //result = select(cp + 1, &set, NULL, NULL, NULL);
//    }
//    while (result == -1 && errno == EINTR);

    if (result == -1) {
      error("CLIENT: error in select", ERROR_PORT); // error occurred in select()
    } else if (result == 0) {
      error("CLIENT: Timeout occurred!  No data after 10.5 seconds.", ERROR_PORT);
    } else {

      if (keyfile_sent == 0)
      {
        //do now...
        //cp = kp;
        *id = OTP_ENC_KEYFILE_INT;
      }
      else 
      {
        //cp = fp;
        *id = OTP_ENC_INT;
      }
      memset(buffer, '\0', MAX_READ); // Clear out the buffer array
      if (fgets(buffer, MAX_READ - 1, cp) != NULL)  //stdin); // Get input from the user, trunc to buffer - 1 chars, leaving \0
      {
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds
        printf("CLIENT: read - :: %s ::\n", buffer);
        //sleep(1);
        charsWritten = send(socketFD, message, sizeof(message), 0);
        if (charsWritten < 0) {
          fprintf(stderr, "Err...%s", strerror(errno));
          error("CLIENT: ERROR writing to socket", ERROR_PORT);

        }
          if (charsWritten < sizeof(message)) printf("CLIENT: WARNING: Not all data written to socket!\n");
        // Get return message from server
        memset(buffer, '\0', MAX_READ); // Clear out the buffer again for reuse
        charsRead = recv(socketFD, message, sizeof(message), 0);
        if (charsRead < 0) {
          //error("CLIENT: ERROR reading from socket", ERROR_PORT);
          fprintf(stderr, "Err...%s", strerror(errno));
        error("CLIENT: ERROR reading from socket", ERROR_PORT);
        }
          printf("CLIENT: I (%d) received this from the server: \"%s\"\n", *id, buffer);
        //sleep(1);
      }
    }
       do {
      FD_ZERO (&set);
      FD_SET (socketFD, &set);
      result = select(FD_SETSIZE, NULL, &set, NULL, &tv);
      //result = select(cp + 1, &set, NULL, NULL, NULL);
    }   
    while (result == -1 && errno == EINTR);
 
  }
  if (keyfile_sent == 0)
  {
    printf("CLIENT: keyfile transferred\n");
    keyfile_sent = 1;
    cp = fp;
    goto SEND;
  }
  close(socketFD); // Close the socket
  fclose(fp);
  fclose(kp);
  return 0;
}

//void * mymalloc(size_t num, size_t size, int * id, char * buffer) {
//  buffer = calloc(size
//}

//void myfree(int * id, char * buffer) {

//}
