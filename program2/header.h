#ifndef HEADER_H_
#define HEADER_H_

/*Change HEADER_H to whatever */
/* use #DEFINE DEBUG {1 2 3} to get debug */
#define DEBUG 3

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/* file and dir stuffs */
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h> /*grr */
#include <sys/fcntl.h> /*uber grr*/

#include <string.h>

/*threads? */
#include <pthread.h>

/* Room names */

#define FOREACH_ROOM(ROOM) \
        ROOM(blue)   \
        ROOM(red)  \
        ROOM(green)   \
        ROOM(purple)  \
        ROOM(black)  \
        ROOM(orange)  \
        ROOM(yellow)  \
        ROOM(gold)  \
        ROOM(white)  \
        ROOM(pink)  \

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

enum ROOM_ENUM {
    FOREACH_ROOM(GENERATE_ENUM)
};

static const char *ROOM_STRING[] = {
    FOREACH_ROOM(GENERATE_STRING)
};

#define ONID "mouldsm"
#define START_ROOM "START_ROOM"
#define MID_ROOM "MID_ROOM"
#define END_ROOM "END_ROOM"
#define MAX_CONNECTED_ROOMS 7
#define MIN_CONNECTIONS 3
#define MAX_CONNECTIONS 6

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#if defined(DEBUG) && DEBUG > 2
#define info(fmt, args...) fprintf(stderr, ANSI_COLOR_GREEN "DEBUG %s:%d:%s(): " ANSI_COLOR_RESET fmt "\n", __FILE__, __LINE__, __func__, ##args)
#else
#define info(fmt, args...) /* Don't do anything in release builds */
#endif

#if defined(DEBUG) && DEBUG > 1
#define trace(fmt, args...) fprintf(stderr, ANSI_COLOR_YELLOW "DEBUG %s:%d:%s(): " ANSI_COLOR_RESET fmt "\n", __FILE__, __LINE__, __func__, ##args)
#else
#define trace(fmt, args...) /* Don't do anything in release builds */
#endif

#if defined(DEBUG) && DEBUG > 0
#define err(fmt, args...) fprintf(stderr, ANSI_COLOR_RED "DEBUG %s:%d:%s(): " ANSI_COLOR_RESET fmt "\n", __FILE__, __LINE__, __func__, ##args)
#else
#define err(fmt, args...) /* Don't do anything in release builds */
#endif

int  _test_debug(int argc, char **args) {
  int i = 10;
  info("Welcome");
  info("Here is some info");
  err("oops an error, i was = %d", i);
  trace("Debugging is enabled.");
  trace("Debug level: %d", i);
  
  /* Some time demo stuff */
  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  info( "Current local time and date: %s", asctime (timeinfo) );
  /* testing the enum stuffs */
  trace("enum orange as a string: %s\n",ROOM_STRING[orange]);
  return 0;
}

#endif
