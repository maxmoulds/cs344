//keygen.c 
//This program creates a key file of specified length. The characters
//in the file generated will be any of the 27 allowed characters, generated
//using the standard UNIX randomization methods. Do not create spaces every five
//characters, as has been historically done. Note that you specifically do not
//have to do any fancy random number generation: we're not looking for
//cryptographically secure random number generation! rand() is just fine. The
//last character keygen outputs should be a newline. All error text must be
//output to stderr, if any.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DEBUG 1
#define MAX_KEY_LENGTH 1024
#define CHARSET 27 //space is @ ?
#define ASCII_OFFSET 64 

int keygen(int key_length);

#ifdef DEBUG
int main(int argc, char ** argv) {
  if (argc <= 1 || argv[1] == NULL)
  {
    printf("keygen: no arguments given\n");
    //printf("%d\n", argc);
    return -1;
  }
  else {
    return keygen(strtol(argv[1],NULL, 10));
  }
}
#endif

int keygen(int key_length) {
  if (key_length >= MAX_KEY_LENGTH) {
    _Exit(1);
  }
  int charcount;
  srand(time(NULL));
  for (charcount = 0; charcount < key_length; )
  {
    charcount += printf("%c",(rand() % CHARSET)+ASCII_OFFSET);
  }
  printf("\n");
  return 0;
}
