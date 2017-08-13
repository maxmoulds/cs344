//otp_enc.c
//Usage:
//> otp_end <plaintext> <key> <port>
//: <plaintest> is a file name for 
//: <key> is a file name for key
//: <port> is the port that otp_enc should attempt to connect to otp_enc_d on
//
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>

#define DEBUG 1

int otp_enc(char * input_filename, char * key_filename, int port);

#ifdef DEBUG
int main(int argc, char ** argv) {
  return otp_enc(NULL, NULL, 0);
}
#endif

int otp_enc(char * input_filename, char * key_filename, int port) {
  return 0;
}
